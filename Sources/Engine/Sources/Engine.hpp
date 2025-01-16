#pragma once 

#include <memory>
#include <chrono>
#include <list>
#include <utility>

#include <OksEngine.Entity.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>
#include <Common/auto_OksEngine.Position3D.hpp>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>

#include <OS.CommandLineParameters.hpp>

namespace OksEngine {

	class Engine {
	public:

		struct CreateInfo {
			OS::CommandLineParameters commandLineParameters_;
		};

		explicit Engine(const CreateInfo& createInfo) noexcept;

		class Scene {
		public:
		private:

		};

		Entity CreateEntity() noexcept;


		template<class ...Components>
		Entity CreateEntity() noexcept {
			ECS::Entity::Id id = context_->GetECS2World()->CreateEntity<Components...>();
			return Entity{ *context_, context_->GetECSWorld(), id };
		}

		template<class ...Components>
		void AddArchetype() {
			context_->GetECS2World()->AddArchetype<Components...>();
		}

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
