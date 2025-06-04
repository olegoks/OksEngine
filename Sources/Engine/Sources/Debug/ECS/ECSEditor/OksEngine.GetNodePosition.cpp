#include <Debug/ECS/ECSEditor\auto_OksEngine.GetNodePosition.hpp>

#include <imgui.h>
#include <imgui-node-editor/imgui_node_editor.h>


namespace OksEngine {
void GetNodePosition::Update(
    ECS2::Entity::Id entity1Id, const ImGuiState* imGuiState,
    const EditorContext* editorContext,
    const LoadECSFilesButtonPressed* loadECSFilesButtonPressed,
    ECS2::Entity::Id entity2Id, const CallGraphNode* callGraphNode,
    const Name* name, Position2D* position2D) {
   const ImVec2 position =  ax::NodeEditor::GetNodePosition((int)entity1Id);

   position2D->x_ = position.x;
   position2D->y_ = position.y;
};
}