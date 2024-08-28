#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class ActiveMarker : public ECSComponent<ActiveMarker> {
	public:

		ActiveMarker() :
			ECSComponent{ nullptr }{ }

	private:
	};

	template<>
	inline void Bind<ActiveMarker>(Lua::Context& context) {

	}

	template<>
	inline void Edit<ActiveMarker>(ActiveMarker* camera) {

	}

	template<>
	inline void Add<ActiveMarker>(ECS::World* world, ECS::Entity::Id id) {
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<ActiveMarker>(id)) {
				world->CreateComponent<ActiveMarker>(id);
			}
		}
	}


}
