#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <OksEngine.Render.Subsystem.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <OksEngine.UI.Subsystem.hpp>
#include <OksEngine.Physics.Subsystem.hpp>

namespace OksEngine {

	Engine::Engine(const CreateInfo& createInfo) noexcept {
		Context::CreateInfo contextCreateInfo{
			createInfo.commandLineParameters_
		};
		context_ = std::make_shared<Context>(contextCreateInfo);
	}

	void Engine::Update() noexcept {
		std::chrono::high_resolution_clock::time_point previousUpdate;
		while (IsRunning()) {
			context_->GetPhysicsSubsystem()->Update();
			context_->GetRenderSubsystem()->Update();
			context_->GetUISubsystem()->Update();
			context_->GetECSWorld()->Process();
			//using namespace std::chrono_literals;
			//auto now = std::chrono::high_resolution_clock::now();
			//auto delta = (now - previousUpdate);
			//std::this_thread::sleep_for(15ms - delta);
			//previousUpdate = now;
		}
	}

	Entity Engine::CreateEntity() noexcept {
		ECS::Entity::Id id = context_->GetECSWorld()->CreateEntity();
		return Entity{ *context_, context_->GetECSWorld(), id };
	}

}