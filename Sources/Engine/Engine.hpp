#pragma once 

#include <memory>
#include <chrono>
#include <list>
#include <utility>

#include <ECS.hpp>
#include <Components/OksEngine.Behaviour.hpp>
#include <Components/OksEngine.Position.hpp>

#include <OksEngine.CommandLineParameters.hpp>

namespace OksEngine {

	class Entity final {
	public:

		Entity(std::shared_ptr<ECS::World> world, ECS::Entity::Id id) noexcept :
			world_{ world },
			id_{ id } {

			

		}

		void AddBehaviour(
			std::filesystem::path scriptsPath,
			const char* scriptName,
			const char* objectName) {
			world_->CreateComponent<Behaviour>(
				GetId(),
				GetWorld(),
				GetId(),
				scriptsPath,
				scriptName,
				objectName);
		}

		void AddPosition(int x, int y, int z) {
			world_->CreateComponent<Position>(GetId(), x, y, z);
		}

		//template<class ...Args>
		//void AddRenderableGeometry(Args&& ...args) {
		//	world_->CreateComponent<RenderableGeometry>(GetId(), std::forward<Args>(args)...);
		//}

	private:
		[[nodiscard]]
		inline ECS::World* GetWorld() noexcept { return world_.get(); }
		[[nodiscard]]
		inline ECS::Entity::Id GetId() noexcept { return id_; }
	private:
		std::shared_ptr<ECS::World> world_ = nullptr;
		ECS::Entity::Id id_;
	};

	class Engine {
	public:

		struct CreateInfo {
			CommandLineParameters commandLineParameters_;
		};

		explicit Engine(const CreateInfo& createInfo) noexcept;

		class Scene {
		public:
		private:

		};

		Entity CreateEntity() noexcept;

		void Run() {
			isRunning_ = true;
			while (IsRunning()) {
				Update();
			}

		}

		inline bool IsRunning() const noexcept {
			return isRunning_;
		}

		void Update() noexcept;

	private:
		bool isRunning_ = false;
		std::shared_ptr<class Context> context_;
	};

}
