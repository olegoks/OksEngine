#pragma once

#include <memory>
#include <filesystem>

//import OksEngine.Engine.ECS;
//import OksEngine.RAL;
//import OksEngine.UIAL;
//import OksEngine.Resource;

#include <ECS.hpp>
#include <Vulkan/Render.Vulkan.hpp>


namespace OksEngine::Engine {

	class RenderSystem : public ECS::System {
	public:

		RenderSystem() noexcept {

			Render::Vulkan::API api;
			api.CreateDriver();

			//uiApi_ = UIAL::CreateAPIObject();
			//UIAL::Window::CreateInfo windowCreateInfo;
			//{
			//	windowCreateInfo.title_ = "OksEngine";
			//	windowCreateInfo.windowSize_ = { 800, 600 };
			//}
			//window_ = uiApi_->CreateWindow(windowCreateInfo);
			//
			///*resourceManager_ = std::make_shared<Resource::ResourceManager>("D:/Desktop/OksEngine/Resources/");
			//resourceManager_->Load();*/

			//auto vertexShader = std::make_shared<Resource::Resource>("./../shaders/fragmentShader.spv");
			//vertexShader->Load();
			//auto fragmentShader = std::make_shared<Resource::Resource>("./../shaders/vertexShader.spv");
			//fragmentShader->Load();

			//renderApi_ = RAL::CreateAPIObject();
			//RAL::Driver::CreateInfo driverCreateInfo;
			//{
			//	driverCreateInfo.enableDepthBuffer_ = false;
			//	driverCreateInfo.windowInfo_ = window_->GetInfo();
			//	driverCreateInfo.vertexShader_ = vertexShader;//resourceManager_->GetResource("triangleFrag.spv");
			//	driverCreateInfo.fragmentShader_ = fragmentShader;//resourceManager_->GetResource("triangleVert.spv");
			//}
			//renderDriver_ = renderApi_->CreateDriver(driverCreateInfo);
			//window_->Show();
		}

	public:
		
		virtual void Update(ECS::World* world, ECS::Entity::Id entityId) const override {
			

		}

	private:

		//std::shared_ptr<RAL::API> renderApi_ = nullptr;
		//std::shared_ptr<RAL::Driver> renderDriver_ = nullptr;

		//std::shared_ptr<UIAL::API> uiApi_ = nullptr;
		//std::shared_ptr<UIAL::Window> window_ = nullptr;

		//std::shared_ptr<Resource::ResourceManager> resourceManager_ = nullptr;

	};


	struct RenderableGeometry : public ECS::IComponent<RenderableGeometry> {

	};

}