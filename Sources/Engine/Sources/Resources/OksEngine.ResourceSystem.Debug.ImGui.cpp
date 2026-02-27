#pragma once
#include <Resources\auto_OksEngine.ResourceSystem.Debug.ImGui.hpp>

namespace OksEngine
{


	namespace Resource
	{
		namespace Debug
		{
			namespace ImGUI
			{
				void CreateMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0) {
				
					CreateComponent<Menu>(entity0id, false);
				};

				void CreateMenuItem::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::Resource::Debug::ImGUI::Menu* menu0) {
				
					CreateComponent<MenuItem>(entity0id);
				
				};


				void BeginMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0, OksEngine::Resource::Debug::ImGUI::Menu* menu0) {
				


					menu0->show_ = false;
					if (ImGui::BeginMenu("Resources")) {
						menu0->show_ = true;
					}

				
				};


				void ProcessMenuItem::Update(ECS2::Entity::Id entity0id, const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::Resource::Debug::ImGUI::Menu* menu0,
					const OksEngine::Resource::Debug::ImGUI::MenuItem* menuItem0) {
					

					if (menu0->show_) {
						if (ImGui::MenuItem("Debug", "Ctrl+E")) {
							if (!IsComponentExist<Window>(entity0id)) {
								CreateComponent<Window>(entity0id);
								
							}
						}
					}
				
				
				};


				void BeginWindow::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::Resource::Debug::ImGUI::Menu* menu0,
					const OksEngine::Resource::Debug::ImGUI::MenuItem* menuItem0,
					OksEngine::Resource::Debug::ImGUI::Window* window0) {
				


					bool isOpen = true;

					ImGui::Begin("Resources Debug",
						&isOpen,
						ImGuiWindowFlags_AlwaysVerticalScrollbar);

					if (!isOpen) {
						RemoveComponent<Window>(entity0id);
					}
				
				
				};


				void ShowResources::Update(
					ECS2::Entity::Id entity0id,
					const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0, 
					const OksEngine::Resource::Debug::ImGUI::Menu* menu0,
					const OksEngine::Resource::Debug::ImGUI::MenuItem* menuItem0,
					const OksEngine::Resource::Debug::ImGUI::Window* window0,
					
					ECS2::Entity::Id entity1id,
					const OksEngine::Resource::Tag* resource__Tag1,
					const OksEngine::Resource::Path* resource__Path1,
					const OksEngine::Resource::Data* resource__Data1) {
					
					if (ImGui::CollapsingHeader(resource__Path1->path_.c_str()))
					{
						ImGui::TextDisabled("Size: %d bytes.", resource__Data1->data_.size());
						ImGui::Spacing();
					}
					
				
				};


				void EndWindow::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0, const OksEngine::Resource::Debug::ImGUI::Menu* menu0,
					const OksEngine::Resource::Debug::ImGUI::MenuItem* menuItem0,
					const OksEngine::Resource::Debug::ImGUI::Window* window0) {
				
					ImGui::End();
				
				};


				void EndMenu::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0,
					const OksEngine::MainMenuBar* mainMenuBar0,
					const OksEngine::Resource::Debug::ImGUI::Menu* menu0) {
				

					if (menu0->show_) {
						ImGui::EndMenu();

					}
				
				};

			}

		} // namespace Debug

	} // namespace Resource

} // namespace OksEngine