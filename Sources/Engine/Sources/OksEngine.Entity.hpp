#pragma once 

#include <filesystem>
#include <memory>
#include <OksEngine.Context.hpp>
#include <Math.Matrix.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	class Entity final {
	public:

		Entity(Context& context, std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) noexcept :
			context_{ context },
			world_{ world },
			id_{ id } { }

		void AddName(const std::string& name);
		void AddDebugInfo(const std::string& name);

		void AddImmutableRenderGeometry(const std::string& meshTag, const std::string& shaderTag);

		void AddSkinnedGeometry(
			const glm::mat4& modelMatrix,
			std::string objName,
			std::string mtlName,
			const std::vector<std::string>& textures);

		void AddBehaviourScriptName(
			const char* scriptName,
			const char* objectName);

		void AddCamera();

		void AddPosition(float x, float y, float z);
		void AddLocalPosition(float x, float y, float z);
		void AddDirection(float x, float y, float z);

		void AddUp3D(float x, float y, float z);

		void AddWidth(Common::Size width);

		void AddHeight(Common::Size height);

		void AddZNear(float zNear);

		void AddZFar(float zFar);

		void AddActive();

		void AddClock();

		void AddRotation(const glm::vec3& rotateVector, Math::Angle angle);

		void AddMapRigidBodyToRenderGeometry();

		void AddStaticRigidBodyCustomMeshShape();

		void AddDynamicRigidBodyCustomMeshShape();
		void AddMass(float mass);
		void AddMaterial(float staticFriction, float dynamicFriction, float restitution);
		//void AddMass();
		//void AddMaterial;

		void AddRigidBodyBox(
			float mass,
			float linearDamping,
			float angularDamping,
			float staticFriction,
			float dynamicFriction,
			float restitution,
			float halfExtentX,
			float halfExtentY,
			float halfExtentZ);

		void AddRigidBodyCapsule(
			float mass,
			float linearDamping,
			float angularDamping,
			float staticFriction,
			float dynamicFriction,
			float restitution,
			float radius,
			float height);

		void AddLight(
			const glm::vec3& color,
			float intensity,
			float radius);

	private:
		[[nodiscard]]
		inline ECS2::World* GetWorld() noexcept { return world_.get(); }
		[[nodiscard]]
		inline ECS2::Entity::Id GetId() noexcept { return id_; }
	private:
		Context& context_;
		std::shared_ptr<ECS2::World> world_ = nullptr;
		ECS2::Entity::Id id_;
	};

}