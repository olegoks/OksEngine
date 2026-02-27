#pragma once

#define SCENE__MANAGER__CREATE_LOAD_SCENE_REQUEST(resourcePath)									\
	[this](const std::string& path){													\
		const ECS2::Entity::Id requestEntityId = CreateEntity();								\
		CreateComponent<Scene::Manager::Request::Tag>(requestEntityId);							\
		CreateComponent<Scene::Manager::Request::LoadScene>(requestEntityId, path);				\
		CreateComponent<Scene::Manager::Request::State::Pending>(requestEntityId);				\
		return 	requestEntityId;																\
	}(resourcePath);