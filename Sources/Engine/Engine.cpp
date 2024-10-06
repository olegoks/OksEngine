#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <UI/OksEngine.UI.Subsystem.hpp>

/*Debug*/
#include <Debug/OksEngine.FramesCounter.hpp>
#include <Debug/OksEngine.Debug.Systems.hpp>
/*Reneder*/
#include <Render/OksEngine.Render.Systems.hpp>

/*Common*/
#include <Common/OksEngine.Common.Systems.hpp>


/*Physics*/
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Systems.hpp>
#include <Physics/OksEngine.PhysicsShape.hpp>

/*UI*/
#include <UI/OksEngine.UI.System.hpp>
#include <UI/OksEngine.UI.Window.hpp>

/*Resource*/
#include <Resources/OksEngine.Resources.Components.hpp>

/*Behaviour*/
#include <Behaviour/OksEngine.Behaviour.Systems.hpp>

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

				context_->GetECSWorld()->RunSystem<LoadResourceSystem>();
				/*UI*/
				context_->GetECSWorld()->RunSystem<GetWindowKeyboardInputEvents>();
				context_->GetECSWorld()->RunSystem<GetWindowFramebufferResizedEvents>();
				context_->GetECSWorld()->RunSystem<GetWindowMouseInputEvents>();
				context_->GetECSWorld()->RunSystem<UISystem>();
				context_->GetECSWorld()->RunSystem<WindowSystem>();
				context_->GetECSWorld()->RunSystem<SendWindowKeyboardInputEvents>();
				context_->GetECSWorld()->RunSystem<CleanWindowKeyboardInputEvents>();
				context_->GetECSWorld()->RunSystem<SendWindowMouseInputEvents>();
				context_->GetECSWorld()->RunSystem<CleanWindowMouseInputEvents>();

				/*Behaviour*/
				context_->GetECSWorld()->RunSystem<LoadLuaScript>();
				context_->GetECSWorld()->RunSystem<CreateLuaContext>();
				context_->GetECSWorld()->RunSystem<CallUpdateMethod>();
				context_->GetECSWorld()->RunSystem<CallInputProcessor>();
				context_->GetECSWorld()->RunSystem<CreatePhysicsShapeForDynamicRigidBody>();
				context_->GetECSWorld()->RunSystem<CreatePhysicsShapeForStaticRigidBody>();
				context_->GetECSWorld()->RunSystem<CreateDynamicRigidBody>();
				context_->GetECSWorld()->RunSystem<CreateStaticRigidBody>();
				context_->GetECSWorld()->RunSystem<RigidBodyToRenderGeometryMapper>();
				context_->GetECSWorld()->RunSystem<CreateDriverCamera>();
				context_->GetECSWorld()->RunSystem<UpdateDriverCamera>();

				/*ImGui*/
				context_->GetECSWorld()->RunSystem<CreateImGuiTexture>();
				ImGui::NewFrame();
				context_->GetECSWorld()->RunSystem<UpdateMainMenuBar>();
				context_->GetECSWorld()->RunSystem<UpdateEnginePerformance>();
				context_->GetECSWorld()->RunSystem<AddECSInspectorMainMenuItem>();
				context_->GetECSWorld()->RunSystem<CollectEntitiesInfo>();
				context_->GetECSWorld()->RunSystem<CollectECSSystemsCallsInfo>();
				ImGui::Render();
				context_->GetECSWorld()->RunSystem<RenderImGuiUI>();
				/*ImGui*/

				/*Render*/
				context_->GetECSWorld()->RunSystem<CreateLoadGeometryDescriptionFileRequest>();
				context_->GetECSWorld()->RunSystem<LoadGeometryDescriptionFile>();
				//context_->GetECSWorld()->RunSystem<CreateLoadMeshRequest>();
				context_->GetECSWorld()->RunSystem<CreateLoadObjRequest>();
				context_->GetECSWorld()->RunSystem<CreateLoadMtlRequest>();

				context_->GetECSWorld()->RunSystem<CreateObj>();
				context_->GetECSWorld()->RunSystem<CreateMtl>();
				context_->GetECSWorld()->RunSystem<CreateModelEntityFromObjMtl>();

				//context_->GetECSWorld()->RunSystem<LoadMesh>();
				//context_->GetECSWorld()->RunSystem<CreateDriverModel>();

				context_->GetECSWorld()->RunSystem < CreateLoadTextureRequest>();
				context_->GetECSWorld()->RunSystem<CreateTexture>();
				context_->GetECSWorld()->RunSystem<CreateDriverTexture>();
				context_->GetECSWorld()->RunSystem<AddMeshToRender>();
				context_->GetECSWorld()->RunSystem<ResizeFrameBuffers>();
				context_->GetECSWorld()->RunSystem<MapMeshTransform>();
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