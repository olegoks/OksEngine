#pragma once
#include <Config/auto_OksEngine.Config.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

#include <Common/auto_OksEngine.LuaScript.hpp>

namespace Config
{
	void Load::Update(
		ECS2::Entity::Id entity0id, const CommandLineParameters* commandLineParameters0,
		const ConfigFilePath* configFilePath0) {
		
		ASSERT(Resource::IsResourcePath(configFilePath0->path_));
		std::vector<Common::Byte> resourceData = Resource::LoadFileAndGetContent(configFilePath0->path_);

		const ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<Tag>(entityId);
		CreateComponent<LuaScript>(entityId,
			std::string{ resourceData.data(), resourceData.size() });


	};

}