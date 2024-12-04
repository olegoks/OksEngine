#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct StartAnimation : public ECSComponent<StartAnimation> {
		struct StateInfo {
			Common::Double time_ = 0;
			glm::vec3 translate_;
		};
		StartAnimation() : ECSComponent{ nullptr } {}

	};

	template<>
	inline void Edit<StartAnimation>(StartAnimation* animation) {

	}


	template<>
	inline void Add<StartAnimation>(ECS::World* world, ECS::Entity::Id id) {
		if (ImGui::CollapsingHeader("Create info")) {
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<StartAnimation>(id)) {
				world->CreateComponent<StartAnimation>(id);
			}
		}
	}

}