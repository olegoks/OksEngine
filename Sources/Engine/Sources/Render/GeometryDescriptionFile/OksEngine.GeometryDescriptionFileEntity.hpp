#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class GeometryDescriptionFileEntity : public ECSComponent<GeometryDescriptionFileEntity> {
	public:

		GeometryDescriptionFileEntity(ECS::Entity::Id id) : ECSComponent{ nullptr }, id_{ id } {}

		ECS::Entity::Id id_;
	};

}