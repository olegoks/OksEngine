#pragma once 

#include <OksEngine.Subsystem.hpp>
#include <RAL.hpp>

#include <OksEngine.Resource.Subsystem.hpp>
#include <Geometry.Shapes.hpp>
#include "OksEngine.UI.Subsystem.hpp"

#include <Components/OksEngine.RenderableGeometry.hpp>
namespace OksEngine {

	class RenderSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
		};

		RenderSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Render, createInfo.context_ } {

			auto& context = GetContext();
			auto config = context.GetConfig();

			auto ecsWorld = context.GetECSWorld();
			ecsWorld->RegisterSystem<RenderSystem>();

			api_ = RAL::CreateAPI();

			auto resourceSubsystem = context.GetResourceSubsystem();

			const auto vertexShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleVert.spv");
			const auto fragmentShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleFrag.spv");
			using namespace std::chrono_literals;
			OS::LogInfo("Engine/RenderSubsystem/", "Tasks to load resources sent.");
			std::this_thread::sleep_for(1s);
			ResourceSubsystem::Resource vertexShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, vertexShaderTaskId);
			ResourceSubsystem::Resource fragmentShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, fragmentShaderTaskId);

			RAL::Shader vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
			RAL::Shader fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };

			auto uiSubsystem = context.GetUISubsystem();
			auto windowInfo = uiSubsystem->GetWindow()->GetInfo(UI::Render::Vulkan);
			RAL::RenderSurface renderSurface;
			if(windowInfo.subsystem_ == UI::Subsystem::GLFW) {
				
				renderSurface.param1_ = windowInfo.param1_;
				renderSurface.param2_ = windowInfo.param2_;
				renderSurface.param3_ = windowInfo.param3_;
				renderSurface.size_ = windowInfo.size_;
				OS::AssertMessage(renderSurface.size_.GetX() == config->GetValueAs<Common::Size>("UI.Window.Size.StartWidth"),
					"Start size in config and got from ui system are different.");
				OS::AssertMessage(renderSurface.size_.GetY() == config->GetValueAs<Common::Size>("UI.Window.Size.StartHeight"),
					"Start size in config and got from ui system are different.");
				renderSurface.uiSubsystem_ = RAL::UISubsystem::GLFW;
			}

			RAL::Driver::CreateInfo driverCreateInfo{
				vertexShader,
				fragmentShader,
				renderSurface
			};
			driver_ = api_->CreateDriver(driverCreateInfo);
			RAL::Camera::CreateInfo cameraCreateInfo;
			{
				cameraCreateInfo.position_ = Math::Vector3f{ 1.4f, 0.f, 0.f };
				cameraCreateInfo.direction_ = Math::Vector3f{ 0.f, 0.f, 0.f } - cameraCreateInfo.position_;
				cameraCreateInfo.size_ = windowInfo.size_;
			}

			std::shared_ptr<RAL::Camera> camera = api_->CreateCamera(cameraCreateInfo);
			driver_->SetCamera(camera);

			RAL::Light::CreateInfo lightCreateInfo;
			{
				lightCreateInfo.intensity_ = 1.f;
				lightCreateInfo.position_ = { 0.f, 0.f, 0.f };
			}
			std::shared_ptr<RAL::Light> light = api_->CreateLight(lightCreateInfo);
			driver_->AddLight(light);
		}

		virtual void Update() noexcept override {
			Geometry::Box box{ 1 };

			Geometry::VertexCloud<RAL::Vertex3fc> coloredBox;
			for (Common::Index i = 0; i < box.GetVertices().GetVerticesNumber(); i++) {
				Math::Vector3f color{ (float)((i * 100) % 255) / 255, (float)((i * 150) % 255) /255, (float)((i * 199) % 255) / 255 };
				RAL::Vertex3fc coloredVertex = {box.GetVertices()[i], color };
				coloredBox.Add(coloredVertex);
			}

			driver_->DrawIndexed(
				(RAL::Vertex3fc*)coloredBox.GetData(),
				coloredBox.GetVerticesNumber(),
				box.GetIndices().GetData(),
				box.GetIndicesNumber()/*, RAL::Color{ 1.f, 1.f, 1.f }*/);
			driver_->StartRender();
			driver_->Render();
			driver_->EndRender();

		}

	private:
		std::shared_ptr<RAL::API> api_ = nullptr;
		std::shared_ptr<RAL::Driver> driver_ = nullptr;
	};

}
