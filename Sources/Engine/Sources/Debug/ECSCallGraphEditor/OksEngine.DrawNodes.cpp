#include <Debug/ECSCallGraphEditor\auto_OksEngine.DrawNodes.hpp>

#include <imgui.h>
#include <Debug/Graph/imgui_node_editor.hpp>

namespace OksEngine {
void DrawNodes::Update(
    ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
    const EditorContext* editorContext) {


    //int uniqueId = 1;

    ////
    //// 1) Commit known data to editor
    ////

    //// Submit Node A
    //ed::NodeId nodeA_Id = uniqueId++;
    //ed::PinId  nodeA_InputPinId = uniqueId++;
    //ed::PinId  nodeA_OutputPinId = uniqueId++;

    //if (m_FirstFrame)
    //    ed::SetNodePosition(nodeA_Id, ImVec2(10, 10));
    //ed::BeginNode(nodeA_Id);
    //ImGui::Text("Node A");
    //ed::BeginPin(nodeA_InputPinId, ed::PinKind::Input);
    //ImGui::Text("-> In");
    //ed::EndPin();
    //ImGui::SameLine();
    //ed::BeginPin(nodeA_OutputPinId, ed::PinKind::Output);
    //ImGui::Text("Out ->");
    //ed::EndPin();
    //ed::EndNode();

    //// Submit Node B
    //ed::NodeId nodeB_Id = uniqueId++;
    //ed::PinId  nodeB_InputPinId1 = uniqueId++;
    //ed::PinId  nodeB_InputPinId2 = uniqueId++;
    //ed::PinId  nodeB_OutputPinId = uniqueId++;

    //

    //if (m_FirstFrame)
    //    ed::SetNodePosition(nodeB_Id, ImVec2(210, 60));

    //ed::BeginNode(nodeB_Id);
    //ImGui::Text("Node B");
    //ImGuiEx_BeginColumn();
    //ed::BeginPin(nodeB_InputPinId1, ed::PinKind::Input);
    //ImGui::Text("-> In1");
    //ed::EndPin();
    //ed::BeginPin(nodeB_InputPinId2, ed::PinKind::Input);
    //ImGui::Text("-> In2");
    //ed::EndPin();
    //ImGuiEx_NextColumn();
    //ed::BeginPin(nodeB_OutputPinId, ed::PinKind::Output);
    //ImGui::Text("Out ->");
    //ed::EndPin();
    //ImGuiEx_EndColumn();
    //ed::EndNode();


                       };
}