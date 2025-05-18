#include <Debug\auto_OksEngine.CreateECSInspector.hpp>


#include <imgui.h>
#include <Debug/Graph/imgui_node_editor.hpp>

namespace OksEngine {

	void CreateECSInspector::Update() {

		ECS2::Entity::Id entityId = CreateEntity();

		CreateComponent<ImGuiState>(entityId);
		CreateComponent<HandleKeyboardInput>(entityId);

		ax::NodeEditor::EditorContext* m_Context = ax::NodeEditor::CreateEditor();;

		CreateComponent<EditorContext>(entityId, m_Context);
		//CreateComponent<MainMenuBar>(entityId);
		//CreateComponent<EnginePerformance>(entityId);
		//CreateComponent<ECSInspector>(entityId);
		//CreateComponent<FramesCounter>(entityId);

	};
}