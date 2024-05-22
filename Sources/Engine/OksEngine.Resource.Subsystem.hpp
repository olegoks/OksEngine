#pragma once

#include <Resource.hpp>
#include <OksEngine.Subsystem.hpp>

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

		private:
			DS::Vector<Common::Byte> data_;
		};

		ResourceSubsystem() {
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
			Resources::Resource resource = resourceSystem_.GetResource(resourcePath);
			auto binaryFile = std::dynamic_pointer_cast<OS::BinaryFile>(resource.GetFile());
			Resource engineResource(binaryFile->GetData(), binaryFile->GetSize());
			return engineResource;
		}

		virtual void Update() override {

		}

	private:
		Resources::ResourceSystem resourceSystem_;
	};


}