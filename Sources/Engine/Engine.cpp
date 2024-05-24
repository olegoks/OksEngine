#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <OksEngine.Render.Subsystem.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <OksEngine.UI.Subsystem.hpp>

namespace OksEngine {

	Engine::Engine() noexcept {
		context_ = std::make_shared<Context>();
	}


<<<<<<< HEAD
		auto renderSystem = world_.RegisterSystem<RenderSystem>();
		/*resourceSubsystem_ = std::shared_ptr<ResourceSubsystem>();

		auto vertexShaderResource = resourceSubsystem_->GetResource("Root/triangle.vert");
		auto fragmentShaderResource = resourceSubsystem_->GetResource("Root/triangle.frag");
		RAL::Shader vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
		RAL::Shader fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };
		RenderSubsystem::CreateInfo renderSubsystemCreateInfo{
			vertexShader,
			fragmentShader
		};
		renderSubsystem_ = std::make_shared<RenderSubsystem>(renderSubsystemCreateInfo);*/
=======
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
>>>>>>> f1c0003fed75d5dbbdc349b4ddfd28017ce8fcef
	}

}