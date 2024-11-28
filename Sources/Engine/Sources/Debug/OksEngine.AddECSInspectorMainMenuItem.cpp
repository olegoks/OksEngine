
#include <Debug/OksEngine.AddECSInspectorMainMenuItem.hpp>

#include <Debug/OksEngine.ECSInspector.hpp>

namespace OksEngine {

	AddECSInspectorMainMenuItem::AddECSInspectorMainMenuItem(Context& context) noexcept :
		ECSSystem{ context } { }

	void AddECSInspectorMainMenuItem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* ecsInspector = world->GetComponent<ECSInspector>(entityId);
		if (ecsInspector != nullptr) {

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

	inline std::pair<ECS::Entity::Filter, ECS::Entity::Filter> AddECSInspectorMainMenuItem::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<ECSInspector>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId AddECSInspectorMainMenuItem::GetTypeId() const noexcept {
		return Common::TypeInfo<AddECSInspectorMainMenuItem>().GetId();
	}

}