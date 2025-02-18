
#include <Debug/auto_OksEngine.AddECSInspectorMainMenuItem.hpp>

#include <Debug/auto_OksEngine.ECSInspector.hpp>

namespace OksEngine {

	void AddECSInspectorMainMenuItem::Update(ECSInspector* ecsInspector) {

		if (ImGui::BeginMainMenuBar()) {
			// Add items to the menu bar.
			if (ImGui::BeginMenu("Engine")) {
				ImGui::MenuItem("ECS Inspector", nullptr, &ecsInspector->show_);
				ImGui::EndMenu();
			}
			// End the menu bar.
			ImGui::EndMainMenuBar();
		}
	}

}