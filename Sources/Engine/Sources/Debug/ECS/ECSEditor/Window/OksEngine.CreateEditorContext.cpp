#include <Debug/ECS/ECSEditor/Window\auto_OksEngine.CreateEditorContext.hpp>

#include <imgui-node-editor/imgui_node_editor.h>

namespace OksEngine {
	void CreateEditorContext::Update(ECS2::Entity::Id entityId,
		const ImGuiState* imGuiState,
		const MainMenuBar* mainMenuBar,
		const ECSMenu* eCSMenu,
		const ECSEditorMenuItem* eCSEditorMenuItem,
		const ECSEditorWindow* eCSEditorWindow) {
		
		ax::NodeEditor::Config config;

		std::shared_ptr<ax::NodeEditor::EditorContext> context{ 
			ax::NodeEditor::CreateEditor(&config),
			[](ax::NodeEditor::EditorContext* contextPtr) {
				
				ax::NodeEditor::DestroyEditor(contextPtr);
			
			} };

		CreateComponent<EditorContext>(entityId, context);


	};
}