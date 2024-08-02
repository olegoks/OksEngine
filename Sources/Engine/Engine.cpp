#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <UI/OksEngine.UI.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>

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
			context_->GetECSWorld()->Process();
			//UI Subsystem must be updated before render to call ImGui_ImplGlfw_NewFrame()
			context_->GetUISubsystem()->Update();
			context_->GetRenderSubsystem()->Update();
			using namespace std::chrono_literals;
			auto now = std::chrono::high_resolution_clock::now();
			auto delta = (now - previousUpdate);
			std::this_thread::sleep_for(15ms - delta);
			previousUpdate = now;
		}
	}

	Entity Engine::CreateEntity() noexcept {
		ECS::Entity::Id id = context_->GetECSWorld()->CreateEntity();
		return Entity{ *context_, context_->GetECSWorld(), id };
	}

}