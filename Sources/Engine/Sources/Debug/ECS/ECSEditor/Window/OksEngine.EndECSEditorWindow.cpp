#include <Debug/ECS/ECSEditor/Window\auto_OksEngine.EndECSEditorWindow.hpp>

namespace OksEngine {
	void EndECSEditorWindow::Update(
		ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSEditorMenuItem* eCSEditorMenuItem0,
		const ECSEditorWindow* eCSEditorWindow0,
		const EditorContext* editorContext0) {

		ImGui::End();

		ax::NodeEditor::SetCurrentEditor(nullptr);

	};
}