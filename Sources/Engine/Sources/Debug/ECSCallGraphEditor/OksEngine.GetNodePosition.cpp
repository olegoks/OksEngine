#include <Debug/ECSCallGraphEditor\auto_OksEngine.GetNodePosition.hpp>

#include <imgui.h>
#include <Debug/Graph/imgui_node_editor.hpp>


namespace OksEngine {
void GetNodePosition::Update(
    ECS2::Entity::Id entity1Id, const CallGraphNode* callGraphNode,
    const Name* name, Position2D* position2D,
    ECS2::Entity::Id entity2Id, const ImGuiState* imGuiState,
    const EditorContext* editorContext) {
   const ImVec2 position =  ax::NodeEditor::GetNodePosition((int)entity1Id);

   position2D->x_ = position.x;
   position2D->y_ = position.y;
};
}