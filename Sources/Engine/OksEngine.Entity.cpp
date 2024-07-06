
#include <OksEngine.Entity.hpp>
#include <OksEngine.Behaviour.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>
#include <OksEngine.MapRigidBodyToRenderGeometry.hpp>
#include <OksEngine.Camera.hpp>

namespace OksEngine {

	void Entity::AddImmutableRenderGeometry(const glm::mat4& modelMatrix, std::string objName, std::string mtlName, std::string textureName) {
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
		const glm::mat4& transform,
		float mass,
		float staticFriction,
		float dynamicFriction,
		float restitution,
		float halfExtentX,
		float halfExtentY, 
		float halfExtentZ) {
		world_->CreateComponent<RigidBody>(
			GetId(),
			&context_,
			transform,
			mass,
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
		const glm::vec3& position,
		const glm::vec3& direction,
		const glm::vec3& up) {
		world_->CreateComponent<Camera>(
			GetId(),
			&context_,
			position,
			direction,
			up);
	}

}