#include <compare>
#include <string>

#include <Resource.hpp>

namespace Resources
{

	void Resource::Load() {
		
		try {
			GetFile()->Open();
			GetFile()->Load();
			//OS::LogInfo("/resources/",
				//{ "Resource %s was loaded", GetPath().string().c_str() });
		}
		catch (const OS::Exception& error) {
			const std::string logMessage = "Error while loading resource: " + error.GetMessage() + " Resource skipped.";
			OS::LogError("/resources/", logMessage.c_str());
		}
	}

	void Resource::Unload() {
		GetFile()->Unload();

	}

	//void ResourceManager::ProcessFiles(const std::filesystem::path& path, const std::function<void(const std::filesystem::path&)>& callback) {
		/*for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
			if (std::filesystem::is_regular_file(entry)) {
				callback(entry);
			}
		}*/
		//}
		//void ResourceManager::Load() {

		//	auto processFile = [this](const std::filesystem::path& path) {
		//		LoadResource(path);
		//		};

		//	ProcessFiles(GetResourcesPath(), processFile);
		//}

		//void ResourceManager::LoadResource(const std::filesystem::path& path) {
		//	auto resource = std::make_shared<Resource>(path);
		//	resource->Load();
		//	const auto resourcePath = resource->GetPath();
		//	const auto resourceFileName = resourcePath.filename().string();
		//	resources_.insert({ resourceFileName, resource });
		//}

		//std::shared_ptr<Resource> ResourceManager::GetResource(const Resource::Identifier& resourceId) {

		//	return resources_[resourceId];

		//}

		//void ResourceManager::ProcessFiles(const std::filesystem::path& path, const std::function<void(const std::filesystem::path&)>& callback) {
		//	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		//		if (std::filesystem::is_directory(entry)) {
		//			ProcessFiles(entry, callback);
		//		}
		//		else if (std::filesystem::is_regular_file(entry)) {
		//			callback(entry);
		//		}
		//	}
		//}

		//const std::filesystem::path& ResourceManager::GetResourcesPath() const { return resourcesPath_; };

}