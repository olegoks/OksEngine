#include <Debug/ECS/ECSEditor\auto_OksEngine.DrawNodes.hpp>

#include <imgui.h>
#include <imgui-node-editor/imgui_node_editor.h>

#include <Debug/ECS/ECSEditor/auto_OksEngine.AfterPin.hpp>
#include <Debug/ECS/ECSEditor/auto_OksEngine.BeforePin.hpp>
#include <Debug/ECS/ECSEditor/auto_OksEngine.Link.hpp>

namespace OksEngine {
	void DrawNodes::Update(
		ECS2::Entity::Id entity0id, const CallGraphNode* calClGraphNode0,
		const Name* name0, const Position2D* position2D0,
		ECS2::Entity::Id entity1id, const ImGuiState* imGuiState1,
		const ECSEditorWindow* eCSEditorWindow1) {


		//int uniqueId = 1;


		//ed::PinId  nodeA_InputPinId = uniqueId++;
		//ed::PinId  nodeA_OutputPinId = uniqueId++;

		//const float columnOffset = 100;
		//const float rowOffset = 100;
		//Common::UInt64 columnIndex = 0;



		//auto drawNode = [&](auto&& self, ECS2::Entity::Id nodeEntityId, Common::UInt64 rowIndex)->void{

		//    const RunBefore* nodeRunBefore = GetComponent<RunBefore>(nodeEntityId);

		//    ax::NodeEditor::SetNodePosition((int)entity1Id, ImVec2(columnIndex * columnOffset, ));
		//    ax::NodeEditor::BeginNode((int)entity1Id);
		//    ImGui::Text(name->value_.c_str());
		//    ax::NodeEditor::EndNode();

		//    ++columnIndex;
		//    for (Common::UInt64 i = 0; i < nodeRunBefore->entities_.size(); i++) {
		//        const ECS2::Entity::Id nodeEntityId = nodeRunBefore->entities_[i];
		//        self(self, nodeEntityId, );
		//    }
		//    };

		//drawNode(drawNode, entity1Id);
		static bool firstFrame = true;

		ax::NodeEditor::GetNodePosition((int)entity0id);

		ImGui::PushID(static_cast<int>(entity0id));
		ax::NodeEditor::BeginNode((int)entity0id);

		ImGui::Text(name0->value_.c_str());

#pragma region Assert
		OS::AssertMessage(entity0id < Common::Limits<int>::Max(), "");
#pragma endregion

		const int intEntityId = static_cast<int>(entity0id);
		ax::NodeEditor::NodeId nodeId = intEntityId;
		ImGui::BeginGroup();
		if (IsComponentExist<BeforePin>(entity0id)) {
			const ECS2::Entity::Id beforePinEntityId = GetComponent<BeforePin>(entity0id)->id_;
			ax::NodeEditor::BeginPin(static_cast<int>(beforePinEntityId), ax::NodeEditor::PinKind::Input);
			ImGui::Text("Before");
			ax::NodeEditor::EndPin();
		}
		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();
		if (IsComponentExist<AfterPin>(entity0id)) {
			const ECS2::Entity::Id afterPinEntityId = GetComponent<AfterPin>(entity0id)->id_;
			ax::NodeEditor::BeginPin(static_cast<int>(afterPinEntityId), ax::NodeEditor::PinKind::Input);
			ImGui::Text("After");
			ax::NodeEditor::EndPin();
		}
		ImGui::EndGroup();
		ax::NodeEditor::EndNode();
		ImGui::PopID();


		if (firstFrame) {
			firstFrame = false;
		}

	};
}