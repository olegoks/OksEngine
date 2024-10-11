#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Active : public ECSComponent<Active> {
	public:

		Active() :
			ECSComponent{ nullptr }{ }

	private:
	};

	template<>
	inline void Bind<Active>(Lua::Context& context) {

	}

	template<>
	inline void Edit<Active>(Active* camera) {

	}

	template<>
	inline void Add<Active>(ECS::World* world, ECS::Entity::Id id) {
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Active>(id)) {
				world->CreateComponent<Active>(id);
			}
		}
	}


}
