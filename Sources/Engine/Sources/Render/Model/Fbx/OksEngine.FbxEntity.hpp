#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class FbxEntity : public ECSComponent<FbxEntity> {
	public:

		FbxEntity(ECS::Entity::Id id) : ECSComponent{ nullptr }, id_{ id } {}

		ECS::Entity::Id id_;
	};

}