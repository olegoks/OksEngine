#include <Debug/ECS/ECSEditor\auto_OksEngine.ProcessLinksDeleting.hpp>

namespace OksEngine {
void ProcessLinksDeleting::Update(
    ECS2::Entity::Id entityId, const ImGuiState* imGuiState,
    const EditorContext* editorContext) {



    //// Handle deletion action
    //if (ed::BeginDelete())
    //{
    //    // There may be many links marked for deletion, let's loop over them.
    //    ed::LinkId deletedLinkId;
    //    while (ed::QueryDeletedLink(&deletedLinkId))
    //    {
    //        // If you agree that link can be deleted, accept deletion.
    //        if (ed::AcceptDeletedItem())
    //        {
    //            // Then remove link from your data.
    //            for (auto& link : m_Links)
    //            {
    //                if (link.Id == deletedLinkId)
    //                {
    //                    m_Links.erase(&link);
    //                    break;
    //                }
    //            }
    //        }

    //        // You may reject link deletion by calling:
    //        // ed::RejectDeletedItem();
    //    }
    //}
    //ed::EndDelete(); // Wrap up deletion action

                                  };
}