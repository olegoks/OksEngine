#pragma once 

#include <filesystem>
#include <memory>
#include <ECS.hpp>
#include <OksEngine.Context.hpp>
#include <Math.Matrix.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	class Entity final {
	public:

		Entity(Context& context, std::shared_ptr<ECS::World> world, ECS::Entity::Id id) noexcept :
			context_{ context },
			world_{ world },
			id_{ id } { }

		void AddDebugInfo(const std::string& name);

		void AddImmutableRenderGeometry(
			const glm::mat4& modelMatrix,
			std::string objName,
			std::string mtlName,
			const std::vector<std::string>& textures);

		void AddSkinnedGeometry(
			const glm::mat4& modelMatrix,
			std::string objName,
			std::string mtlName,
			const std::vector<std::string>& textures);

		void AddBehaviour(
			const char* scriptName,
			const char* objectName);

		void AddCamera(
			const glm::vec3& position,
			const glm::vec3& direction,
			const glm::vec3& up);

		void AddPosition(float x, float y, float z);

		void AddMapRigidBodyToRenderGeometry();

		void AddStaticRigidBodyCustomMeshShape(
			const glm::mat4& transform,
			const std::string& geomName,
			float staticFriction,
			float dynamicFriction,
			float restitution);

		void AddDynamicRigidBodyCustomMeshShape(
			const glm::mat4& transform,
			const std::string& geomName,
			float mass,
			float linearDamping,
			float angularDamping,
			float staticFriction,
			float dynamicFriction,
			float restitution);

		void AddRigidBodyBox(
			const glm::mat4& transform,
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
			const glm::mat4& transform,
			float mass,
			float linearDamping,
			float angularDamping,
			float staticFriction,
			float dynamicFriction,
			float restitution,
			float radius,
			float height);

		void AddAttachedCamera(
			const glm::mat4& offset);

		void AddLight(
			const glm::vec3& color,
			float intensity,
			float radius);

	private:
		[[nodiscard]]
		inline ECS::World* GetWorld() noexcept { return world_.get(); }
		[[nodiscard]]
		inline ECS::Entity::Id GetId() noexcept { return id_; }
	private:
		Context& context_;
		std::shared_ptr<ECS::World> world_ = nullptr;
		ECS::Entity::Id id_;
	};

}