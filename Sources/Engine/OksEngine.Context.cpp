
#include <memory>
#include <OksEngine.Behaviour.hpp>


#include <OksEngine.UI.Subsystem.hpp>
#include <OksEngine.Render.Subsystem.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <ECS.hpp>

#include <OksEngine.Config.hpp>

#include <OksEngine.Log.Subsystem.hpp>
#include <OksEngine.Physics.Subsystem.hpp>
#include <OksEngine.Engine.System.hpp>

namespace OksEngine
{
	Context::Context(const CreateInfo& createInfo) :
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

		world_->RegisterSystem<BehaviourSystem>();
		world_->RegisterSystem<PhysicsGeometryMapper>(*this);
		world_->RegisterSystem<AttachCameraSystem>(*this);

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

		//std::string configText(configResource.GetData<char>(), configResource.GetSize());
		//config_.AddText(configText);
	}

}
