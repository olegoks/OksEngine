#pragma once
#include <ECS\Project\auto_OksEngine.ECS.Module.hpp>
#include <ECS\File\auto_OksEngine.ECS.File.Archetype.hpp>

#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

namespace OksEngine::ECS::Module
{
	void CreateModules::Update(
		ECS2::Entity::Id entity0id,
		const OksEngine::ECS::Project::Tag* eCS__Project__Tag0,
		const OksEngine::ECS::Project::Path* eCS__Project__Path0,
		const OksEngine::LuaScript* luaScript0) {


		::Lua::Context context;
		::Lua::Script script{ luaScript0->text_ };
		context.LoadScript(script);

		luabridge::LuaRef ecsTable = luabridge::getGlobal(context.state_, "ECS");

		ASSERT_FMSG(ecsTable.isTable() || ecsTable["Modules"].isTable(), "");

		luabridge::LuaRef modulesTable = ecsTable["Modules"];

		std::vector<std::filesystem::path> modulePaths;

		std::filesystem::path projectFilePath = eCS__Project__Path0->path_;

		int len = modulesTable.length();
		for (int i = 1; i <= len; ++i) {
			luabridge::LuaRef desc = modulesTable[i];
			if (desc.isTable()) {
				const std::string includePath = desc["IncludePath"].cast<std::string>().value();

				luabridge::LuaRef pathsRef = desc["Paths"];
				Common::Size pathsNumber = pathsRef.length();
				for (Common::Index j = 1; j <= pathsNumber; ++j) {
					const std::string path = pathsRef[j].cast<std::string>().value();
					std::filesystem::path modulePath = (projectFilePath.parent_path() / path).lexically_normal();
					for (const auto& entry : std::filesystem::recursive_directory_iterator(modulePath.parent_path())) {
						if (std::filesystem::is_regular_file(entry)) {
							if (entry.path().extension().string() == ".ecs_module") {

								std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(entry.path().string());

								const ECS2::Entity::Id fileEntityId = CreateEntity();
								CreateComponent<ECS::Module::Tag>(fileEntityId);
								CreateComponent<ECS::Module::Path>(fileEntityId, entry.path().string());
								CreateComponent<ECS::Project::EntityId>(fileEntityId, entity0id);
								CreateComponent<LuaScript>(fileEntityId, std::string{ data.data(), data.size() });
							}
						}
					}


				}

				
			}
		}


	};

	void CreateFileEntities::Update(
		ECS2::Entity::Id entity0id, 
		const OksEngine::ECS::Module::Tag* eCS__Module__Tag0,
		const OksEngine::ECS::Module::Path* eCS__Module__Path0,
		const OksEngine::LuaScript* luaScript0) {

		::Lua::Context context;
		::Lua::Script script{ luaScript0->text_ };
		context.LoadScript(script);

		luabridge::LuaRef ecsTable = luabridge::getGlobal(context.state_, "ECS");

		luabridge::LuaRef modulesFiles = ecsTable["Files"];

		std::filesystem::path modulePath = eCS__Module__Path0->path_;
		std::vector<std::filesystem::path> moduleFilePaths;
		int len = modulesFiles.length();
		for (int i = 1; i <= len; ++i) {
			if (modulesFiles[i].isString()) {
				std::filesystem::path moduleFilePath = modulePath.parent_path() / modulesFiles[i].cast<std::string>().value();
				moduleFilePaths.push_back(moduleFilePath.lexically_normal());
			}
		}

		for (auto filePath : moduleFilePaths) {

			std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(filePath.string());

			const ECS2::Entity::Id fileEntityId = CreateEntity<ECS__FILE__FILE>();
			CreateComponent<ECS::File::Tag>(fileEntityId);
			CreateComponent<ECS::File::Path>(fileEntityId, filePath.string());
			CreateComponent<ECS::Module::EntityId>(fileEntityId, entity0id);
			CreateComponent<LuaScript>(fileEntityId, std::string{ data.data(), data.size() });
		}
	}

} // namespace OksEngine