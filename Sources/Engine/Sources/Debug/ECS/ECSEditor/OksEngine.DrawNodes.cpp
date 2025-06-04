#include <Debug/ECS/ECSEditor\auto_OksEngine.DrawNodes.hpp>

#include <imgui.h>
#include <imgui-node-editor/imgui_node_editor.h>

namespace OksEngine {
	void DrawNodes::Update(
		ECS2::Entity::Id entity1Id, const CallGraphNode* callGraphNode,
		const Name* name, const RootNode* rootNode,
		const Position2D* position2D, const RunBefore* runBefore,
		const RunAfter* runAfter, ECS2::Entity::Id entity2Id,
		const ImGuiState* imGuiState, const EditorContext* editorContext) {


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

		//ax::NodeEditor::SetNodePosition((int)entity1Id, ImVec2(position2D->x_, position2D->y_));

		ax::NodeEditor::BeginNode((int)entity1Id);

		ImGui::Text(name->value_.c_str());
		//ax::NodeEditor::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
		//ImGui::Text("-> In");
		//ax::NodeEditor::EndPin();
		//ImGui::SameLine();
		//ax::NodeEditor::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
		//ImGui::Text("Out ->");
		//ax::NodeEditor::EndPin();
		ax::NodeEditor::EndNode();

		//// Submit Node B
		//ax::NodeEditor::NodeId nodeB_Id = uniqueId++;
		//ax::NodeEditor::PinId  nodeB_InputPinId1 = uniqueId++;
		//ax::NodeEditor::PinId  nodeB_InputPinId2 = uniqueId++;
		//ax::NodeEditor::PinId  nodeB_OutputPinId = uniqueId++;
		if (firstFrame) {
			firstFrame = false;
		}

	};
}