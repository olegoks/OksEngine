#include <Debug/ECS/ECSEditor\auto_OksEngine.GetNodePosition.hpp>

#include <imgui.h>
#include <imgui-node-editor/imgui_node_editor.h>


namespace OksEngine {
void GetNodePosition::Update(
    ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
    const EditorContext* editorContext0, ECS2::Entity::Id entity1id,
    const CallGraphNode* callGraphNode1, const Name* name1,
    Position2D* position2D1) {
   const ImVec2 position =  ax::NodeEditor::GetNodePosition((int)entity1id);

   position2D1->x_ = position.x;
   position2D1->y_ = position.y;
};
}