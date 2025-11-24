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
			auto* rbRotation = GetComponent<WorldRotation3D>(physics__RigidBodyEntityId0->id_);

			auto* cameraPosition = GetComponent<WorldPosition3D>(bindedCameraEntityId0->id_);
			auto* cameraDirection = GetComponent<Direction3D>(bindedCameraEntityId0->id_);


			glm::quat rbQuat{ rbRotation->w_, rbRotation->x_, rbRotation->y_, rbRotation->z_};

			cameraPosition->x_ = rbPosition->x_ + bindedCameraOffset0->x_;
			cameraPosition->y_ = rbPosition->y_ + bindedCameraOffset0->y_;
			cameraPosition->z_ = rbPosition->z_ + bindedCameraOffset0->z_;
			
			glm::vec3 camera = rbQuat * glm::vec3{ bindedCameraOffset0->x_, bindedCameraOffset0->y_, bindedCameraOffset0->z_};

			cameraPosition->x_ = rbPosition->x_ + camera.x;
			cameraPosition->y_ = rbPosition->y_ + camera.y;
			cameraPosition->z_ = rbPosition->z_ + camera.z;
			

			//glm::vec3 forward = glm::quat{ rbRotation->w_, rbRotation->x_, rbRotation->y_, rbRotation->z_ } *glm::vec3(0.0f, 0.0f, 1.0f);  // Вперед (ось Z)
			
			cameraDirection->x_ = rbPosition->x_ - cameraPosition->x_;
			cameraDirection->y_ = rbPosition->y_ + 1.0  - cameraPosition->y_;
			cameraDirection->z_ = rbPosition->z_ - cameraPosition->z_;


		};

	}

} // namespace OksEngine