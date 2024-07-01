
#include <OksEngine.Entity.hpp>
#include <OksEngine.Behaviour.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>
#include <OksEngine.MapRigidBodyToRenderGeometry.hpp>
#include <OksEngine.Camera.hpp>

namespace OksEngine {

	void Entity::AddImmutableRenderGeometry(const Math::Matrix4x4f& modelMatrix, std::string objName, std::string mtlName, std::string textureName) {
		world_->CreateComponent<ImmutableRenderGeometry>(
			GetId(),
			&context_,
			modelMatrix,
			objName,
			mtlName,
			textureName);
	}

	void Entity::AddMapRigidBodyToRenderGeometry() {
		world_->CreateComponent<MapRigidBodyToRenderGeometry>(
			GetId(),
			&context_);
	}

	void Entity::AddRigidBody(
		float staticFriction,
		float dynamicFriction,
		float restitution,
		float halfExtentX,
		float halfExtentY, 
		float halfExtentZ) {
		world_->CreateComponent<RigidBody>(
			GetId(),
			&context_,
			staticFriction,
			dynamicFriction, 
			restitution,
			halfExtentX,
			halfExtentY,
			halfExtentZ);
	}

	void Entity::AddBehaviour(
		const char* scriptName,
		const char* objectName) {
		world_->CreateComponent<Behaviour>(
			GetId(),
			&context_,
			GetId(),
			scriptName,
			objectName);
	}

	void Entity::AddPosition(float x, float y, float z) {
		world_->CreateComponent<Position>(
			GetId(),
			x, y, z);
	}

	void Entity::AddCamera(
		const Math::Vector3f& position,
		const Math::Vector3f& direction) {
		world_->CreateComponent<Camera>(
			GetId(),
			&context_,
			position,
			direction);
	}

}