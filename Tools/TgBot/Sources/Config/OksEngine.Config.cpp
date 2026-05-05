#pragma once

#include <Config/auto_OksEngine.Config.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

namespace OksEngine
{
	namespace Config {
		void Load::Update(
			ECS2::Entity::Id entity0id, const OksEngine::CommandLineParameters* commandLineParameters0,
			const OksEngine::ConfigFilePath* configFilePath0) {

			std::vector<Common::Byte> resourceData = Resource::LoadFileAndGetContent(configFilePath0->path_);

			const ECS2::Entity::Id entityId = CreateEntity();

			CreateComponent<Config::Tag>(entityId);
			CreateComponent<LuaScript>(entityId,
				std::string{ resourceData.data(), resourceData.size() });


		};
	}
}