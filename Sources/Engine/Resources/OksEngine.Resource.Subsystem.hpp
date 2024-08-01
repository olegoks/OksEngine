#pragma once

#include <memory>
#include <future>

#include <Resource.hpp>
#include <Common/OksEngine.Subsystem.hpp>
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

			Resource() = default;

			Resource(const Common::Byte* data, Common::Size size) noexcept {
				data_.Resize(size);
				std::memcpy(data_.GetData(), data, size);
			}

			Resource(Resource&& moveResource) : data_{ /*std::move(moveResource.data_)*/} {
				std::swap(data_, moveResource.data_);
			}

			Resource& operator=(Resource&& moveResource) {
				if (this == &moveResource) {
					return *this;
				}

				data_ = std::move(moveResource.data_);

				return *this;
			}

			~Resource() {
				
			}

		private:
			DS::Vector<Common::Byte> data_;
		};

		ResourceSubsystem(Context& context) : Subsystem{ Subsystem::Type::Resource, context } {
			
		}

		Resource ForceGetResource(std::filesystem::path OSResourcePath) {
			Resources::Resource forceResource = resourceSystem_.ForceGetResource(OSResourcePath);
			//OS::AssertMessage(forceResource.IsLoaded(), "Resource must be loaded at the moment.");
			auto file = std::dynamic_pointer_cast<OS::BinaryFile>(forceResource.GetFile());
			return { file->GetData(), file->GetSize() };
		} 

		Resource GetResource(std::filesystem::path resourcePath) {
			std::filesystem::path withoutRootPath;
			for (auto it = ++resourcePath.begin(); it != resourcePath.end(); ++it) {
				withoutRootPath /= *it;
			}
			resourceSystem_.LoadResource(resourcePath);
			Resources::Resource resource = resourceSystem_.GetResource(resourcePath);
			auto binaryFile = std::dynamic_pointer_cast<OS::BinaryFile>(resource.GetFile());
			Resource engineResource(binaryFile->GetData(), binaryFile->GetSize());
			return engineResource;
		}

		void SetRoots(const std::vector<std::filesystem::path>& rootPaths) {
			rootPaths_ = rootPaths;
			try {
				for(const auto& rootPath : rootPaths){
					for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
						if (std::filesystem::is_regular_file(entry)) {
							resourceSystem_.AddResource(entry.path().filename().string(), entry.path(), "Root");
						}
					}
				}
				
			} catch (std::exception error) {
				OS::LogInfo("ResourceSystem", error.what());
			}
		}

		virtual void Update() override {
		}

	private:
		std::vector<std::filesystem::path> rootPaths_;
		Memory::AllocationCallbacks allocation_callbacks_;
		Resources::ResourceSystem resourceSystem_;
	};


	class AsyncResourceSubsystem {
	public:

		enum class TaskType : Common::UInt8 {
			GetResource,
			LoadResource,
			SetRoots,
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

		struct SetRootsTask : public ResourceSystemTask {
			SetRootsTask(const std::vector<std::filesystem::path>& roots) :
				ResourceSystemTask{ TaskType::SetRoots },
				roots_{ roots } {}
			std::vector<std::filesystem::path> roots_;
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
				static std::atomic<Id> lastTaskId_ = 0;
				return lastTaskId_.fetch_add(1, std::memory_order_relaxed) + 1;
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
			//OS::LogInfo("Engine/Render", { "Task added to MT system. sender subsystem type: %d", subsystemType });
			return AddTask(
				subsystemType,
				Subsystem::Type::Resource,
				CreateTask<GetResourceTask>(resourcePath)
			);
		}

		Task::Id SetRoot(Subsystem::Type subsystemType, std::vector<std::filesystem::path> rootPaths) {
			//OS::LogInfo("Engine/Render", { "Task added to MT system. sender subsystem type: %d", subsystemType });
			return AddTask(
				subsystemType,
				Subsystem::Type::Resource,
				CreateTask<SetRootsTask>(rootPaths)
			);
		}

		[[nodiscard]]
		Task WaitForTask(Subsystem::Type receiver, Task::Id taskId) {
			Subsystem::Type sender = Subsystem::Type::Undefined;
			auto dataInfo = exchangeSystem_.WaitForData(receiver, [&taskId, &sender](const MTDataExchangeSystem<Task, Subsystem::Type>::DataInfo& dataInfo) {
				//if (dataInfo.data_.GetId() == taskId) {
				sender = dataInfo.sender_;
					return true;
				//}
				//return false;
				});
			OS::LogInfo("Engine/AsyncResourceSubsystem",
				{ "Task with id {} was waited by {} from {}.",
					taskId,
					magic_enum::enum_name(receiver).data(),
					magic_enum::enum_name(sender).data()
				});

			return std::move(dataInfo);
		}

		ResourceSubsystem::Resource GetResource(Subsystem::Type receiverSubsystem, Task::Id taskId) {
			//OS::LogInfo("Engine/Render", "Waiting for task.");
			Task task = WaitForTask(receiverSubsystem, taskId);
			//OS::LogInfo("Engine/Render", "Task was got.");
			GetResourceResult getResourceResult = std::move(task.GetData<GetResourceResult>());
			return std::move(getResourceResult.resource_);
		}

		[[nodiscard]]
		Task::Id AddTask(Subsystem::Type senderType, Subsystem::Type receiverType, Task&& task) {
			const Task::Id taskId = task.GetId();
			OS::LogInfo("Engine/AsyncResourceSubsystem",
				{ "Task with id {} was sent from {} to {} subsystem by Update function.",
					taskId,
					magic_enum::enum_name(senderType).data(),
					magic_enum::enum_name(receiverType).data()
				});

			exchangeSystem_.AddData(senderType, receiverType, std::move(task));
			return taskId;
		}

		[[nodiscard]]
		bool GetTask(Subsystem::Type receiver, Task& task, std::function<bool(Subsystem::Type sender, const DS::Vector<Subsystem::Type>& receivers, const Task& task)> filter) {
			Subsystem::Type sender = Subsystem::Type::Undefined;
			//task = exchangeSystem_.WaitForData(receiver,
			//	[&filter, &sender](const MTDataExchangeSystem<Task, Subsystem::Type>::DataInfo& dataInfo)->bool {
			//		const bool isSuitable = filter(dataInfo.sender_, dataInfo.receivers_, dataInfo.data_.GetData<Task>());
			//		if (isSuitable) {
			//			sender = dataInfo.sender_;
			//			return true;
			//		}
			//		return false;
			//	});
			const bool isGot = exchangeSystem_.TryGetData(receiver, task,
				[&filter, &sender](const MTDataExchangeSystem<Task, Subsystem::Type>::DataInfo& dataInfo)->bool {
					const bool isSuitable = filter(dataInfo.sender_, dataInfo.receivers_, dataInfo.data_.GetData<Task>());
					if(isSuitable){
						sender = dataInfo.sender_;
						return true;
					}
					return false;
				});
			if(isGot) {
				OS::LogInfo("Engine/AsyncResourceSubsystem",
					{ "Task with id {} was received by {} from {}.",
						task.GetId(),
						magic_enum::enum_name(receiver).data(),
						magic_enum::enum_name(sender).data()
					});
			}
			return isGot;
		}

		template<class Type, class ...Args>
		[[nodiscard]]
		Task CreateTask(Task::Id taskId, Args&& ... args) {
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
				using namespace std::chrono_literals;
				if (!isGot) { std::this_thread::sleep_for(100ms); continue; }
				//OS::LogInfo("Engine/ResourceSubsystem", { "Task with id %d was received from %d subsystem", task.GetId(), taskSender });
				ResourceSystemTask& resourceSystemTask = task.GetData<ResourceSystemTask>();
				const TaskType taskType = resourceSystemTask.GetType();
				switch (taskType) {
				case TaskType::GetResource: {
					const GetResourceTask& getResourceTask = task.GetData<GetResourceTask>();
					ResourceSubsystem::Resource resource;
					if (getResourceTask.path_.string().starts_with("Root")) {
						resource = std::move(resourceSubsystem_->GetResource(getResourceTask.path_));
					} else {
						resource = resourceSubsystem_->ForceGetResource(getResourceTask.path_);
					}
					const Task::Id taskId = AddTask(
						Subsystem::Type::Resource, taskSender,
						CreateTask<GetResourceResult>(task.GetId(), std::move(resource)));

					break;
				}
				case TaskType::SetRoots: {
					const SetRootsTask& setRootTask = task.GetData<SetRootsTask>();
					resourceSubsystem_->SetRoots(setRootTask.roots_);
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