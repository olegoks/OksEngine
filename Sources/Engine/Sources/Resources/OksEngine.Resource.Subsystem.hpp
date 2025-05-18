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


		ResourceSubsystem(Context& context) : Subsystem{ Subsystem::Type::Resource, context },
			resourceSystem_{
			Resources::ResourceSystem::CreateInfo{
					.fileExtensions_ = {
					".frag", ".vert", ".geom", ".ecs" }
			} } {

		}

		Resources::ResourceData ForceGetResource(std::filesystem::path OSResourcePath) {
			return resourceSystem_.ForceGetResourceData(OSResourcePath);
		}

		Resources::ResourceData GetResource(std::filesystem::path resourcePath) {
			resourceSystem_.LoadResource(resourcePath);
			Resources::Resource resource = resourceSystem_.GetResource(resourcePath);
			auto binaryFile = std::dynamic_pointer_cast<OS::BinaryFile>(resource.GetFile());
			Resources::ResourceData engineResource(binaryFile->GetData(), binaryFile->GetSize());
			return engineResource;
		}

		std::vector<std::filesystem::path> GetAddedResources(const std::vector<std::string>& extensions) {
			std::vector<std::filesystem::path> addedResources;
			resourceSystem_.ForEachAddedResource([&](const std::filesystem::path& resourceData) ->bool {

				bool extensionCorresponds = false;
				for (auto& extension : extensions) {
					if (extension == resourceData.extension().string()) {
						extensionCorresponds = true;
						break;
					}
				}
				if (extensions.empty() || extensionCorresponds) {
					addedResources.push_back(resourceData);
				}
				return true;
				});
			return addedResources;
		}

		void SetRoots(const std::vector<std::filesystem::path>& rootPaths) {
			/*rootPaths_ = rootPaths;
			try {
				for (const auto& rootPath : rootPaths) {
					for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath)) {
						if (std::filesystem::is_regular_file(entry)) {
							resourceSystem_.AddResource(entry.path().filename().string(), entry.path(), "Root");
						}
					}
				}

			}
			catch (std::exception error) {
				OS::LogInfo("ResourceSystem", error.what());
			}*/

			resourceSystem_.SetRoots(rootPaths);
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
			SetRoots,
			IsResourceLoaded,
			IsResourceExists,
			GetAddedResources,
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
			GetResourceResult(Resources::ResourceData&& resource) : resource_{ std::move(resource) } {}
			Resources::ResourceData resource_;
		};

		struct GetAddedResourcesTask : ResourceSystemTask {
			GetAddedResourcesTask(const std::vector<std::string>& extensions) 
				: ResourceSystemTask{ TaskType::GetAddedResources },
				extensions_{ extensions } {}
			std::vector<std::string> extensions_;
		};

		struct GetAddedResourcesResult {
			GetAddedResourcesResult(const std::vector<std::filesystem::path>& addedResources) : addedResources_{ addedResources } {}
			std::vector<std::filesystem::path> addedResources_;

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
				return *reinterpret_cast<const Type*>(memory_);
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
				Common::Size operator()(const Task& task) {
					std::string_view bytes{ task.memory_, sizeof(task.memory_) };
					return std::hash<std::string_view>{}(bytes);
				}
			};
			//Subsystem::Type senderSubsystem_ = Subsystem::Type::Undefined;
			//Subsystem::Type receiverSubsystem_ = Subsystem::Type::Undefined;
		private:
			Id id_ = 0;
			Common::Byte memory_[5 * Common::cacheLineSize_ - sizeof(Id) /* - 2 * sizeof(Subsystem::Type) */] = { 0 };
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

		[[nodiscard]]
		Task::Id GetAddedResources(Subsystem::Type subsystemType, const std::vector<std::string>& extensions) {
			//OS::LogInfo("Engine/Render", { "Task added to MT system. sender subsystem type: %d", subsystemType });
			return AddTask(
				subsystemType,
				Subsystem::Type::Resource,
				CreateTask<GetAddedResourcesTask>(extensions)
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
				if (dataInfo.data_.GetId() == taskId) {
					sender = dataInfo.sender_;
					return true;
				}
				return false;
				});
			//OS::LogInfo("Engine/AsyncResourceSubsystem",
			//	{ "Task with id {} was waited by {} from {}.",
			//		taskId,
			//		magic_enum::enum_name(receiver).data(),
			//		magic_enum::enum_name(sender).data()
			//	});

			return std::move(dataInfo);
		}

		Resources::ResourceData GetResource(Subsystem::Type receiverSubsystem, Task::Id taskId) {
			//OS::LogInfo("Engine/Render", "Waiting for task.");
			Task task = WaitForTask(receiverSubsystem, taskId);
			//OS::LogInfo("Engine/Render", "Task was got.");
			GetResourceResult getResourceResult = std::move(task.GetData<GetResourceResult>());
			return std::move(getResourceResult.resource_);
		}

		std::vector<std::filesystem::path> GetAddedResources(Subsystem::Type receiverSubsystem, Task::Id taskId) {
			//OS::LogInfo("Engine/Render", "Waiting for task.");
			Task task = WaitForTask(receiverSubsystem, taskId);
			//OS::LogInfo("Engine/Render", "Task was got.");
			GetAddedResourcesResult getResourceResult = std::move(task.GetData<GetAddedResourcesResult>());
			return std::move(getResourceResult.addedResources_);
		}

		[[nodiscard]]
		Task::Id AddTask(Subsystem::Type senderType, Subsystem::Type receiverType, Task&& task) {
			const Task::Id taskId = task.GetId();
			/*OS::LogInfo("Engine/AsyncResourceSubsystem",
				{ "Task with id {} was sent from {} to {} subsystem by Update function.",
					taskId,
					magic_enum::enum_name(senderType).data(),
					magic_enum::enum_name(receiverType).data()
				});*/

			exchangeSystem_.AddData(senderType, receiverType, std::move(task));
			return taskId;
		}

		[[nodiscard]]
		bool IsIncomeTaskExist(Subsystem::Type receiver, std::function<bool(Subsystem::Type sender, const DS::Vector<Subsystem::Type>& receivers, const Task& task)> filter) {
			Subsystem::Type sender = Subsystem::Type::Undefined;
			const bool isGot = exchangeSystem_.IsDataExist(receiver, [&filter, &sender](const MTDataExchangeSystem<Task, Subsystem::Type>::DataInfo& dataInfo)->bool {
				const bool isSuitable = filter(dataInfo.sender_, dataInfo.receivers_, *(const Task*)&dataInfo.data_);
				if (isSuitable) {
					return true;
				}
				return false;
				});
			return isGot;
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
					const bool isSuitable = filter(dataInfo.sender_, dataInfo.receivers_, *(const Task*)&dataInfo.data_);
					if (isSuitable) {
						sender = dataInfo.sender_;
						return true;
					}
					return false;
				});
			if (isGot) {
				//OS::LogInfo("Engine/AsyncResourceSubsystem",
				//	{ "Task with id {} was received by {} from {}.",
				//		task.GetId(),
				//		magic_enum::enum_name(receiver).data(),
				//		magic_enum::enum_name(sender).data()
				//	});
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
						const Task& task) {
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
					Resources::ResourceData resource;
					if (getResourceTask.path_.string().starts_with("Root")) {
						resource = std::move(resourceSubsystem_->GetResource(getResourceTask.path_));
					}
					else {
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
				case TaskType::GetAddedResources: {
					const GetAddedResourcesTask& setRootTask = task.GetData<GetAddedResourcesTask>();
					auto addedResources = resourceSubsystem_->GetAddedResources(setRootTask.extensions_);
					const Task::Id taskId = AddTask(
						Subsystem::Type::Resource, taskSender,
						CreateTask<GetAddedResourcesResult>(task.GetId(), std::move(addedResources))
					);
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







}