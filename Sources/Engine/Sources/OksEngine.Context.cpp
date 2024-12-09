
#include <memory>
#include <Behaviour/OksEngine.Behaviour.hpp>


#include <UI/OksEngine.UI.Subsystem.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Common/OksEngine.Log.Subsystem.hpp>
#include <Debug/OksEngine.Debug.Subsystem.hpp>
#include <ECS.hpp>

#include <OksEngine.Config.hpp>

#include <OksEngine.Components.hpp>
#include <OksEngine.Systems.hpp>

namespace OksEngine
{
	Context::Context(const CreateInfo& createInfo) :
		commandLineParameters_{ createInfo.commandLineParameters_ },
		config_{ }
	{
		CommandLineParameters commandLineParameters = createInfo.commandLineParameters_;
		{
			LogSubsystem::CreateInfo createInfo{
				commandLineParameters,
				*this
			};
			logSubsystem_ = std::make_shared<LogSubsystem>(createInfo);
		}
		{
			resourceSubsystem_ = std::make_shared<AsyncResourceSubsystem>(*this);
			std::filesystem::path configFilePath = commandLineParameters.GetValue("-cfg");
			auto loadConfigTaskId = resourceSubsystem_->GetResource(Subsystem::Type::Engine, configFilePath);
			auto configResource = resourceSubsystem_->GetResource(Subsystem::Type::Engine, loadConfigTaskId);
			config_ = std::make_shared<Config>(std::string{ configResource.GetData<char>(), configResource.GetSize() });
			auto resourcesRootPath = config_->GetValueAs<std::string>("ResourceSystem.resourcesRootDirectory");
			auto fullResourcesPath = configFilePath.parent_path() / resourcesRootPath;

			auto scriptsRootPath = config_->GetValueAs<std::string>("ResourceSystem.scriptsRootDirectory");
			auto scriptsFullResourcesPath = configFilePath.parent_path() / scriptsRootPath;

			resourceSubsystem_->SetRoot(Subsystem::Type::Engine, { scriptsFullResourcesPath, fullResourcesPath });
		}
		


		world_ = std::make_shared<ECS::World>();
		
		//Resource ecs subsystem
		{
			const ECS::Entity::Id resourceSubsystem = world_->CreateEntity();
			world_->CreateComponent<ECSResourceSubsystem>(resourceSubsystem);

		}

		//Frames counter
		{
			const ECS::Entity::Id framesCounter = world_->CreateEntity();
			world_->CreateComponent<FramesCounter>(framesCounter);
			world_->CreateComponent<Counter>(framesCounter);
		}

		world_->RegisterSystem<IncreaseFramesCounter>(*this);
		uiSubsystem_ = std::make_shared<UISubsystem>(UISubsystem::CreateInfo{ *this });
		world_->RegisterSystem<CreateLoadLuaScriptRequest>(*this);

		world_->RegisterSystem<CreateLuaScriptEntity>(*this);
		world_->RegisterSystem<CreateAsyncTask>(*this);
		world_->RegisterSystem < CheckResourceLoaded>(*this);
		world_->RegisterSystem < WaitForResourceLoading>(*this);
		//world_->RegisterSystem < CheckResourceLoaded>(*this);
		world_->RegisterSystem<CreateLuaContext>(*this);
		world_->RegisterSystem<CallUpdateMethod>(*this);
		world_->RegisterSystem<CallInputProcessor>(*this);
		world_->RegisterSystem<RigidBodyToRenderGeometryMapper>(*this);
		world_->RegisterSystem <CreatePhysicsShapeForDynamicRigidBody>(*this);
		world_->RegisterSystem <CreateDynamicRigidBody>(*this);
		world_->RegisterSystem<UpdateClock>(*this);
		world_->RegisterSystem < UpdateDriverTransform3DWithLocalTransform>(*this);


		//ANIMATION
		world_->RegisterSystem < CreateAnimationState>(*this);
		world_->RegisterSystem<ProcessAnimation>(*this);
		world_->RegisterSystem<RunAnimation>(*this);
		world_->RegisterSystem<StopAnimation>(*this);

		RenderSubsystem::CreateInfo renderSubsystemCreateInfo{
			*this,
			//vertexShader,
			//fragmentShader
		};
		renderSubsystem_ = std::make_shared<RenderSubsystem>(renderSubsystemCreateInfo);

		PhysicsSubsystem::CreateInfo physicsSubsystemCreateInfo{
			*this
		};
		physicsSubsystem_ = std::make_shared<PhysicsSubsystem>(physicsSubsystemCreateInfo);
		DebugSubsystem::CreateInfo debugSubsystemCreateInfo{ *this };
		debugSubsystem_ = std::make_shared<DebugSubsystem>(debugSubsystemCreateInfo);
		//std::string configText(configResource.GetData<char>(), configResource.GetSize());
		//config_.AddText(configText);
	}

}
