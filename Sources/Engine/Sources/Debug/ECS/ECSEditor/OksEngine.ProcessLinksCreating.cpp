#include <Debug/ECS/ECSEditor\auto_OksEngine.ProcessLinksCreating.hpp>

namespace OksEngine {
void ProcessLinksCreating::Update(
    ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
    const EditorContext* editorContext) {


    ////
    //// 2) Handle interactions
    ////

    //// Handle creation action, returns true if editor want to create new object (node or link)
    //if (ed::BeginCreate())
    //{
    //    ed::PinId inputPinId, outputPinId;
    //    if (ed::QueryNewLink(&inputPinId, &outputPinId))
    //    {
    //        // QueryNewLink returns true if editor want to create new link between pins.
    //        //
    //        // Link can be created only for two valid pins, it is up to you to
    //        // validate if connection make sense. Editor is happy to make any.
    //        //
    //        // Link always goes from input to output. User may choose to drag
    //        // link from output pin or input pin. This determine which pin ids
    //        // are valid and which are not:
    //        //   * input valid, output invalid - user started to drag new ling from input pin
    //        //   * input invalid, output valid - user started to drag new ling from output pin
    //        //   * input valid, output valid   - user dragged link over other pin, can be validated

    //        if (inputPinId && outputPinId) // both are valid, let's accept link
    //        {
    //            // ed::AcceptNewItem() return true when user release mouse button.
    //            if (ed::AcceptNewItem())
    //            {
    //                // Since we accepted new link, lets add one to our list of links.
    //                m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });

    //                // Draw new link.
    //                ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
    //            }

    //            // You may choose to reject connection between these nodes
    //            // by calling ed::RejectNewItem(). This will allow editor to give
    //            // visual feedback by changing link thickness and color.
    //        }
    //    }
    //}
    //ed::EndCreate(); // Wraps up object creation action handling.


                                  };
}