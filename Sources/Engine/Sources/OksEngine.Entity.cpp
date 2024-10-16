
#include <OksEngine.Entity.hpp>
#include <OksEngine.Components.hpp>

namespace OksEngine {
	void Entity::AddName(const std::string& name) {
		world_->CreateComponent<Name>(
			GetId(),
			name);
	}
	void Entity::AddDebugInfo(const std::string& name) {
		world_->CreateComponent<DebugInfo>(
			GetId(),
			&context_,
			name);
	}

	void Entity::AddImmutableRenderGeometry(const std::string& tag) {
		world_->CreateComponent<ImmutableRenderGeometry>(
			GetId(), tag);
	}

	void Entity::AddSkinnedGeometry(const glm::mat4& modelMatrix, std::string objName, std::string mtlName, const std::vector<std::string>& textures) {
		world_->CreateComponent<SkinnedGeometry>(
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

	void Entity::AddStaticRigidBodyCustomMeshShape() {
		world_->CreateComponent<StaticRigidBodyCustomMeshShape>(
			GetId());
	}

	void Entity::AddDynamicRigidBodyCustomMeshShape() {
		world_->CreateComponent<DynamicRigidBodyCustomMeshShape>(
			GetId());
	}

	void Entity::AddMass(float mass) {
		world_->CreateComponent<Mass>(
			GetId(), mass);
	}

	void Entity::AddMaterial(float staticFriction, float dynamicFriction, float restitution) {
		world_->CreateComponent<Material>(
			GetId(),
			staticFriction,
			dynamicFriction,
			restitution);
	}

	void Entity::AddRigidBodyBox(
		float mass,
		float linearDamping,
		float angularDamping,
		float staticFriction,
		float dynamicFriction,
		float restitution,
		float halfExtentX,
		float halfExtentY, 
		float halfExtentZ) {
	/*	world_->CreateComponent<DynamicRigidBodyBox>(
			GetId(),
			mass,
			linearDamping,
			angularDamping,
			Material{
				staticFriction,
				dynamicFriction,
				restitution
			},
			halfExtentX,
			halfExtentY,
			halfExtentZ);*/
	}


	void Entity::AddRigidBodyCapsule(
		float mass,
		float linearDamping,
		float angularDamping,
		float staticFriction,
		float dynamicFriction,
		float restitution,
		float radius,
		float height) {
		/*world_->CreateComponent<DynamicRigidBodyCapsule>(
			GetId(),
			mass,
			linearDamping,
			angularDamping,
			Material{ 
				staticFriction,
				dynamicFriction,
				restitution
			},
			radius,
			height);*/
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
			scriptName,
			objectName);
	}

	void Entity::AddPosition(float x, float y, float z) {
		world_->CreateComponent<Position>(
			GetId(),
			x, y, z);
	}



	void Entity::AddDirection(float x, float y, float z) {
		world_->CreateComponent<Direction>(
			GetId(),
			x, y, z);
	}

	void Entity::AddUp(float x, float y, float z) {
		world_->CreateComponent<Up>(
			GetId(),
			x, y, z);
	}

	void Entity::AddWidth(Common::Size width) {
		world_->CreateComponent<Width>(
			GetId(),
			width);
	}

	void Entity::AddHeight(Common::Size height) {
		world_->CreateComponent<Height>(
			GetId(),
			height);
	}
	
	void Entity::AddActive() {
		world_->CreateComponent<Active>(
			GetId());
	}

	void Entity::AddRotation(const glm::vec3& rotateVector, Math::Angle angle) {
		world_->CreateComponent<Rotation>(
			GetId(),
			rotateVector,
			angle
			);
	}

	void Entity::AddCamera(
		//const glm::vec3& position,
		const glm::vec3& direction,
		const glm::vec3& up, bool isActive) {
		world_->CreateComponent<Camera>(
			GetId(),
			//position,
			//direction,
			up,
			isActive);
	}



}