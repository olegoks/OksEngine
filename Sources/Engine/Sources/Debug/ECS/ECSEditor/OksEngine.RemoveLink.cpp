#include <Debug/ECS/ECSEditor\auto_OksEngine.RemoveLink.hpp>

namespace OksEngine {
	void RemoveLink::Update(ECS2::Entity::Id entityId, const ImGUI::State* imGuiState,
		const ECSEditorWindow* eCSEditorWindow,
		const EditorContext* editorContext) {


		// Handle deletion action
		if (ax::NodeEditor::BeginDelete())
		{
			// There may be many links marked for deletion, let's loop over them.
			ax::NodeEditor::LinkId deletedLinkId;
			while (ax::NodeEditor::QueryDeletedLink(&deletedLinkId))
			{
				// If you agree that link can be deleted, accept deletion.
				if (ax::NodeEditor::AcceptDeletedItem())
				{
					DestroyEntity(deletedLinkId.Get());
					break;
				}

				// You may reject link deletion by calling:
				// ed::RejectDeletedItem();
			}
		}
		ax::NodeEditor::EndDelete(); // Wrap up deletion action

	};
}