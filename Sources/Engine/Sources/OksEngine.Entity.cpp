
#include <OksEngine.Entity.hpp>
#include <OksEngine.ECS.hpp>

namespace OksEngine {
	void Entity::AddName(const std::string& name) {
		world_->CreateComponent<Name>(
			GetId(),
			name);
	}
	void Entity::AddDebugInfo(const std::string& name) {
		world_->CreateComponent<DebugInfo>(
			GetId(),
			name);
	}

	void Entity::AddImmutableRenderGeometry(const std::string& meshTag, const std::string& shaderTag) {
		world_->CreateComponent<ImmutableRenderGeometry>(
			GetId(), meshTag, shaderTag);
	}

	void Entity::AddClock() {
		world_->CreateComponent<TimeSinceEpoch>(
			GetId());
	}

	void Entity::AddSkinnedGeometry(const glm::mat4& modelMatrix, std::string objName, std::string mtlName, const std::vector<std::string>& textures) {
		//world_->CreateComponent<SkinnedGeometry>(
		//	GetId(),
		//	&context_,
		//	modelMatrix,
		//	objName,
		//	mtlName,
		//	textures);
	}

	void Entity::AddMapRigidBodyToRenderGeometry() {
		/*world_->CreateComponent<MapRigidBodyToRenderGeometry>(
			GetId());*/
	}

	void Entity::AddStaticRigidBodyCustomMeshShape() {
		world_->CreateComponent<Physics::StaticRigidBodyCustomMeshShape>(
			GetId());
	}

	void Entity::AddDynamicRigidBodyCustomMeshShape() {
		world_->CreateComponent<Physics::DynamicRigidBodyCustomMeshShape>(
			GetId());
	}

	void Entity::AddMass(float mass) {
		world_->CreateComponent<Physics::Mass>(
			GetId(), mass);
	}

	void Entity::AddMaterial(float staticFriction, float dynamicFriction, float restitution) {
		world_->CreateComponent<Physics::Material>(
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
		/*world_->CreateComponent<PointLight>(
			GetId(),
			&context_,
			color,
			intensity,
			radius);*/
	}

	void Entity::AddBehaviourScriptName(
		const char* scriptName,
		const char* objectName) {
		world_->CreateComponent<Behaviour::ScriptName>(
			GetId(),
			scriptName);
	}

	void Entity::AddPosition(float x, float y, float z) {
		world_->CreateComponent<Position3D>(
			GetId(),
			x, y, z);
	}

	
	void Entity::AddLocalPosition(float x, float y, float z) {
		OS::AssertFail();
		/*world_->CreateComponent<LocalPosition3D>(
			GetId(),
			x, y, z);*/
	}


	void Entity::AddDirection(float x, float y, float z) {
		world_->CreateComponent<Direction3D>(
			GetId(),
			x, y, z);
	}
	void Entity::AddZNear(float zNear) {
		world_->CreateComponent<ZNear>(
			GetId(),
			zNear);
	}
	void Entity::AddZFar(float zFar) {
		world_->CreateComponent<ZFar>(
			GetId(),
			zFar);
	}

	void Entity::AddUp3D(float x, float y, float z) {
		world_->CreateComponent<Up3D>(
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

		// �������������� ���� �� �������� � �������
		float radians = glm::radians(angle.ToDegrees().GetValue());

		// �������� �����������
		glm::quat quat = glm::angleAxis(radians, glm::normalize(rotateVector));

		world_->CreateComponent<Rotation3D>(
			GetId(),
			quat.w,
			quat.x,
			quat.y,
			quat.z
			);
	}

	void Entity::AddCamera() {
		world_->CreateComponent<Camera>(
			GetId());
	}



}