#include <Debug/ECS/ECSEditor/Window\auto_OksEngine.BeginECSEditorWindow.hpp>

namespace OksEngine {
	void BeginECSEditorWindow::Update(
		ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSEditorMenuItem* eCSEditorMenuItem0,
		ECSEditorWindow* eCSEditorWindow0,
		const EditorContext* editorContext0) {

		float menuBarHeight = ImGui::GetFrameHeight();

		ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));

		ImVec2 displaySize = ImGui::GetIO().DisplaySize;
		ImGui::SetNextWindowSize(ImVec2(displaySize.x, displaySize.y - menuBarHeight));

		bool isOpen = true;
		ImGui::Begin("ECS Editor",
			&isOpen,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus);

		if (!isOpen) {
			RemoveComponent<ECSEditorWindow>(entity0id);
			return;
		}

		ax::NodeEditor::SetCurrentEditor(editorContext0->context_.get());

	};
}