
#include <memory>
#include <OksEngine.Behaviour.hpp>


#include <Components/OksEngine.RenderableGeometry.hpp>
#include <OksEngine.UI.Subsystem.hpp>
#include <OksEngine.Render.Subsystem.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <ECS.hpp>

namespace OksEngine
{
	Context::Context()
	{

		uiSubsystem_ = std::make_shared<UISubsystem>(UISubsystem::CreateInfo{ *this });
		world_ = std::make_shared<ECS::World>();
		world_->RegisterSystem<BehaviourSystem>();
		resourceSubsystem_ = std::make_shared<AsyncResourceSubsystem>(*this);

		auto renderSystem = world_->RegisterSystem<RenderSystem>();


		RenderSubsystem::CreateInfo renderSubsystemCreateInfo{
			*this,
			//vertexShader,
			//fragmentShader
		};
		renderSubsystem_ = std::make_shared<RenderSubsystem>(renderSubsystemCreateInfo);
	}

}
