#include <Debug/ECS/ECSEditor\auto_OksEngine.GraphEditorBegin.hpp>

namespace OksEngine {
	void GraphEditorBegin::Update(
		ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
		const EditorContext* editorContext) {

		ax::NodeEditor::Begin("");

		//auto& io = ImGui::GetIO();

		//ImGui::SetNextWindowPos(ImVec2(0, 0));
		//ImGui::SetNextWindowSize(io.DisplaySize);
		//const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize;
		//const auto windowRounding = ImGui::GetStyle().WindowRounding;
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//ImGui::Begin("Content", nullptr, ImGuiWindowFlags_NoTitleBar |
		//	ImGuiWindowFlags_NoResize |
		//	ImGuiWindowFlags_NoMove |
		//	ImGuiWindowFlags_NoScrollbar |
		//	ImGuiWindowFlags_NoScrollWithMouse |
		//	ImGuiWindowFlags_NoSavedSettings |
		//	ImGuiWindowFlags_NoBringToFrontOnFocus);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);

		//ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

		//ImGui::Separator();

		//ax::NodeEditor::SetCurrentEditor(editorContext->context_);

		//ax::NodeEditor::Begin("My Editor", ImVec2(0.0, 0.0f));

	};
}