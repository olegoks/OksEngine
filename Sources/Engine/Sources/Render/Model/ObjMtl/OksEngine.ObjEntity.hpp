#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class ObjEntity : public ECSComponent<ObjEntity> {
	public:

		ObjEntity(ECS::Entity::Id id) : ECSComponent{ nullptr }, id_{ id } {}

		ECS::Entity::Id id_;
	};

}