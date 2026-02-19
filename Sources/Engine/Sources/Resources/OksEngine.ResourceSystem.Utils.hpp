#pragma once 

#define RESOURCE__MANAGER__CREATE_LOAD_RESOURCE_REQUEST(resourcePath)							\
	[this](const std::filesystem::path& path){													\
		const ECS2::Entity::Id requestEntityId = CreateEntity();								\
		CreateComponent<Resource::Manager::Request::Tag>(requestEntityId);						\
		CreateComponent<Resource::Manager::Request::LoadResource>(requestEntityId, path);		\
		CreateComponent<Resource::Manager::Request::State::Pending>(requestEntityId);			\
		return 	requestEntityId;																\
	}(resourcePath);


#define RESOURCE__CREATE_RESOURCE(resourcePath, resourceData)					\
	[this](const std::filesystem::path& path, std::vector<Common::Byte> data){	\
		const ECS2::Entity::Id resourceEntityId = CreateEntity();				\
		CreateComponent<Resource::Tag>(resourceEntityId);						\
		CreateComponent<Resource::Path>(resourceEntityId, path);				\
		CreateComponent<Resource::Data>(resourceEntityId, data);				\
		return 	resourceEntityId;												\
	}(resourcePath, resourceData);