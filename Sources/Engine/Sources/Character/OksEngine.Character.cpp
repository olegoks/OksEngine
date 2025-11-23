#pragma once
#include <Character\auto_OksEngine.Character.hpp>

namespace OksEngine
{

	namespace Character
	{
		void UpdateBindedCameraPosition::Update(ECS2::Entity::Id entity0id, const Character::CharacterEntity* characterEntity0,
			const Character::BindedCameraEntityId* bindedCameraEntityId0,
			const Character::BindedCameraOffset* bindedCameraOffset0,
			const Physics::RigidBodyEntityId* physics__RigidBodyEntityId0, ECS2::Entity::Id entity1id,
			RenderDriver* renderDriver1) {

			auto* rbPosition = GetComponent<WorldPosition3D>(physics__RigidBodyEntityId0->id_);
			auto* cameraPosition = GetComponent<WorldPosition3D>(bindedCameraEntityId0->id_);


			cameraPosition->x_ = rbPosition->x_ + bindedCameraOffset0->x_;
			cameraPosition->y_ = rbPosition->y_ + bindedCameraOffset0->y_;
			cameraPosition->z_ = rbPosition->z_ + bindedCameraOffset0->z_;

		};

	}

} // namespace OksEngine