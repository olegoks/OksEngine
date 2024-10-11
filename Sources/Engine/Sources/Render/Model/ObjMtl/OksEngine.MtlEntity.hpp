#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class MtlEntity : public ECSComponent<MtlEntity> {
	public:

		MtlEntity(ECS::Entity::Id id) : ECSComponent{ nullptr }, id_{ id } {}

		ECS::Entity::Id id_;
	};

}