#include <Engine.hpp>



#include <Components/OksEngine.RenderableGeometry.hpp>


namespace OksEngine {

	Engine::Engine() noexcept {

		uiSubsystem_ = std::make_shared<UISubsystem>();
		world_.RegisterSystem<BehaviourSystem>();

		auto renderSystem = world_.RegisterSystem<RenderSystem>();
		/*resourceSubsystem_ = std::shared_ptr<ResourceSubsystem>();

		auto vertexShaderResource = resourceSubsystem_->GetResource("Root/triangle.vert");
		auto fragmentShaderResource = resourceSubsystem_->GetResource("Root/triangle.frag");
		RAL::Shader vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
		RAL::Shader fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };
		RenderSubsystem::CreateInfo renderSubsystemCreateInfo{
			vertexShader,
			fragmentShader
		};
		renderSubsystem_ = std::make_shared<RenderSubsystem>(renderSubsystemCreateInfo);*/
	}

}