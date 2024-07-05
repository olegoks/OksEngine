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

		void AddImmutableRenderGeometry(
			const glm::mat4& modelMatrix,
			std::string objName,
			std::string mtlName,
			std::string textureName);

		void AddBehaviour(
			const char* scriptName,
			const char* objectName);

		void AddCamera(const glm::vec3& position,
			const glm::vec3& direction);

		void AddPosition(float x, float y, float z);
		void AddMapRigidBodyToRenderGeometry();
		void AddRigidBody(
			const glm::mat4& transform,
			float staticFriction_,
			float dynamicFriction_,
			float restitution_,
			float halfExtentX,
			float halfExtentY,
			float halfExtentZ);

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