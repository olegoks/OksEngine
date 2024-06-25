#pragma once 

#include <filesystem>
#include <memory>
#include <ECS.hpp>
#include <OksEngine.Context.hpp>

namespace OksEngine {

	class Entity final {
	public:

		Entity(Context& context, std::shared_ptr<ECS::World> world, ECS::Entity::Id id) noexcept :
			context_{ context },
			world_{ world },
			id_{ id } { }

		void AddDebugRenderableGeometry();

		void AddBehaviour(
			const char* scriptName,
			const char* objectName);

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