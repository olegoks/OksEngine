#include <Debug/ECSCallGraphEditor/Resources\auto_OksEngine.CreateNodeSettingsFile.hpp>

namespace OksEngine {
void CreateNodeSettingsFile::Update(
    ECS2::Entity::Id entity1Id, const CallGraphNode* callGraphNode,
    const Name* name, const RootNode* rootNode, const Position2D* position2D,
    const RunBefore* runBefore, const RunAfter* runAfter,
    ECS2::Entity::Id entity2Id, const ImGuiState* imGuiState,
    const EditorContext* editorContext) {};
}