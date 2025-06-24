#include <Debug/ECS/ECSEditor\auto_OksEngine.SetNodePosition.hpp>

#include <imgui-node-editor/imgui_node_editor.h>

namespace OksEngine {
void SetNodePosition::Update(
    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const ECSEditorWindow* eCSEditorWindow, const EditorContext* editorContext,
    const SceneLoaded* sceneLoaded, ECS2::Entity::Id entity2Id,
    const CallGraphNode* callGraphNode, const Position2D* position2D) {

    ax::NodeEditor::SetNodePosition(static_cast<int>(entity2Id), { position2D->x_, position2D->y_ });

};
}