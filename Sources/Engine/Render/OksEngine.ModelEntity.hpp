#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct ModelEntity : public ECSComponent<ModelEntity> {

		ModelEntity() :
			ECSComponent{ nullptr } { }

		ModelEntity(ECS::Entity::Id id) :
			ECSComponent{ nullptr },
			id_{ id } { }

		ECS::Entity::Id id_;
	};

	template<>
	inline void Edit<ModelEntity>(ModelEntity* modelEntity) {
		ImGui::TextDisabled("Id: %d", modelEntity->id_);
	}


}