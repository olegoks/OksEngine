
#include <OksEngine.Entity.hpp>
#include <OksEngine.Behaviour.hpp>
#include <OksEngine.ImmutableRenderGeometry.hpp>
#include <OksEngine.Light.hpp>
#include <OksEngine.MapRigidBodyToRenderGeometry.hpp>
#include <OksEngine.Camera.hpp>
#include <OksEngine.AttachedCamera.hpp>

namespace OksEngine {

	void Entity::AddImmutableRenderGeometry(const glm::mat4& modelMatrix, std::string objName, std::string mtlName, const std::vector<std::string>& textures) {
		world_->CreateComponent<ImmutableRenderGeometry>(
			GetId(),
			&context_,
			modelMatrix,
			objName,
			mtlName,
			textures);
	}

	void Entity::AddMapRigidBodyToRenderGeometry() {
		world_->CreateComponent<MapRigidBodyToRenderGeometry>(
			GetId(),
			&context_);
	}

	void Entity::AddRigidBodyBox(
		const glm::mat4& transform,
		float mass,
		float linearDamping,
		float angularDamping,
		float staticFriction,
		float dynamicFriction,
		float restitution,
		float halfExtentX,
		float halfExtentY, 
		float halfExtentZ) {
		world_->CreateComponent<RigidBodyBox>(
			GetId(),
			&context_,
			transform,
			mass,
			linearDamping,
			angularDamping,
			staticFriction,
			dynamicFriction, 
			restitution,
			halfExtentX,
			halfExtentY,
			halfExtentZ);
	}


	void Entity::AddRigidBodyCapsule(
		const glm::mat4& transform,
		float mass,
		float linearDamping,
		float angularDamping,
		float staticFriction,
		float dynamicFriction,
		float restitution,
		float radius,
		float height) {
		world_->CreateComponent<RigidBodyCapsule>(
			GetId(),
			&context_,
			transform,
			mass,
			linearDamping,
			angularDamping,
			staticFriction,
			dynamicFriction,
			restitution,
			radius,
			height);
	}

	void Entity::AddLight(
		const glm::vec3& color,
		float intensity,
		float radius) {
		world_->CreateComponent<PointLight>(
			GetId(),
			&context_,
			color,
			intensity,
			radius);
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

	void Entity::AddAttachedCamera(
		const glm::mat4& offset) {
		world_->CreateComponent<AttachedCamera>(
			GetId(),
			&context_,
			offset);
	}


}