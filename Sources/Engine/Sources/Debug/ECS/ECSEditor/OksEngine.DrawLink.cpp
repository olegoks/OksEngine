#include <Debug/ECS/ECSEditor\auto_OksEngine.DrawLink.hpp>

#include <imgui-node-editor/imgui_node_editor.h>

namespace OksEngine {
void DrawLink::Update(ECS2::Entity::Id entityId, const Link* link) {

#pragma region Assert
	OS::AssertMessage(entityId < Common::Limits<int>::Max(), "");
#pragma endregion

	// Draw new link.
	ax::NodeEditor::Link(
		static_cast<int>(entityId),
		static_cast<int>(link->nodeFrom_),
		static_cast<int>(link->nodeTo_));

};
}