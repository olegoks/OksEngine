#include <Debug/ECS/ECSEditor\auto_OksEngine.CreateLink.hpp>

#include <imgui-node-editor/imgui_node_editor.h>

namespace OksEngine {
void CreateLink::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
	const ECSEditorWindow* eCSEditorWindow0,
	const EditorContext* editorContext0) {


	if (ax::NodeEditor::BeginCreate())
	{
		ax::NodeEditor::PinId inputPinId, outputPinId;
		if (ax::NodeEditor::QueryNewLink(&inputPinId, &outputPinId))
		{
			// QueryNewLink returns true if editor want to create new link between pins.
			//
			// Link can be created only for two valid pins, it is up to you to
			// validate if connection make sense. Editor is happy to make any.
			//
			// Link always goes from input to output. User may choose to drag
			// link from output pin or input pin. This determine which pin ids
			// are valid and which are not:
			//   * input valid, output invalid - user started to drag new ling from input pin
			//   * input invalid, output valid - user started to drag new ling from output pin
			//   * input valid, output valid   - user dragged link over other pin, can be validated

			if (inputPinId && outputPinId) // both are valid, let's accept link
			{
				// ed::AcceptNewItem() return true when user release mouse button.
				if (ax::NodeEditor::AcceptNewItem())
				{
					const ECS2::Entity::Id linkEntityId = CreateEntity();
					CreateComponent<Link>(
						linkEntityId,
						static_cast<ECS2::Entity::Id>(inputPinId.Get()),
						static_cast<ECS2::Entity::Id>(outputPinId.Get()));
					CreateComponent<Serializable>(linkEntityId);
				}

				// You may choose to reject connection between these nodes
				// by calling ed::RejectNewItem(). This will allow editor to give
				// visual feedback by changing link thickness and color.
			}
		}
	}
	ax::NodeEditor::EndCreate(); // Wraps up object creation action handling.

};
}