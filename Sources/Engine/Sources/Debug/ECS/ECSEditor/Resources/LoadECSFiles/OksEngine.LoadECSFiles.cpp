#include <Debug/ECS/ECSEditor/Resources/LoadECSFiles\auto_OksEngine.LoadECSFiles.hpp>

namespace OksEngine {
void LoadECSFiles::Update(

    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const ECSEditorWindow* eCSEditorWindow, const EditorContext* editorContext,
    const LoadECSFilesButtonPressed* loadECSFilesButtonPressed,

    ECS2::Entity::Id entity2Id, const ResourceSystem* resourceSystem) {

        const auto addedResources = resourceSystem->system_->GetAddedResourcesSynch(Subsystem::Type::Debug, { ".ecs" });

        for (auto& ecsFilePath : addedResources) {
            
            auto ecsFileData = resourceSystem->system_->GetResourceSynch(
                    Subsystem::Type::Debug,
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

            //Create node entity.
            const  ECS2::Entity::Id systemEntityId
                = CreateEntity<
                CallGraphNode,
                Name,
                RootNode,
                Position2D,
                Serializable,
                BeforePin,
                AfterPin>();

            CreateComponent<CallGraphNode>(systemEntityId);

            const std::string systemName = system["name"].cast<std::string>().value();
            CreateComponent<Name>(systemEntityId, 
                systemName
                //ecsFilePath.filename().stem().string().substr(std::size("OksEngine.")) // Get system name.
                );
            CreateComponent<Position2D>(systemEntityId, 0.f, 0.f);
            CreateComponent<Serializable>(systemEntityId);

        }



    };
}