#pragma once 

#include <memory>
#include <chrono>
#include <list>
#include <utility>

#include <OksEngine.Entity.hpp>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Rotation3D.hpp>
#include <Common/auto_OksEngine.Active.hpp>
#include <Render/auto_OksEngine.ImmutableRenderGeometry.hpp>

#include <OS.CommandLineParameters.hpp>

namespace OksEngine {

	class Engine {
	public:

		struct CreateInfo {
			int argc_ = 0;
			char** argv_ = nullptr;
		};

		explicit Engine(const CreateInfo& createInfo) noexcept;


		Entity CreateEntity() noexcept {
			ECS2::Entity::Id id = context_->GetECS2World()->CreateEntity();

			return Entity{ *context_, context_->GetECS2World(), id };
		}

		template<class ...Components>
		void AddArchetype() noexcept {
			//context_->GetECS2World()->AddArchetype<Components...>();
		}

		template<class ...Components>
		Entity CreateEntity() noexcept {
			ECS2::Entity::Id id = context_->GetECS2World()->CreateEntity<Components...>();
			
			return Entity{ *context_, context_->GetECS2World(), id };
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
