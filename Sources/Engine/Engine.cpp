#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <OksEngine.Render.Subsystem.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <OksEngine.UI.Subsystem.hpp>

namespace OksEngine {

	Engine::Engine(std::filesystem::path configFilePath) noexcept {
		context_ = std::make_shared<Context>(configFilePath);
	}


	void Engine::Update() noexcept {
		while (IsRunning()) {
			context_->renderSubsystem_->Update();
			context_->uiSubsystem_->Update();
			context_->world_->Process();
			//OS::LogInfo("Engine/loop", "Loop is running");
		}
	}

	Entity Engine::CreateEntity() noexcept {
		ECS::Entity::Id id = context_->world_->CreateEntity();
		return Entity{ context_->world_.get(), id };
	}

}