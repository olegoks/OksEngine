#include <Debug/ECS/ECSEditor\auto_OksEngine.GraphEditorEnd.hpp>

namespace OksEngine {
void GraphEditorEnd::Update(
    ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
    const EditorContext* editorContext) {


    // End of interaction with editor.
    ax::NodeEditor::End();

    //if (m_FirstFrame)
    //    ed::NavigateToContent(0.0f);

    //m_FirstFrame = false;

    ax::NodeEditor::SetCurrentEditor(nullptr);
   // ImGui::ShowMetricsWindow();
    ImGui::PopStyleVar(2);
    ImGui::End();
    ImGui::PopStyleVar(2);


   };
}