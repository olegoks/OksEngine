#include <Debug/ECS/ECSEditor/Resources\auto_OksEngine.CreateNodeEntities.hpp>

namespace OksEngine {
void CreateNodeEntities::Update(
    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const EditorContext* editorContext,
    const LoadECSFilesButtonPressed* loadECSFilesButtonPressed,
    ECS2::Entity::Id entity2Id, ResourceSystem* resourceSystem) {

    // auto taskId = resourceSystem->system_->GetAddedResources(Subsystem::Type::Debug, { ".ecs" });
    // auto addedResource = resourceSystem->system_->GetAddedResources(Subsystem::Type::Debug, taskId);

    // std::vector<AsyncResourceSubsystem::Task::Id> getResourceTaskIds;

    // for (auto& ecsFilePath : addedResource) {
    //     getResourceTaskIds.push_back(
    //         resourceSystem->system_->GetResource(
    //             Subsystem::Type::Debug, 
    //             ecsFilePath));
    // }

    // std::vector<Resources::ResourceData> ecsResources;

    // for (auto& taskId : getResourceTaskIds) {
    //     ecsResources.push_back(resourceSystem->system_->GetResource(Subsystem::Type::Debug, taskId));
    // }

	// std::map<std::string, ECS2::Entity::Id> systemNameEntity;
	// //CreateEntities
	// for (Resources::ResourceData& resourceData : ecsResources) {

	// 	::Lua::Script script{ std::string{ resourceData.GetData<Common::Byte>(), resourceData.GetSize()} };

	// 	::Lua::Context context;
	// 	context.LoadScript(script);
	// 	if (!context.GetGlobalAsRef("system").isNil()) {
	// 		luabridge::LuaRef system = context.GetGlobalAsRef("system");
	// 		const std::string name = system["name"].cast<std::string>().value();
	// 		const  ECS2::Entity::Id systemEntityId 
	// 			= CreateEntity<
	// 			CallGraphNode,
	// 			Name,
	// 			ResourcePath,
	// 			FilesystemPath,
	// 			RootNode,
	// 			Position2D,
	// 			RunBefore,
	// 			RunAfter>();
	// 		systemNameEntity[name] = systemEntityId;
	// 	}
	// }

	// for (Common::UInt64 i = 0; i < ecsResources.size(); i++) {
	// 	Resources::ResourceData& resourceData = ecsResources[i];

    //     ::Lua::Script script{ std::string{ resourceData.GetData<Common::Byte>(), resourceData.GetSize()} };

		

    //     ::Lua::Context context;
    //     context.LoadScript(script);
    //     if (!context.GetGlobalAsRef("system").isNil()) {
    //         luabridge::LuaRef system = context.GetGlobalAsRef("system");
    //         const std::string name = system["name"].cast<std::string>().value();

	// 		const ECS2::Entity::Id systemEntityId = systemNameEntity[name];

	// 		auto getOSPathTaskId = resourceSystem->system_->GetFilesystemPath(Subsystem::Type::Debug, addedResource[i]);
	// 		const std::filesystem::path osPath = resourceSystem->system_->GetFilesystemPath(Subsystem::Type::Debug, getOSPathTaskId);

	// 		auto isSettingsFileExistTask = resourceSystem->system_->IsFileExist(Subsystem::Type::Debug, osPath.parent_path() / ("OksEngine." + name + ".dgs")); // draw graph settings
	// 		bool isSettingsFileExist = resourceSystem->system_->IsFileExist(Subsystem::Type::Debug, isSettingsFileExistTask);
	// 		if (!isSettingsFileExist) {
				

	// 			std::string luaSettingsFile =
	// 				"node = {\n"
	// 				"		position2D = { 0.0, 0.0 }\n"
	// 				"}";
	// 			Resources::ResourceData settingsFileData{ luaSettingsFile.c_str(), luaSettingsFile.size() };

	// 			auto createSettingsFileTask = resourceSystem->system_->CreateNewFile(Subsystem::Type::Debug, osPath.parent_path() / ("OksEngine." + name + ".dgs"), std::move(settingsFileData)); // draw graph settings
	// 			resourceSystem->system_->CreateNewFile(Subsystem::Type::Debug, createSettingsFileTask);
	// 		}

	// 		CreateComponent<ResourcePath>(systemEntityId, addedResource[i]);
	// 		CreateComponent<FilesystemPath>(systemEntityId, osPath);
	// 		CreateComponent<CallGraphNode>(systemEntityId);
	// 		CreateComponent<Name>(systemEntityId, name);
	// 		if (!isSettingsFileExist) {
	// 			CreateComponent<Position2D>(systemEntityId, 0, 0);
	// 		} else {
	// 				auto getSettingsResourceTask = resourceSystem->system_->GetResource(
	// 					Subsystem::Type::Debug,
	// 					"Root/OksEngine." + name + ".dgs");

	// 				auto resourceData = resourceSystem->system_->GetResource(Subsystem::Type::Debug, getSettingsResourceTask);
	// 				Lua::Context settingsFileContext;
	// 				settingsFileContext.ExecuteCode(std::string{ resourceData.GetData<Common::Byte>(), resourceData.GetSize() });
	// 				luabridge::LuaRef nodeTable = settingsFileContext.GetGlobalAsRef("node");
	// 				luabridge::LuaRef nodePosition2D = nodeTable["position2D"];
	// 				float x = nodePosition2D[1].cast<float>().value();
	// 				float y = nodePosition2D[2].cast<float>().value();
	// 				CreateComponent<Position2D>(systemEntityId, x, y);

	// 		}
	// 		std::vector<std::string> runAfterSystems;
	// 		std::vector<std::string> runBeforeSystems;
	// 		{
	// 			{
	// 				luabridge::LuaRef runAfterRef = system["runAfter"];
	// 				if (!runAfterRef.isNil()) {
	// 					if (runAfterRef.isTable()) {
	// 						for (luabridge::Iterator it(runAfterRef); !it.isNil(); ++it) {
	// 							luabridge::LuaRef runAfterSystemRef = it.value();
	// 							runAfterSystems.push_back(runAfterSystemRef.cast<std::string>().value());
	// 						}
	// 					}
	// 					else {
	// 						runAfterSystems.push_back(runAfterRef.cast<std::string>().value());
	// 					}
	// 				}
	// 			}
	// 			{
	// 				luabridge::LuaRef runBeforeRef = system["runBefore"];
	// 				if (!runBeforeRef.isNil()) {
	// 					if (runBeforeRef.isTable()) {
	// 						for (luabridge::Iterator it(runBeforeRef); !it.isNil(); ++it) {
	// 							luabridge::LuaRef runAfterSystemRef = it.value();
	// 							runBeforeSystems.push_back(runAfterSystemRef.cast<std::string>().value());
	// 						}
	// 					}
	// 					else {
	// 						runBeforeSystems.push_back(runBeforeRef.cast<std::string>().value());
	// 					}
	// 				}
	// 			}
	// 		}

	// 		std::vector<ECS2::Entity::Id> runAfterEntities;
	// 		for (const std::string& runAfterSystem : runAfterSystems) {
	// 			runAfterEntities.push_back(systemNameEntity[runAfterSystem]);
	// 		}

	// 		CreateComponent<RunAfter>(systemEntityId, runAfterEntities);

	// 		std::vector<ECS2::Entity::Id> runBeforeEntities;
	// 		for (const std::string& runBeforeSystem : runBeforeSystems) {
	// 			runAfterEntities.push_back(systemNameEntity[runBeforeSystem]);
	// 		}

	// 		CreateComponent<RunBefore>(systemEntityId, runAfterEntities);
			
	// 		if (runAfterEntities.empty()) {
	// 			CreateComponent<RootNode>(systemEntityId);
	// 		}


    //     }
    // }

	// RemoveComponent<LoadECSFilesButtonPressed>(entity1Id);
	

};
}