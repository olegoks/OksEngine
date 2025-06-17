
#include <memory>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Common/OksEngine.Log.Subsystem.hpp>
//#include <ECS.hpp>

#include <OksEngine.Config.hpp>

#include <Render/auto_OksEngine.RenderDriver.hpp>
#include <Resources/auto_OksEngine.ResourceSystem.hpp>
#include <Common/CommandLineParameters/auto_OksEngine.Components.hpp>

namespace OksEngine
{
	Context::Context(const CreateInfo& createInfo) :
		config_{ }
	{
		world2_ = std::make_shared<ECS2::World>();

		{
			const int argc = createInfo.argc_;
			char** argv = createInfo.argv_;

			const ECS2::Entity::Id commandLineParametersEntity = world2_->CreateEntity();
			world2_->CreateComponent<CommandLineParameters>(commandLineParametersEntity);
			world2_->CreateComponent<ExecutablePath>(commandLineParametersEntity, argv[0]);

			for (int i = 1; i < argc; i++) {

				const char* value = argv[i];
				using namespace std::string_literals;
				if (value == "-cfg"s) {
					world2_->CreateComponent<ConfigFilePath>(commandLineParametersEntity, argv[i + 1]);
				}
			}

		}


		{
			LogSubsystem::CreateInfo lsci{
				createInfo.argc_,
				createInfo.argv_,
				*this
			};
			logSubsystem_ = std::make_shared<LogSubsystem>(lsci);
		}

		//resourceSubsystem_ = std::make_shared<AsyncResourceSubsystem>(*this);

		//const ECS2::Entity::Id resourceSystemEntity = world2_->CreateEntity();
		//world2_->CreateComponent<ResourceSystem>(resourceSystemEntity, resourceSubsystem_);

		//
		//std::vector<std::string_view> values = commandLineParameters.GetValue("-cfg");
		//OS::Assert(values.size() == 1);
		//std::filesystem::path configFilePath = values[0];
		//auto loadConfigTaskId = resourceSubsystem_->GetResource(Subsystem::Type::Engine, configFilePath);
		//auto configResource = resourceSubsystem_->GetResource(Subsystem::Type::Engine, loadConfigTaskId);
		//config_ = std::make_shared<Config>(std::string{ configResource.GetData<char>(), configResource.GetSize() });
		//auto resourcesRootPath = config_->GetValueAs<std::string>("ResourceSystem.resourcesRootDirectory");
		//auto fullResourcesPath = configFilePath.parent_path() / resourcesRootPath;

		//auto scriptsRootPath = config_->GetValueAs<std::string>("ResourceSystem.scriptsRootDirectory");
		//auto scriptsFullResourcesPath = configFilePath.parent_path() / scriptsRootPath;

		//auto ecsFilesRootPath = config_->GetValueAs<std::string>("ECSCallGraphEditor.ecsFilesRootDirectory");
		//auto ecsFilesFullResourcesPath = configFilePath.parent_path() / ecsFilesRootPath;

		//auto scenesFilesRootPath = config_->GetValueAs<std::string>("scenesRootDirectory");
		//auto scenesFilesFullResourcesPath = configFilePath.parent_path() / scenesFilesRootPath;


		//resourceSubsystem_->SetRoot(Subsystem::Type::Engine, { scenesFilesFullResourcesPath, scriptsFullResourcesPath, fullResourcesPath, ecsFilesFullResourcesPath });


		//world_ = std::make_shared<ECS::World>();


		//world2_->AddArchetype<>
		////Resource ecs subsystem
		//{
		//	const ECS::Entity::Id resourceSubsystem = world_->CreateEntity();
		//	world_->CreateComponent<ECSResourceSubsystem>(resourceSubsystem);

		//}

		////Frames counter
		//{
		//	const ECS::Entity::Id framesCounter = world_->CreateEntity();
		//	world_->CreateComponent<FramesCounter>(framesCounter);
		//	world_->CreateComponent<Counter>(framesCounter);
		//}

		//world_->RegisterSystem<IncreaseFramesCounter>(*this);
		//uiSubsystem_ = std::make_shared<UISubsystem>(UISubsystem::CreateInfo{ *this });
		//const ECS2::Entity::Id mainWindowEntityId = world2_->CreateEntity();
		//world2_->CreateComponent<MainWindow>(mainWindowEntityId, uiSubsystem_->GetWindow());
		//world_->RegisterSystem<CreateLoadLuaScriptRequest>(*this);

		//world_->RegisterSystem<CreateLuaScriptEntity>(*this);
		//world_->RegisterSystem<CreateAsyncTask>(*this);
		//world_->RegisterSystem < CheckResourceLoaded>(*this);
		//world_->RegisterSystem < WaitForResourceLoading>(*this);
		////world_->RegisterSystem < CheckResourceLoaded>(*this);
		//world_->RegisterSystem<CreateLuaContext>(*this);
		//world_->RegisterSystem<CallUpdateMethod>(*this);
		//world_->RegisterSystem<CallInputProcessor>(*this);
		//world_->RegisterSystem<RigidBodyToRenderGeometryMapper>(*this);
		//world_->RegisterSystem <CreatePhysicsShapeForDynamicRigidBody>(*this);
		//world_->RegisterSystem <CreateDynamicRigidBody>(*this);
		////world_->RegisterSystem<UpdateClock>(*this);
		//world_->RegisterSystem < UpdateModelDriverTransform>(*this);

		////ANIMATION
		//world_->RegisterSystem < CreateAnimationState>(*this);
		//world_->RegisterSystem<ProcessAnimation>(*this);
		//world_->RegisterSystem<RunAnimation>(*this);
		//world_->RegisterSystem<StopAnimation>(*this);
		//world_->RegisterSystem < StartModelAnimation>(*this);

		////SOUND
		//world_->RegisterSystem < CreateLoadSoundRequest>(*this);
		//world_->RegisterSystem < CreateSound>(*this);
		//world_->RegisterSystem < ProcessSound>(*this);

		//RenderSubsystem::CreateInfo renderSubsystemCreateInfo{
		//	*this,
		//	//vertexShader,
		//	//fragmentShader
		//};
		//renderSubsystem_ = std::make_shared<RenderSubsystem>(renderSubsystemCreateInfo);

		//const ECS2::Entity::Id entityId = world2_->CreateEntity();
		//world2_->CreateComponent<RenderDriver>(entityId, renderSubsystem_->GetDriver());

		PhysicsSubsystem::CreateInfo physicsSubsystemCreateInfo{
			*this
		};
		physicsSubsystem_ = std::make_shared<PhysicsSubsystem>(physicsSubsystemCreateInfo);

	}

}
