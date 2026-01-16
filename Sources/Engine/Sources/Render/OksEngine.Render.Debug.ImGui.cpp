#pragma once
#include <Render\auto_OksEngine.Render.Debug.ImGui.hpp>

namespace OksEngine
{


namespace Render
{
namespace Debug
{
namespace ImGUI
{
void CreateRenderMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State *imGUI__State0,
                              const OksEngine::Input::HandleKeyboardEvents *input__HandleKeyboardEvents0,
                              const OksEngine::Input::KeyboardEvents *input__KeyboardEvents0) {
    CreateComponent<RenderMenu>(entity0id, false);

};

}

} // namespace Debug

} // namespace Render

namespace Render
{
namespace Debug
{
namespace ImGUI
{
void CreateDebugRenderMenuItem::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State *imGUI__State0,
                                       const OksEngine::MainMenuBar *mainMenuBar0,
                                       const OksEngine::Render::Debug::ImGUI::RenderMenu *renderMenu0) {


    CreateComponent<DebugRenderMenuItem>(entity0id);
};

}

} // namespace Debug

} // namespace Render

namespace Render
{
namespace Debug
{
namespace ImGUI
{
void BeginRenderMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State *imGUI__State0,
                             const OksEngine::MainMenuBar *mainMenuBar0,
                             OksEngine::Render::Debug::ImGUI::RenderMenu *renderMenu0) {



    renderMenu0->show_ = false;
    if (ImGui::BeginMenu("Render")) {
        renderMenu0->show_ = true;
    }


};

}

} // namespace Debug

} // namespace Render

namespace Render
{
namespace Debug
{
namespace ImGUI
{
void ProcessDebugRenderMenuItem::Update(
    ECS2::Entity::Id entity0id, const OksEngine::MainMenuBar *mainMenuBar0,
    const OksEngine::ImGUI::State *imGUI__State0, const OksEngine::Render::Debug::ImGUI::RenderMenu *renderMenu0,
    const OksEngine::Render::Debug::ImGUI::DebugRenderMenuItem *debugRenderMenuItem0) {

    if (renderMenu0->show_) {
        if (ImGui::MenuItem("DebugRender", "Ctrl+E")) {
            if (!IsComponentExist<DebugRenderWindow>(entity0id)) {
                CreateComponent<DebugRenderWindow>(entity0id);
                //ECS2::Entity::Id debuggerEntityId = CreateEntity();
                //CreateComponent<Debugger>(debuggerEntityId);
                //CreateComponent<CommonParameters>(debuggerEntityId);
                //CreateComponent<ConstraintsParameters>(debuggerEntityId);
            }
        }
    }


};

}

} // namespace Debug

} // namespace Render

namespace Render
{
namespace Debug
{
namespace ImGUI
{
void BeginDebugRenderWindow::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State *imGUI__State0,
                                    const OksEngine::MainMenuBar *mainMenuBar0,
                                    const OksEngine::Render::Debug::ImGUI::RenderMenu *renderMenu0,
                                    const OksEngine::Render::Debug::ImGUI::DebugRenderMenuItem *debugRenderMenuItem0,
                                    OksEngine::Render::Debug::ImGUI::DebugRenderWindow *debugRenderWindow0,
                                    ECS2::Entity::Id entity1id, const OksEngine::Render::Debug::Debugger *debugger1) {


    bool isOpen = true;

    ImGui::Begin("Debug Render parameters",
        &isOpen,
        ImGuiWindowFlags_AlwaysVerticalScrollbar);

    if (!isOpen) {
        RemoveComponent<DebugRenderWindow>(entity0id);
        DestroyEntity(entity1id);
    }

};

}

} // namespace Debug

} // namespace Render

namespace Render
{
    namespace Debug
    {
        namespace ImGUI
        {
            void ShowRenderParameters::Update(
                ECS2::Entity::Id entity0id,
                const OksEngine::ImGUI::State* imGUI__State0,
                const OksEngine::MainMenuBar* mainMenuBar0,
                const OksEngine::Render::Debug::ImGUI::RenderMenu* renderMenu0,
                const OksEngine::Render::Debug::ImGUI::DebugRenderMenuItem* debugRenderMenuItem0,
                const OksEngine::Render::Debug::ImGUI::DebugRenderWindow* debugRenderWindow0,

                ECS2::Entity::Id entity1id,
                const OksEngine::Render::Debug::Debugger* debugger1,

                ECS2::Entity::Id entity2id,
                const OksEngine::RenderDriver* renderDriver2) {


                const ECS2::ComponentsFilter debuggerFilter = GetComponentsFilter(entity1id);

                if (debuggerFilter.IsSet<Enable>()) {
                    if (ImGui::Button("Disable render")) {
                        RemoveComponent<Enable>(entity1id);
                    }
                }
                else {
                    if (ImGui::Button("Enable render")) {
                        CreateComponent<Enable>(entity1id);
                    }
                }

                const ECS2::ComponentsFilter renderDriverFilter = GetComponentsFilter(entity2id);

                if (renderDriverFilter.IsSet<EnableRegularRender>()) {
                    if (ImGui::Button("Disable regular render")) {
                        RemoveComponent<EnableRegularRender>(entity2id);
                    }
                }
                else {
                    if (ImGui::Button("Enable regular render")) {
                        CreateComponent<EnableRegularRender>(entity2id);
                    }
                }


            }

        }
    }
}

namespace Render
{
namespace Debug
{
namespace ImGUI
{
void EndDebugRenderWindow::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State *imGUI__State0,
                                  const OksEngine::MainMenuBar *mainMenuBar0,
                                  const OksEngine::Render::Debug::ImGUI::RenderMenu *renderMenu0,
                                  const OksEngine::Render::Debug::ImGUI::DebugRenderMenuItem *debugRenderMenuItem0,
                                  const OksEngine::Render::Debug::ImGUI::DebugRenderWindow *debugRenderWindow0) {

    ImGui::End();
};

}

} // namespace Debug

} // namespace Render

namespace Render
{
namespace Debug
{
namespace ImGUI
{
void EndRenderMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State *imGUI__State0,
                           const OksEngine::MainMenuBar *mainMenuBar0,
                           const OksEngine::Render::Debug::ImGUI::RenderMenu *renderMenu0) {


    if (renderMenu0->show_) {
        ImGui::EndMenu();

    }

};

}

} // namespace Debug

} // namespace Render

} // namespace OksEngine