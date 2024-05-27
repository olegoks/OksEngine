#pragma once

#include <memory>
#include <future>

#include <Resource.hpp>
#include <OksEngine.Subsystem.hpp>
#include <OksEngine.TaskSystem.hpp>

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

		ResourceSubsystem(Context& context) : Subsystem{ context } {
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
			TaskType GetType() { return type_; }
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
				static_assert(sizeof(Type) <= sizeof(memory));
				return *Memory::Construct<Type>(reinterpret_cast<Type*>(&memory), std::forward<Args>(args)...);
			}

			template<class Type>
			Type& GetData() {
				return *reinterpret_cast<Type*>(&memory);
			}

			template<class Type>
			void Destruct() {
				static_assert(sizeof(Type) <= sizeof(memory));
				return Memory::Destruct<Type>()(&memory);
			}

			[[nodiscard]]
			Id GetId() const noexcept {
				return id_;
			}

			Task& operator=(const Task& copyTask) {
				if (this == &copyTask) {
					return *this;
				}

				senderSubsystem_ = copyTask.senderSubsystem_;
				receiverSubsystem_ = copyTask.receiverSubsystem_;

				id_ = copyTask.id_;

				std::memcpy(memory, copyTask.memory, sizeof(memory));

				return *this;
			}

			Subsystem::Type senderSubsystem_ = Subsystem::Type::Undefined;
			Subsystem::Type receiverSubsystem_ = Subsystem::Type::Undefined;
		private:
			Id id_ = 0;
			Common::Byte memory[5 * Common::cacheLineSize_ - sizeof(Id) - 2 * sizeof(Subsystem::Type)] = { 0 };
		};

		static_assert(sizeof(Task) == 5 * Common::cacheLineSize_);

		[[nodiscard]]
		Task::Id GetResource(Subsystem::Type subsystemType, std::filesystem::path resourcePath) {
			AddTask(
				subsystemType,
				Subsystem::Type::Resource,
				CreateTask<GetResourceTask>(resourcePath)
			);
		}

		[[nodiscard]]
		Task::Id AddTask(Subsystem::Type senderType, Subsystem::Type receiverType, Task&& task) {
			const Task::Id taskId = task.GetId();
			exchangeSystem_.AddData(senderType, receiverType, std::move(task));
			return taskId;
		}

		[[nodiscard]]
		std::optional<Task> GetTask(Subsystem::Type senderType, Subsystem::Type receiverType) {

			auto maybeResultTaskDataInfo = exchangeSystem_.GetData(senderType, receiverType);
			if (!maybeResultTaskDataInfo.has_value()) {
				return std::optional<Task>{};
			}

			auto resultTaskDataInfo = maybeResultTaskDataInfo.value();
			resultTaskDataInfo.data_.senderSubsystem_ = resultTaskDataInfo.senderThreadType_;
			resultTaskDataInfo.data_.receiverSubsystem_ = resultTaskDataInfo.receiverThreadType_;
			return resultTaskDataInfo.data_;
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


		void Loop()
		{
			while (isRunning_){
				auto maybeTask = exchangeSystem_.GetData(Subsystem::Type::Render, Subsystem::Type::Resource);
				if (maybeTask.has_value())
				{
					auto dataInfo = maybeTask.value();
					Task& task = dataInfo.data_;
					ResourceSystemTask& resourceSystemTask = task.GetData<ResourceSystemTask>();
					const TaskType taskType = resourceSystemTask.GetType();
					switch (taskType) {
					case TaskType::GetResource: {
						const GetResourceTask& getResourceTask = task.GetData<GetResourceTask>();
						auto resource = resourceSubsystem_->GetResource(getResourceTask.path_);
						const Task::Id taskId = AddTask(Subsystem::Type::Resource,
							dataInfo.senderThreadType_,
							CreateTask<GetResourceResult>(std::move(resource)));
						break;
					}
					default: {
						OS::NotImplemented();
					}
					};
				}
				exchangeSystem_.Update();
			}
		}

	protected:
		MTDataExchangeSystem<Task, Subsystem::Type> exchangeSystem_;
	private:
		 
		std::atomic<bool> isRunning_ = false;
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