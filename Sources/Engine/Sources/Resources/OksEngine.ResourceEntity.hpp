#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct ResourceEntity : public ECSComponent<ResourceEntity> {

		ResourceEntity() :
			ECSComponent{ nullptr } { }

		ResourceEntity(ECS::Entity::Id id) :
			ECSComponent{ nullptr },
			id_{ id } { }

		ECS::Entity::Id id_;
	};

	template<>
	inline void Edit<ResourceEntity>(ResourceEntity* resourceEntity) {
		ImGui::TextDisabled("Id: %d", resourceEntity->id_);
	}


}