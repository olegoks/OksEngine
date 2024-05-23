#pragma once

#include <Resource.hpp>
#include <OksEngine.Subsystem.hpp>
#include <OksEngine.TaskSystem.hpp>

namespace OksEngine {



	struct GetResourceTask {
		std::string name_;
		std::filesystem::path path_;
	};

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

	protected:
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


	class ThreadedResourceSubsystem : public ResourceSubsystem {
	public:

		ThreadedResourceSubsystem(Context& context) : ResourceSubsystem{ context  }
		{
			thread_ = std::thread{ &ThreadedResourceSubsystem::Loop, this };
		}

	private:

		void Loop()
		{
			while(true)
			{
				exchangeSystem_.Update();
				auto task = exchangeSystem_.GetData("Unknown", "resourceSystem");
				if(task.has_value())
				{
					void* taskPointer = task.value();
					{
						std::string* type = (std::string*)taskPointer;
						{
							if(*type == "GetResource")
							{
								OS::LogInfo("Engine/ResourceSystem", { "ResourceSystem receive task %s ", type->c_str() });
							}

						}
					}
					
				}
			}
		}
	protected:
		MTDataExchangeSystem<void*> exchangeSystem_;
	private:
		std::thread thread_;
		
		std::shared_ptr<ResourceSubsystem> resourceSubsystem_ = nullptr;
	};

	class ResourceSubsystemAsyncInterface : public ThreadedResourceSubsystem {
	public:

		ResourceSubsystemAsyncInterface(Context& context) : ThreadedResourceSubsystem{ context }
		{
			
		}

		class Future {
			
		};

		void GetResource(std::filesystem::path resourcePath) {
			GetResourceTask* task = new GetResourceTask{ "GetResource", resourcePath};
			exchangeSystem_.AddData("Unknown", "resourceSystem", (void*)task);
		}


	private:

	};






}