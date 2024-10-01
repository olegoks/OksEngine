
#include <memory>
#include <Behaviour/OksEngine.Behaviour.hpp>


#include <UI/OksEngine.UI.Subsystem.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <ECS.hpp>

#include <OksEngine.Config.hpp>

#include <Common/OksEngine.Log.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Subsystem.hpp>
#include <Physics/OksEngine.Physics.Systems.hpp>
#include <Physics/OksEngine.PhysicsShape.hpp>
#include <Physics/OksEngine.DynamicRigidBodyCustomMeshShape.hpp>
#include <Common/OksEngine.Common.Systems.hpp>
#include <Debug/OksEngine.Debug.Subsystem.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Render/OksEngine.GeometryStorage.hpp>
#include <Behaviour/OksEngine.Behaviour.Systems.hpp>

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
		uiSubsystem_ = std::make_shared<UISubsystem>(UISubsystem::CreateInfo{ *this });

		world_->RegisterSystem<LoadLuaScript>(*this);
		world_->RegisterSystem<LoadResourceSystem>(*this);
		world_->RegisterSystem<CreateLuaContext>(*this);
		world_->RegisterSystem<CallUpdateMethod>(*this);
		world_->RegisterSystem<CallInputProcessor>(*this);
		world_->RegisterSystem<RigidBodyToRenderGeometryMapper>(*this);
		world_->RegisterSystem <CreatePhysicsShape>(*this);
		world_->RegisterSystem <CreateDynamicRigidBody>(*this);
		//world_b->RegisterSystem<AttachCameraSystem>(*this);

		meshStorage_ = std::make_shared<TaggedStorage<Geometry::Mesh>>();
		modelStorage_ = std::make_shared<ModelStorage>();
		textureStorage_ = std::make_shared<TaggedStorage<Geometry::Texture>>();
		scriptStorage_ = std::make_shared<TaggedStorage<Lua::Script>>();

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
