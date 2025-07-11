#include <Debug/ECS/ECSEditor/Resources/LoadECSFiles\auto_OksEngine.LoadECSFiles.hpp>

namespace OksEngine {
void LoadECSFiles::Update(
    ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
    const ECSEditorWindow* eCSEditorWindow0,
    const EditorContext* editorContext0,
    const LoadingECSFiles* loadingECSFiles0,

    ECS2::Entity::Id entity1id,
    const ResourceSystem* resourceSystem1) {

        const auto addedResources = resourceSystem1->system_->GetAddedResourcesSynch(Subsystem::Type::ChildThread, { ".ecs" });

        for (auto& ecsFilePath : addedResources) {

            //if (ecsFilePath.) {

            //}
            
            auto ecsFileData = resourceSystem1->system_->GetResourceSynch(
                    Subsystem::Type::ChildThread,
                    ecsFilePath);
            ::Lua::Context luaContext;
            luaContext.LoadScript(
                std::string{ 
                    ecsFileData.GetData<Common::Byte>(),
                    ecsFileData.GetSize() }
            );

            luabridge::LuaRef system = luaContext.GetGlobalAsRef("system");

            if (system.isNil()) {
                continue;
            }
            if (system["runAfter"].isNil() && system["runBefore"].isNil()) {
                continue;
            }

            //Create node entity.
            const  ECS2::Entity::Id systemEntityId
                = CreateEntity<
                CallGraphNode,
                Name,
                RootNode,
                Position2D,
                Serializable,
                RunAfter,
                RunBefore,
                BeforePin,
                AfterPin>();

            CreateComponent<CallGraphNode>(systemEntityId);

            const std::string systemName = system["name"].cast<std::string>().value();
            CreateComponent<Name>(systemEntityId, 
                systemName
                //ecsFilePath.filename().stem().string().substr(std::size("OksEngine.")) // Get system name.
                );

            {
                std::vector<std::string> runAfterSystems;
                luabridge::LuaRef runAfterRef = system["runAfter"];
                if (!runAfterRef.isNil()) {
                    if (runAfterRef.isTable()) {
                        for (luabridge::Iterator it(runAfterRef); !it.isNil(); ++it) {
                            luabridge::LuaRef runAfterSystemRef = it.value();
                            runAfterSystems.push_back(runAfterSystemRef.cast<std::string>().value());
                        }
                    }
                    else {
                        runAfterSystems.push_back(runAfterRef.cast<std::string>().value());
                    }
                }
                CreateComponent<RunAfter>(systemEntityId, runAfterSystems);
            }
            {
                std::vector<std::string> runBeforeSystems;
                luabridge::LuaRef runBeforeRef = system["runBefore"];
                if (!runBeforeRef.isNil()) {
                    if (runBeforeRef.isTable()) {
                        for (luabridge::Iterator it(runBeforeRef); !it.isNil(); ++it) {
                            luabridge::LuaRef runAfterSystemRef = it.value();
                            runBeforeSystems.push_back(runAfterSystemRef.cast<std::string>().value());
                        }
                    }
                    else {
                        runBeforeSystems.push_back(runBeforeRef.cast<std::string>().value());
                    }
                }
                CreateComponent<RunBefore>(systemEntityId, runBeforeSystems);
            }
            CreateComponent<Position2D>(systemEntityId, 0.f, 0.f);
            CreateComponent<Serializable>(systemEntityId);

            //Create pins
            const  ECS2::Entity::Id beforePinEntityId
                = CreateEntity<
                Name,
                Pin,
                Serializable>();

            CreateComponent<Name>(beforePinEntityId, "Before");
            CreateComponent<Pin>(beforePinEntityId);
            CreateComponent<Serializable>(beforePinEntityId);

            const  ECS2::Entity::Id afterPinEntityId
                = CreateEntity<
                Name,
                Pin,
                Serializable>();

            CreateComponent<Name>(afterPinEntityId, "After");
            CreateComponent<Pin>(afterPinEntityId);
            CreateComponent<Serializable>(afterPinEntityId);
            //

            CreateComponent<BeforePin>(systemEntityId, beforePinEntityId);
            CreateComponent<AfterPin>(systemEntityId, afterPinEntityId);

        }

        RemoveComponent<LoadingECSFiles>(entity0id);
        CreateComponent<CreateLinksRequest>(entity0id);
    };
}