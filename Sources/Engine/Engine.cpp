#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <UI/OksEngine.UI.Subsystem.hpp>

/*Debug*/
#include <Debug/OksEngine.FramesCounter.hpp>

/*Reneder*/
#include <Render/OksEngine.Render.System.hpp>

/*Common*/
#include <Common/OksEngine.Engine.System.hpp>


/*Physics*/
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Physics/OksEngine.Physics.System.hpp>

/*UI*/
#include <UI/OksEngine.UI.System.hpp>
#include <UI/OksEngine.UI.Window.hpp>

#include <Debug/OksEngine.Debug.Subsystem.hpp>

namespace OksEngine {

	Engine::Engine(const CreateInfo& createInfo) noexcept {
		Context::CreateInfo contextCreateInfo{
			createInfo.commandLineParameters_
		};
		context_ = std::make_shared<Context>(contextCreateInfo);
	}

	void Engine::Update() noexcept {
		while (IsRunning()) {
			context_->GetUISubsystem()->Update();
			context_->GetPhysicsSubsystem()->Update();
			{
				context_->GetECSWorld()->StartFrame();

				/*UI*/
				context_->GetECSWorld()->RunSystem<GetWindowKeyboardInputEvents>();				
				context_->GetECSWorld()->RunSystem<UISystem>();
				context_->GetECSWorld()->RunSystem<WindowSystem>();
				context_->GetECSWorld()->RunSystem<SendWindowKeyboardInputEvents>();
				context_->GetECSWorld()->RunSystem<CleanWindowKeyboardInputEvents>();
				/*UI*/

				context_->GetECSWorld()->RunSystem<BehaviourSystem>();
				context_->GetECSWorld()->RunSystem<PhysicsSystem>();
				context_->GetECSWorld()->RunSystem<PhysicsGeometryMapper>();
				context_->GetECSWorld()->RunSystem<CameraSystem>();
				context_->GetECSWorld()->RunSystem<AttachCameraSystem>();

				/*ImGui*/
				context_->GetECSWorld()->RunSystem<ImGuiSystem>();
				ImGui::NewFrame();
				context_->GetECSWorld()->RunSystem<MainMenuBarSystem>();
				context_->GetECSWorld()->RunSystem<EnginePerformanceSystem>();
				context_->GetECSWorld()->RunSystem<ECSInspectorSystem>();
				context_->GetECSWorld()->RunSystem<CollectEntitiesInfo>();
				ImGui::Render();
				context_->GetECSWorld()->RunSystem<ImGuiRenderSystem>();
				/*ImGui*/
				context_->GetECSWorld()->RunSystem<RenderSystem>();

				context_->GetECSWorld()->RunSystem<FramesCounterSystem>();
				context_->GetECSWorld()->EndFrame();
			}
			context_->GetRenderSubsystem()->Update();
			context_->GetDebugSubsystem()->Update();
		}
	}

	Entity Engine::CreateEntity() noexcept {
		ECS::Entity::Id id = context_->GetECSWorld()->CreateEntity();
		return Entity{ *context_, context_->GetECSWorld(), id };
	}

}