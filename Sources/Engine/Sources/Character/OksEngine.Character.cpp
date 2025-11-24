#pragma once
#include <Character\auto_OksEngine.Character.hpp>

namespace OksEngine
{

	namespace Character
	{
		void UpdateBindedCameraPosition::Update(ECS2::Entity::Id entity0id, const Character::CharacterEntity* characterEntity0,
			const Character::BindedCameraEntityId* bindedCameraEntityId0,
			const Character::BindedCameraOffset* bindedCameraOffset0,
			const Physics::RigidBodyEntityId* physics__RigidBodyEntityId0) {

			auto* rbPosition = GetComponent<WorldPosition3D>(physics__RigidBodyEntityId0->id_);
			auto* rbRotation = GetComponent<WorldRotation3D>(physics__RigidBodyEntityId0->id_);

			auto* cameraPosition = GetComponent<WorldPosition3D>(bindedCameraEntityId0->id_);
			auto* cameraDirection = GetComponent<Direction3D>(bindedCameraEntityId0->id_);


			glm::quat rbQuat{ rbRotation->w_, rbRotation->x_, rbRotation->y_, rbRotation->z_};

			//cameraPosition->x_ = rbPosition->x_ + bindedCameraOffset0->x_;
			//cameraPosition->y_ = rbPosition->y_ + bindedCameraOffset0->y_;
			//cameraPosition->z_ = rbPosition->z_ + bindedCameraOffset0->z_;
			//
			glm::vec3 camera = rbQuat * glm::vec3{ bindedCameraOffset0->x_, bindedCameraOffset0->y_, bindedCameraOffset0->z_};

			cameraPosition->x_ = rbPosition->x_ + camera.x;
			cameraPosition->y_ = rbPosition->y_ + camera.y;
			cameraPosition->z_ = rbPosition->z_ + camera.z;
			

			//glm::vec3 forward = glm::quat{ rbRotation->w_, rbRotation->x_, rbRotation->y_, rbRotation->z_ } *glm::vec3(0.0f, 0.0f, 1.0f);  // Вперед (ось Z)
			
			cameraDirection->x_ = rbPosition->x_ - cameraPosition->x_;
			cameraDirection->y_ = rbPosition->y_ + 1.0  - cameraPosition->y_;
			cameraDirection->z_ = rbPosition->z_ - cameraPosition->z_;


		};

		void UpdateCharacterDirection::Update(
			ECS2::Entity::Id entity0id, const Character::CharacterEntity* characterEntity0,
			Direction3D* direction3D0, const Physics::RigidBodyEntityId* physics__RigidBodyEntityId0) {

			auto* worldRotation3D = GetComponent<WorldRotation3D>(physics__RigidBodyEntityId0->id_);

			glm::quat rbRotation{ worldRotation3D->w_, worldRotation3D->x_, worldRotation3D->y_, worldRotation3D->z_ };
			//HACK
			glm::vec3 startDirection{ 0.0, 0.0, -1.0 };

			glm::vec3 actualDirection = rbRotation * startDirection;
			direction3D0->x_ = actualDirection.x;
			direction3D0->y_ = actualDirection.y;
			direction3D0->z_ = actualDirection.z;

		}

	}

} // namespace OksEngine