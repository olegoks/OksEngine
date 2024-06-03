#pragma once

#include <memory>
#include <future>

#include <Resource.hpp>
#include <OksEngine.Subsystem.hpp>
#include <OksEngine.TaskSystem.hpp>

#include <Windows.h>

namespace OksEngine {


	class ResourceSubsystem : public Subsystem {
	public:

		class Resource {
		public:

			template<class Type>
			const Type* GetData() {
				return static_cast<Type*>(data_.GetData());
			}

			[[nodiscard]]
			Common::Size GetSize() const noexcept { return data_.GetSize(); }

			Resource(const Common::Byte* data, Common::Size size) noexcept {
				data_.Resize(size);
				std::memcpy(data_.GetData(), data, size);
			}

			Resource(Resource&& moveResource) : data_{ std::move(moveResource.data_) } {}

		private:
			DS::Vector<Common::Byte> data_;
		};

		ResourceSubsystem(Context& context) : Subsystem{ Subsystem::Type::Resource, context } {
			std::filesystem::path currentPath = std::filesystem::current_path();
			std::filesystem::path resourcesPath = currentPath / "../../Resources";
			for (const auto& entry : std::filesystem::recursive_directory_iterator(resourcesPath)) {
				if (std::filesystem::is_regular_file(entry)) {
					resourceSystem_.AddResource(entry.path().filename().string(), entry.path(), "Root");
				}
			}
			resourceSystem_.LoadAllResources();
		}

		Resource GetResource(std::filesystem::path resourcePath) {
			resourceSystem_.LoadResource(resourcePath);
			Resources::Resource resource = resourceSystem_.GetResource(resourcePath);
			auto binaryFile = std::dynamic_pointer_cast<OS::BinaryFile>(resource.GetFile());
			Resource engineResource(binaryFile->GetData(), binaryFile->GetSize());
			return engineResource;
		}

		virtual void Update() override {
		}

	private:
		Memory::AllocationCallbacks allocation_callbacks_;
		Resources::ResourceSystem resourceSystem_;
	};


	class AsyncResourceSubsystem {
	public:

		enum class TaskType : Common::UInt8 {
			GetResource,
			LoadResource,
			IsResourceLoaded,
			IsResourceExists,
			Undefined
		};

		struct ResourceSystemTask {
			TaskType type_ = TaskType::Undefined;

			[[nodiscard]]
			inline TaskType GetType() const { return type_; }
		};

		struct GetResourceTask : public ResourceSystemTask {
			GetResourceTask(const std::filesystem::path& path) :
				ResourceSystemTask{ TaskType::GetResource },
				path_{ path } {}
			std::filesystem::path path_;
		};

		struct GetResourceResult {
			GetResourceResult(ResourceSubsystem::Resource&& resource) : resource_{ std::move(resource) } {}
			ResourceSubsystem::Resource resource_;
		};

		AsyncResourceSubsystem(Context& context) {
			resourceSubsystem_ = std::make_shared<ResourceSubsystem>(context);
			thread_ = std::jthread{ &AsyncResourceSubsystem::Loop, this };
		}

		class Task {
		public:
			using Id = Common::UInt64;


			Task() : Task{ 0 } {}
			Task(Id id) : id_{ id } { }

			[[nodiscard]]
			static Id GetNewId() {
				static Id lastTaskId_ = 0;
				return ++lastTaskId_;
			}
			template<class Type, class ...Args>
			Type& Construct(Args&& ... args) {
				static_assert(sizeof(Type) <= sizeof(memory_));
				return *Memory::Construct<Type>(reinterpret_cast<Type*>(&memory_), std::forward<Args>(args)...);
			}

			template<class Type>
			Type& GetData() {
				return *reinterpret_cast<Type*>(&memory_);
			}

			template<class Type>
			const Type& GetData() const {
				return *reinterpret_cast<const Type*>(&memory_);
			}

			template<class Type>
			void Destruct() {
				static_assert(sizeof(Type) <= sizeof(memory_));
				return Memory::Destruct<Type>()(&memory_);
			}

			[[nodiscard]]
			Id GetId() const noexcept {
				return id_;
			}

			Task& operator=(const Task& copyTask) {
				if (this == &copyTask) {
					return *this;
				}

				//senderSubsystem_ = copyTask.senderSubsystem_;
				//receiverSubsystem_ = copyTask.receiverSubsystem_;

				id_ = copyTask.id_;

				std::memcpy(memory_, copyTask.memory_, sizeof(memory_));

				return *this;
			}
			struct Hash {
				Common::Size operator()(const Task& task){
					std::string_view bytes{ task.memory_, sizeof(task.memory_)};
					return std::hash<std::string_view>{}(bytes);
				}
			};
			//Subsystem::Type senderSubsystem_ = Subsystem::Type::Undefined;
			//Subsystem::Type receiverSubsystem_ = Subsystem::Type::Undefined;
		private:
			Id id_ = 0;
			Common::Byte memory_[5 * Common::cacheLineSize_ - sizeof(Id) /* - 2 * sizeof(Subsystem::Type) */ ] = {0};
		};

		static_assert(sizeof(Task) == 5 * Common::cacheLineSize_);

		[[nodiscard]]
		Task::Id GetResource(Subsystem::Type subsystemType, std::filesystem::path resourcePath) {
			OS::LogInfo("Engine/Render", "Task added to MT system.");
			return AddTask(
				subsystemType,
				Subsystem::Type::Resource,
				CreateTask<GetResourceTask>(resourcePath)
			);
		}

		Task WaitForTask(Subsystem::Type receiver, Task::Id taskId) {
			auto dataInfo = exchangeSystem_.WaitForData(receiver, [&taskId](const MTDataExchangeSystem<Task, Subsystem::Type>::DataInfo& dataInfo) {
				if (dataInfo.data_.GetId() == taskId) {
					return true;
				}
				return false;
				});
			return std::move(dataInfo.data_);
		}

		ResourceSubsystem::Resource GetResource(Subsystem::Type receiverSubsystem, Task::Id taskId) {
			OS::LogInfo("Engine/Render", "Waiting for task.");
			Task task = WaitForTask(Subsystem::Type::Resource, taskId);
			/*auto task = GetTask(Subsystem::Type::Resource, receiverSubsystem);
			OS::AssertMessage(
				task.has_value(),
				"At the moment task must have value."
			);*/
			OS::LogInfo("Engine/Render", "Task was got.");
			return std::move(task.GetData<ResourceSubsystem::Resource>());
		}

		[[nodiscard]]
		Task::Id AddTask(Subsystem::Type senderType, Subsystem::Type receiverType, Task&& task) {
			const Task::Id taskId = task.GetId();
			exchangeSystem_.AddData(senderType, receiverType, std::move(task));
			return taskId;
		}

		[[nodiscard]]
		bool GetTask(Subsystem::Type receivier, Task& task, std::function<bool(Subsystem::Type sender, const DS::Vector<Subsystem::Type>& receivers, const Task& task)> filter) {
			const bool isGot = exchangeSystem_.GetData(receivier, task,
				[&filter](const MTDataExchangeSystem<Task, Subsystem::Type>::DataInfo& dataInfo)->bool {
					return filter(dataInfo.sender_, dataInfo.receivers_, dataInfo.data_.GetData<Task>());
				});
			return isGot;
		}

		template<class Type, class ...Args>
		[[nodiscard]]
		Task CreateTask(Task::Id taskId, Args&& ... args) {
			const Task::Id newTaskId = Task::GetNewId();
			Task newTask{ taskId };
			newTask.Construct<Type>(std::forward<Args>(args)...);
			return newTask;
		}

		template<class Type, class ...Args>
		[[nodiscard]]
		Task CreateTask(Args&& ... args) {
			const Task::Id newTaskId = Task::GetNewId();
			Task newTask{ newTaskId };
			newTask.Construct<Type>(std::forward<Args>(args)...);
			return newTask;
		}

	private:


		void Loop() {
			HRESULT r;
			r = SetThreadDescription(
				GetCurrentThread(),
				L"Resource system loop thread"
			);
			while (isRunning_) {
				exchangeSystem_.Update();
				Subsystem::Type taskSender = Subsystem::Type::Undefined;
				Task task;
				const bool isGot = GetTask(Subsystem::Type::Resource, task, 
					[&taskSender](
						Subsystem::Type sender,
						const DS::Vector<Subsystem::Type>& receivers,
						const Task& task){
							taskSender = sender;
						return true;
					});
				if (!isGot) { continue; }
				//OS::AssertMessage(
				//	task.receiverSubsystem_ == Subsystem::Type::Resource,
				//	"WTF"
				//);
				ResourceSystemTask& resourceSystemTask = task.GetData<ResourceSystemTask>();
				const TaskType taskType = resourceSystemTask.GetType();
				switch (taskType) {
				case TaskType::GetResource: {
					const GetResourceTask& getResourceTask = task.GetData<GetResourceTask>();
					auto resource = resourceSubsystem_->GetResource(getResourceTask.path_);

					const Task::Id taskId = AddTask(
						Subsystem::Type::Resource, taskSender,
						CreateTask<GetResourceResult>(task.GetId(), std::move(resource)));
					//OS::LogInfo("ResourceSubsystem/GetResource", { "Task completed to load resource "/*, getResourceTask.path_.string().c_str(), task.GetId()*/ });
					break;
				}
				default: {
					OS::NotImplemented();
				}
				};
				
			}
		}



	protected:

		MTDataExchangeSystem<Task, Subsystem::Type> exchangeSystem_;

	private:
		std::atomic<bool> isRunning_ = true;
		std::jthread thread_;
		std::shared_ptr<ResourceSubsystem> resourceSubsystem_ = nullptr;
	};
	//  
	//class ResourceSubsystemAsyncInterface : public ThreadedResourceSubsystem {
	//public:

	//	ResourceSubsystemAsyncInterface(Context& context) : ThreadedResourceSubsystem{ context } {

	//	}


	//	[[nodiscard]]
	//	Task::Id GetResource(Subsystem::Type requestSystem, std::filesystem::path resourcePath) {
	//		/*GetResourceTask* task = new GetResourceTask{ "GetResource", resourcePath};
	//		exchangeSystem_.AddData(requestSystem, Subsystem::Type::Resource, (void*)task);*/

	//		const Task::Id taskId = AddTask(
	//			requestSystem,
	//			Subsystem::Type::Resource,
	//			CreateTask<GetResourceTask>(resourcePath)
	//		);
	//		return taskId;
	//	}
	//	ResourceSubsystem::Resource GetResource(Task::Id taskId) {
	//		Common::Index foundTaskIndex = Common::Limits<Common::Index>::Max();
	//		for (Common::Index i = 0; i < resultTasksCache_.GetSize(); i++) {
	//			if (resultTasksCache_[i].GetId() == taskId) {
	//				foundTaskIndex = i;
	//				break;
	//			}
	//		}
	//		OS::AssertMessage(foundTaskIndex != Common::Limits<Common::Index>::Max(), "");
	//		//if (foundTaskIndex != Common::Limits<Common::Index>::Max()) {
	//			Task task = resultTasksCache_[foundTaskIndex];
	//			GetResourceResult& getResourceResult = task.GetData<GetResourceResult>();
	//			resultTasksCache_.Erase(foundTaskIndex);
	//			return std::move(getResourceResult.resource_);
	//		//}

	//	}

	//	void WaitFor(Subsystem::Type subsystemType, Task::Id taskId) {
	//		while (true) {
	//			auto maybeTask = GetTask(Subsystem::Type::Resource, subsystemType);
	//			if (maybeTask.has_value()) {
	//				Task task = maybeTask.value();
	//				resultTasksCache_.PushBack(task);
	//				if (task.GetId() == taskId) {
	//					return;
	//				}
	//			}
	//		}
	//	}

	//private:
	//	DS::Vector<Task> resultTasksCache_;
	//};






}