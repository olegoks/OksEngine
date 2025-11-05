#include <Debug/ECS/ECSEditor\auto_OksEngine.GraphEditorEnd.hpp>

#include <imgui-node-editor/imgui_node_editor.h>

namespace OksEngine {
void GraphEditorEnd::Update(
    ECS2::Entity::Id entityId, const ImGUI::State* imGuiState,
    const EditorContext* editorContext) {


   // // End of interaction with editor.
   ax::NodeEditor::End();

   static bool firstFrame = true;

    if (firstFrame)
        ax::NodeEditor::NavigateToContent(0.0f);

    firstFrame = false;

    ax::NodeEditor::SetCurrentEditor(nullptr);
   //// ImGui::ShowMetricsWindow();
   // ImGui::PopStyleVar(2);
   // ImGui::End();
   // ImGui::PopStyleVar(2);


   };
}