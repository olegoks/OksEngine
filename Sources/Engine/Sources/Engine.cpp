#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <UI/OksEngine.UI.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>

#include <OksEngine.Systems.hpp>

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
				/*Common*/

				context_->GetECSWorld()->RunSystem<UpdateClock>();

				/*Common*/
				/*Resources*/
				context_->GetECSWorld()->RunSystem<CreateAsyncTask>();
				context_->GetECSWorld()->RunSystem<CheckResourceLoaded>();
				context_->GetECSWorld()->RunSystem<WaitForResourceLoading>();
				///*UI*/
				context_->GetECSWorld()->RunSystem<GetWindowKeyboardInputEvents>();
				context_->GetECSWorld()->RunSystem<GetWindowFramebufferResizedEvents>();
				context_->GetECSWorld()->RunSystem<GetWindowMouseInputEvents>();
				context_->GetECSWorld()->RunSystem<UISystem>();
				context_->GetECSWorld()->RunSystem<WindowSystem>();
				context_->GetECSWorld()->RunSystem<SendWindowKeyboardInputEvents>();
				context_->GetECSWorld()->RunSystem<CleanWindowKeyboardInputEvents>();
				context_->GetECSWorld()->RunSystem<SendWindowMouseInputEvents>();
				context_->GetECSWorld()->RunSystem<CleanWindowMouseInputEvents>();

				///*Behaviour*/
				context_->GetECSWorld()->RunSystem<CreateLoadLuaScriptRequest>();
				context_->GetECSWorld()->RunSystem<CreateLuaScriptEntity>();
				context_->GetECSWorld()->RunSystem<CreateLuaContext>();
				context_->GetECSWorld()->RunSystem<CallUpdateMethod>();
				context_->GetECSWorld()->RunSystem<CallInputProcessor>();

				///*Physics*/
				context_->GetECSWorld()->RunSystem<CreatePhysicsShapeForDynamicRigidBody>();
				context_->GetECSWorld()->RunSystem<CreatePhysicsShapeForStaticRigidBody>();
				context_->GetECSWorld()->RunSystem<CreateDynamicRigidBody>();
				context_->GetECSWorld()->RunSystem<CreateStaticRigidBody>();
				context_->GetECSWorld()->RunSystem<RigidBodyToRenderGeometryMapper>();


				/*ImGui*/

				context_->GetECSWorld()->RunSystem<CreateImGuiTexture>();
				ImGui::NewFrame();
				context_->GetECSWorld()->RunSystem<UpdateMainMenuBar>();
				context_->GetECSWorld()->RunSystem<UpdateEnginePerformance>();
				context_->GetECSWorld()->RunSystem<AddECSInspectorMainMenuItem>();
				context_->GetECSWorld()->RunSystem<CollectEntitiesInfo>();
				//context_->GetECSWorld()->RunSystem<CollectECSSystemsCallsInfo>();
				ImGui::Render();
				context_->GetECSWorld()->RunSystem<CreateImGUIModel>();
				context_->GetECSWorld()->RunSystem<AddImGuiMeshToRender>();
				context_->GetECSWorld()->RunSystem<UpdateImGUIRenderData>();

				/*ImGui*/

				/*Animation*/
				context_->GetECSWorld()->RunSystem<CreateAnimationState>();
				context_->GetECSWorld()->RunSystem<RunAnimation>();
				context_->GetECSWorld()->RunSystem<ProcessAnimation>();
				context_->GetECSWorld()->RunSystem < StopAnimation>();
				/*Animation*/

				/*Render*/
				context_->GetECSWorld()->RunSystem<CreateDriverCameraTransformDriverUniformBuffer>();
				context_->GetECSWorld()->RunSystem<UpdateCameraTransformUniformBuffer>();
				context_->GetECSWorld()->RunSystem<CreateLoadGeometryDescriptionFileRequest>();
				context_->GetECSWorld()->RunSystem<CreateGeometryDescriptionFileEntity>();
				context_->GetECSWorld()->RunSystem<CreateLoadObjRequest>();
				context_->GetECSWorld()->RunSystem<CreateObjEntity>();
				context_->GetECSWorld()->RunSystem<CreateLoadMtlRequest>();
				context_->GetECSWorld()->RunSystem<CreateMtlEntity>();

				context_->GetECSWorld()->RunSystem<CreateLoadFbxRequest>();
				context_->GetECSWorld()->RunSystem<CreateFbxEntity>();
				
				context_->GetECSWorld()->RunSystem<CreateModelEntityFromObjMtl>();
				context_->GetECSWorld()->RunSystem<CreateModelEntityFromFbx>();

				context_->GetECSWorld()->RunSystem<CreateLoadTextureRequest>();
				context_->GetECSWorld()->RunSystem<CreateTexture>();
				context_->GetECSWorld()->RunSystem<CreateDriverVertexBuffer>();
				context_->GetECSWorld()->RunSystem<CreateDriverVertex2DBuffer>();
				context_->GetECSWorld()->RunSystem<CreateDriverIndexBuffer>();
				context_->GetECSWorld()->RunSystem<UpdateDriverVertexBuffer>();
				context_->GetECSWorld()->RunSystem<UpdateDriverIndexBuffer>();
				context_->GetECSWorld()->RunSystem<CreateDriverTexture>();

				context_->GetECSWorld()->RunSystem<CreateDriverTransform3D>();
				//context_->GetECSWorld()->RunSystem<UpdateDriverTransform3D>();
				//context_->GetECSWorld()->RunSystem<UpdateDriverTransform3DWithLocalTransform>();
				context_->GetECSWorld()->RunSystem < UpdateModelDriverTransform>();
				context_->GetECSWorld()->RunSystem<AddMeshToRender>();

				context_->GetECSWorld()->RunSystem<ResizeFrameBuffers>();
				context_->GetECSWorld()->RunSystem<MapMeshTransform>();
				context_->GetECSWorld()->RunSystem<IncreaseFramesCounter>();


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