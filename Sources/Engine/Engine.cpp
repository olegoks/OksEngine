#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <UI/OksEngine.UI.Subsystem.hpp>

/*Reneder*/
#include <Render/OksEngine.Render.System.hpp>

/*Common*/
#include <Common/OksEngine.Engine.System.hpp>
#include <Common/OksEngine.FramesCounter.hpp>

/*Physics*/
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Physics/OksEngine.Physics.System.hpp>

/*UI*/
#include <UI/OksEngine.UI.System.hpp>

#include <Debug/OksEngine.Debug.Subsystem.hpp>

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
			context_->GetUISubsystem()->Update();
			context_->GetPhysicsSubsystem()->Update();
			{
				context_->GetECSWorld()->RunSystem<UISystem>();
				context_->GetECSWorld()->RunSystem<BehaviourSystem>();
				context_->GetECSWorld()->RunSystem<PhysicsSystem>();
				context_->GetECSWorld()->RunSystem<PhysicsGeometryMapper>();
				context_->GetECSWorld()->RunSystem<CameraSystem>();
				context_->GetECSWorld()->RunSystem<AttachCameraSystem>();
				//context_->GetECSWorld()->RunSystem<CameraSystem>();
				ImGui::NewFrame();
				context_->GetECSWorld()->RunSystem<MainMenuBarSystem>();
				context_->GetECSWorld()->RunSystem<EnginePerformanceSystem>();
				context_->GetECSWorld()->RunSystem<ECSInspectorSystem>();
				ImGui::Render();
				context_->GetECSWorld()->RunSystem<RenderSystem>();
				context_->GetECSWorld()->RunSystem<FramesCounterSystem>();
				//context_->GetECSWorld()->AddDelayedComponents();
			}
			//context_->GetECSWorld()->Process();
			//UI Subsystem must be updated before render to call ImGui_ImplGlfw_NewFrame()

			context_->GetRenderSubsystem()->Update();
			context_->GetDebugSubsystem()->Update();
		}
	}

	Entity Engine::CreateEntity() noexcept {
		ECS::Entity::Id id = context_->GetECSWorld()->CreateEntity();
		return Entity{ *context_, context_->GetECSWorld(), id };
	}

}