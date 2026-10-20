#pragma once
#include <ECS\Project\auto_OksEngine.ECS.Module.hpp>
#include <ECS\File\auto_OksEngine.ECS.File.Archetype.hpp>

#include <Resources/OksEngine.ResourceSystem.Utils.hpp>

#include <ECS/File/OksEngine.ECS.File.Utils.hpp>

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

		if (!ecsTable.isNil()) {


			ASSERT_FMSG(ecsTable.isTable() || ecsTable["Packages"].isTable(), "");

			luabridge::LuaRef modulesTable = ecsTable["Packages"];

			std::vector<std::filesystem::path> modulePaths;

			std::filesystem::path projectFilePath = eCS__Project__Path0->path_;

			auto processModulePath = [&](std::filesystem::path modulePath, const std::string includePath, const std::vector<std::string>& disableTags) {
				for (const auto& entry : std::filesystem::recursive_directory_iterator(modulePath.parent_path())) {
					if (std::filesystem::is_regular_file(entry)) {
						if (entry.path().extension().string() == ".ecs_module") {

							std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(entry.path().string());

							const ECS2::Entity::Id moduleEntityId = CreateEntity<ECS__MODULE__MODULE>();
							CreateComponent<ECS::Module::Tag>(moduleEntityId);
							CreateComponent<ECS::Module::Path>(moduleEntityId, entry.path().string());
							CreateComponent<ECS::Module::IncludePath>(moduleEntityId, includePath);
							CreateComponent<ECS::Project::EntityId>(moduleEntityId, entity0id);
							CreateComponent<LuaScript>(moduleEntityId, std::string{ data.data(), data.size() });
							CreateComponent<ECS::Module::DisableTags>(moduleEntityId, disableTags);
						}
					}
				}
				};

			int len = modulesTable.length();
			for (int i = 1; i <= len; ++i) {
				luabridge::LuaRef desc = modulesTable[i];
				if (desc.isTable()) {

					const std::vector<std::string> disableTags = [&]() {
						luabridge::LuaRef disableTagsRef = desc["DisableTags"];
						if (disableTagsRef.isNil()) {
							return std::vector<std::string>{};
						}
						Common::Size disableTagsNumber = disableTagsRef.length();
						std::vector<std::string> disableTags;
						for (Common::Index j = 1; j <= disableTagsNumber; ++j) {
							const std::string disableTag = disableTagsRef[j].cast<std::string>().value();
							ASSERT_FMSG(ECS::File::Table::Tags::IsValid(disableTag), "Invalid tag syntax in project file {} : {}", projectFilePath.string(), disableTag);
							disableTags.push_back(disableTag);
						}
						
						return disableTags;
						}();

					ASSERT(desc["IncludePath"].isString());

					const std::string includePath = desc["IncludePath"].cast<std::string>().value();

					luabridge::LuaRef pathsRef = desc["ModulePaths"];
					Common::Size pathsNumber = pathsRef.length();
					for (Common::Index j = 1/*In lua arrays from 1*/; j <= pathsNumber; ++j) {
						const std::string path = pathsRef[j].cast<std::string>().value();
						std::filesystem::path modulePath = (projectFilePath.parent_path() / path).lexically_normal();
						processModulePath(modulePath, includePath, disableTags);


					}

				}
			}


			//
			/*if (std::filesystem::exists(projectFilePath.parent_path() / "Packages")) {
				std::vector<std::filesystem::path> result;
				for (const auto& entry : std::filesystem::recursive_directory_iterator(projectFilePath.parent_path() / "Packages")) {
					if (entry.is_regular_file() && entry.path().extension() == ".ecs_module") {
						result.push_back(entry.path());
					}
				}

				for (const auto modulePath : result) {
					processModulePath(modulePath, "./Sources");
				}
			}*/
		}
		else {
			// .ecs_module as ecs scene


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
		std::unordered_set<std::filesystem::path> moduleFilePaths;
		int len = modulesFiles.length();
		for (int i = 1; i <= len; ++i) {
			if (modulesFiles[i].isString()) {
				std::filesystem::path moduleFilePath = modulePath.parent_path() / modulesFiles[i].cast<std::string>().value();
				moduleFilePaths.insert(moduleFilePath.lexically_normal());
			}
		}

		ASSERT_FMSG(moduleFilePaths.size() == len, "Duplicates in .ecs paths.");

		LOG_INFO("", "Loaded .ecs files:");
		for (auto filePath : moduleFilePaths) {

			std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(filePath.string());

			const ECS2::Entity::Id fileEntityId = CreateEntity<ECS__FILE__FILE>();
			CreateComponent<ECS::File::Tag>(fileEntityId);
			CreateComponent<ECS::File::Path>(fileEntityId, filePath.string());
			LOG_INFO("", "{}", filePath.string());
			CreateComponent<ECS::Module::EntityId>(fileEntityId, entity0id);
			if (IsComponentExist<ECS::Module::DisableTags>(entity0id)) {
				CreateComponent<ECS::File::DisableTags>(
					fileEntityId,
					GetComponent<ECS::Module::DisableTags>(entity0id)->disableTags_
				);
			}
			CreateComponent<LuaScript>(fileEntityId, std::string{ data.data(), data.size() });
		}
	}

} // namespace OksEngine