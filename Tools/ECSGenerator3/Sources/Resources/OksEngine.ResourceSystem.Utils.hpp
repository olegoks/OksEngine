#pragma once 

#include <regex>
#include <filesystem>

#include <Common.Types.hpp>
#include <OS.Assert.hpp>
#include <OS.FileSystem.BinaryFile.hpp>

#define RESOURCE__MANAGER__CREATE_LOAD_RESOURCE_REQUEST(resourcePath)							\
	[this](const std::string& path){													\
		const ECS2::Entity::Id requestEntityId = CreateEntity();								\
		CreateComponent<Resource::Manager::Request::Tag>(requestEntityId);						\
		CreateComponent<Resource::Manager::Request::LoadResource>(requestEntityId, path);		\
		CreateComponent<Resource::Manager::Request::State::Pending>(requestEntityId);			\
		return 	requestEntityId;																\
	}(resourcePath);


#define RESOURCE__CREATE_RESOURCE(resourcePath, resourceData)					\
	[this](const std::string& path, std::vector<Common::Byte> data){	\
		const ECS2::Entity::Id resourceEntityId = CreateEntity();				\
		CreateComponent<Resource::Tag>(resourceEntityId);						\
		CreateComponent<Resource::Path>(resourceEntityId, path);				\
		CreateComponent<Resource::Data>(resourceEntityId, data);				\
		return 	resourceEntityId;												\
	}(resourcePath, resourceData);

#define RESOURCE__MANAGER__IS_REQUEST_FINISHED(requestEntityId)\
	[this](ECS2::Entity::Id entityId){																				\
		const ECS2::ComponentsFilter requestCF = GetComponentsFilter(entityId);										\
		ASSERT_MSG(requestCF.IsSet<Resource::Manager::Request::Tag>(), "Attempt to check state of not request entity");	\
		return requestCF.IsSet<Resource::Manager::Request::State::Finished>();										\
	}(requestEntityId)

namespace OksEngine::Resource {

	static const char* prefix_ = "resource:/";

	[[nodiscard]]
	static inline bool IsResourcePath(const std::string& str) {
		std::regex pattern(R"(^resource:\/([^\/]+\/)*[^\/]+\.[^\/]+$)");
		return std::regex_match(str, pattern);
	}
	[[nodiscard]]
	static inline std::filesystem::path GetRelativePath(const std::string& str) {
		ASSERT(Resource::IsResourcePath(str));
		std::filesystem::path path = str.substr(std::strlen(prefix_) - 1);
		return path.relative_path();
	}

	[[nodiscard]]
	static inline std::vector<Common::Byte> LoadFileAndGetContent(const std::string& resourcePath) {
		BEGIN_PROFILE("Loading resource %s", resourcePath);
		ASSERT(std::filesystem::exists(resourcePath));
		ASSERT(std::filesystem::is_regular_file(resourcePath));
		auto file = std::make_unique<OS::BinaryFile>(resourcePath);
		file->Open();
		file->Load();
		const Common::Byte* data = file->GetData();
		const Common::Size size = file->GetSize();
		END_PROFILE();
		return std::vector<Common::Byte>(data, data + size);
	}



}

