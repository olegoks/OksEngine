#pragma once 

#include <OksEngine.ECS.Component.hpp>
namespace OksEngine {
#undef PlaySound
	struct PlaySound : public ECSComponent<PlaySound> {

		PlaySound() :
			ECSComponent{ nullptr } { }
		
	};

	template<>
	inline void Edit<PlaySound>(PlaySound* playSound) {
		
	}

	template<>
	inline void Add<PlaySound>(ECS::World* world, ECS::Entity::Id id) {
		//static char geom[100] = { "USPHit.ogg" };
		//if (ImGui::CollapsingHeader("Create info")) {
		//	ImGui::InputText("Audio file:", geom, sizeof(geom));
		//}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<PlaySound>(id)) {
				world->CreateComponent<PlaySound>(id);
			}
		}

	}

}