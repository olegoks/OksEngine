#pragma once
#include <Config/auto_OksEngine.Config.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>


namespace OksEngine
{
	void LoadConfigFile::Update(
		ECS2::Entity::Id entity0id, const OksEngine::CommandLineParameters* commandLineParameters0,
		const OksEngine::ConfigFilePath* configFilePath0) {

		ASSERT(Resource::IsResourcePath(configFilePath0->path_));
		std::vector<Common::Byte> resourceData = Resource::GetResourceContent(configFilePath0->path_);

		const ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<Config>(entityId);
		CreateComponent<LuaScript>(entityId,
			std::string{ resourceData.data(), resourceData.size() });


	};

}