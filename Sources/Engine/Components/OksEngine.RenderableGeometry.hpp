#pragma once

//#include <Vulkan/Render.Vulkan.hpp>

#include <memory>
#include <filesystem>

//#include <Engine.ECS;
//#include <RAL;
//#include <UIAL;
//#include <Resource;

#include <ECS.hpp>



namespace OksEngine {

	class RenderSystem : public ECS::System {
	public:

		RenderSystem() noexcept {

			//Render::Vulkan::API api;
			//RAL::Driver::CreateInfo driverCreateInfo
			{

			}
			//api.CreateDriver(driverCreateInfo);
			 
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

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<RenderSystem>().GetId();
		}

		//std::shared_ptr<RAL::API> renderApi_ = nullptr;
		//std::shared_ptr<RAL::Driver> renderDriver_ = nullptr;

		//std::shared_ptr<UIAL::API> uiApi_ = nullptr;
		//std::shared_ptr<UIAL::Window> window_ = nullptr;

		//std::shared_ptr<Resource::ResourceManager> resourceManager_ = nullptr;

	};


	struct RenderableGeometry : public ECS::IComponent<RenderableGeometry> {

	};

}