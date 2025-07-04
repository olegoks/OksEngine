
#include <auto_OksEngine.RunSystems.hpp>


#include <pix3.h>

#include <Engine.hpp>

#include <OksEngine.Context.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>

#include <OksEngine.ECS.hpp>



namespace OksEngine {

	Engine::Engine(const CreateInfo& createInfo) noexcept {
		Context::CreateInfo contextCreateInfo{
			createInfo.argc_,
			createInfo.argv_
		};
		context_ = std::make_shared<Context>(contextCreateInfo);
	}

	void Engine::Update() noexcept {

		std::shared_ptr<ECS2::World> world2 = context_->GetECS2World();


		HRESULT r;
		r = SetThreadDescription(GetCurrentThread(), L"Main thread");
	
		CreateThreads(world2);

		RunInitializeSystems(world2);

		while (IsRunning()) {

			RunSystems(world2);

			////	/*Resources*/
			////	context_->GetECSWorld()->RunSystem<CreateAsyncTask>();
			////	context_->GetECSWorld()->RunSystem<CheckResourceLoaded>();
			////	context_->GetECSWorld()->RunSystem<WaitForResourceLoading>();
			////	///*UI*/
			////	context_->GetECSWorld()->RunSystem<GetWindowKeyboardInputEvents>();
			////	context_->GetECSWorld()->RunSystem<GetWindowFramebufferResizedEvents>();
			////	context_->GetECSWorld()->RunSystem<GetWindowMouseInputEvents>();
			////	context_->GetECSWorld()->RunSystem<UISystem>();
			////	context_->GetECSWorld()->RunSystem<WindowSystem>();
			////	context_->GetECSWorld()->RunSystem<SendWindowKeyboardInputEvents>();
			////	context_->GetECSWorld()->RunSystem<CleanWindowKeyboardInputEvents>();
			////	context_->GetECSWorld()->RunSystem<SendWindowMouseInputEvents>();
			////	context_->GetECSWorld()->RunSystem<CleanWindowMouseInputEvents>();

			////	//SOUND
			////	context_->GetECSWorld()->RunSystem<CreateLoadSoundRequest>();
			////	context_->GetECSWorld()->RunSystem<CreateSound>();
			////	context_->GetECSWorld()->RunSystem<ProcessSound>();

			////	///*Behaviour*/
			////	context_->GetECSWorld()->RunSystem<CreateLoadLuaScriptRequest>();
			////	context_->GetECSWorld()->RunSystem<CreateLuaScriptEntity>();
			////	context_->GetECSWorld()->RunSystem<CreateLuaContext>();
			////	context_->GetECSWorld()->RunSystem<CallUpdateMethod>();
			////	context_->GetECSWorld()->RunSystem<CallInputProcessor>();

			////	///*Physics*/
			////	context_->GetECSWorld()->RunSystem<CreatePhysicsShapeForDynamicRigidBody>();
			////	context_->GetECSWorld()->RunSystem<CreatePhysicsShapeForStaticRigidBody>();
			////	context_->GetECSWorld()->RunSystem<CreateDynamicRigidBody>();
			////	context_->GetECSWorld()->RunSystem<CreateStaticRigidBody>();
			////	context_->GetECSWorld()->RunSystem<RigidBodyToRenderGeometryMapper>();


			////	/*ImGui*/

			//	//RunCreateImGuiTextureSystem(world2);
			//	//ImGui::NewFrame();
			//	//RunUpdateMainMenuBarSystem(world2);
			//	//RunUpdateEnginePerformanceSystem(world2);
			//	//RunAddECSInspectorMainMenuItemSystem(world2);
			//	//RunCollectEntitiesInfoSystem(world2);
			//	////context_->GetECSWorld()->RunSystem<CollectECSSystemsCallsInfo>();
			//	//ImGui::Render();
			//	//RunCreateImGuiModelSystem(world2);
			//	//RunAddImGuiMeshToRenderSystem(world2);
			//	//RunUpdateImGuiRenderDataSystem(world2);

			////	/*ImGui*/

			////	/*Animation*/
			////	context_->GetECSWorld()->RunSystem < StartModelAnimation>();
			////	context_->GetECSWorld()->RunSystem<CreateAnimationState>();
			////	context_->GetECSWorld()->RunSystem<RunAnimation>();
			////	context_->GetECSWorld()->RunSystem<ProcessAnimation>();
			////	context_->GetECSWorld()->RunSystem < StopAnimation>();
			////	/*Animation*/

			////	/*Render*/
			////RunCreateCameraTransformDriverUniformBufferSystem(world2);
			////RunUpdateCameraTransformUniformBufferSystem(world2);
			////RunCreateLoadGeometryDescriptionFileTaskSystem(world2);
			////RunCheckGeometryDescriptionFileLoadedSystem(world2);
			////RunCreateGeometryDescriptionFileInfoSystem(world2);
			////RunCreateGeometryDescriptionFileSystem(world2);
			////	context_->GetECSWorld()->RunSystem<CreateLoadObjRequest>();
			////	context_->GetECSWorld()->RunSystem<CreateObjEntity>();
			////	context_->GetECSWorld()->RunSystem<CreateLoadMtlRequest>();
			////	context_->GetECSWorld()->RunSystem<CreateMtlEntity>();

			////	context_->GetECSWorld()->RunSystem<CreateLoadFbxRequest>();
			////	context_->GetECSWorld()->RunSystem<CreateFbxEntity>();
			////	
			////	context_->GetECSWorld()->RunSystem<CreateModelEntityFromObjMtl>();
			////	context_->GetECSWorld()->RunSystem<CreateModelEntityFromFbx>();

			////	context_->GetECSWorld()->RunSystem<CreateLoadTextureRequest>();
			////	context_->GetECSWorld()->RunSystem<CreateTexture>();
			////	context_->GetECSWorld()->RunSystem<CreateDriverVertexBuffer>();
			////	context_->GetECSWorld()->RunSystem<CreateDriverVertex2DBuffer>();
			////	context_->GetECSWorld()->RunSystem<CreateDriverIndexBuffer>();
			////	context_->GetECSWorld()->RunSystem<UpdateDriverVertexBuffer>();
			////	context_->GetECSWorld()->RunSystem<UpdateDriverIndexBuffer>();
			////	context_->GetECSWorld()->RunSystem<CreateDriverTexture>();

			////RunCreateDriverTransform3DSystem(world2);
			////	//context_->GetECSWorld()->RunSystem<UpdateDriverTransform3D>();
			////	//context_->GetECSWorld()->RunSystem<UpdateDriverTransform3DWithLocalTransform>();
			////	context_->GetECSWorld()->RunSystem < UpdateModelDriverTransform>();
			////	context_->GetECSWorld()->RunSystem<AddMeshToRender>();

			////	context_->GetECSWorld()->RunSystem<ResizeFrameBuffers>();
			////	context_->GetECSWorld()->RunSystem<MapMeshTransform>();
			////	context_->GetECSWorld()->RunSystem<IncreaseFramesCounter>();


			////	context_->GetECSWorld()->EndFrame();
			////}

			//world2->EndFrame();
			//++frameNumber;
			//PIXEndEvent();

		}

	}


}