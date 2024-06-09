
#include <memory>
#include <OksEngine.Behaviour.hpp>


#include <Components/OksEngine.RenderableGeometry.hpp>
#include <OksEngine.UI.Subsystem.hpp>
#include <OksEngine.Render.Subsystem.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <ECS.hpp>

#include <OksEngine.Config.hpp>

namespace OksEngine
{
	Context::Context(std::filesystem::path configFilePath) :
		config_{ }
	{
		resourceSubsystem_ = std::make_shared<AsyncResourceSubsystem>(*this);
		auto loadConfigTaskId = resourceSubsystem_->GetResource(Subsystem::Type::Engine, configFilePath);
		auto configResource = resourceSubsystem_->GetResource(Subsystem::Type::Engine, loadConfigTaskId);
		const std::string configText{configResource.GetData<char>(), configResource.GetSize() };
		config_.AddText(configText);
		auto resourcesRootPath = config_.GetValueAs<std::string>("ResourceSystem.resourcesRootDirectory");
		resourceSubsystem_->SetRoot(Subsystem::Type::Engine, resourcesRootPath);
		


		uiSubsystem_ = std::make_shared<UISubsystem>(UISubsystem::CreateInfo{ *this });
		world_ = std::make_shared<ECS::World>();
		world_->RegisterSystem<BehaviourSystem>();

		auto renderSystem = world_->RegisterSystem<RenderSystem>();


		RenderSubsystem::CreateInfo renderSubsystemCreateInfo{
			*this,
			//vertexShader,
			//fragmentShader
		};
		renderSubsystem_ = std::make_shared<RenderSubsystem>(renderSubsystemCreateInfo);
		
		//std::string configText(configResource.GetData<char>(), configResource.GetSize());
		//config_.AddText(configText);
	}

}
