#pragma once 

#include <filesystem>
#include <memory>
#include <ECS.hpp>
#include <OksEngine.Context.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	class Entity final {
	public:

		Entity(Context& context, std::shared_ptr<ECS::World> world, ECS::Entity::Id id) noexcept :
			context_{ context },
			world_{ world },
			id_{ id } { }

		void AddImmutableRenderGeometry(
			const Math::Matrix4x4f& modelMatrix,
			std::string objName,
			std::string mtlName,
			std::string textureName);

		void AddBehaviour(
			const char* scriptName,
			const char* objectName);

		void AddCamera(const Math::Vector3f& position,
			const Math::Vector3f& direction);

		void AddPosition(int x, int y, int z);

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