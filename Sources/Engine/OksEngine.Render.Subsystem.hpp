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

			auto resourceSubsystem = context.GetResourceSubsystem();

			const auto vertexShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleVert.spv");
			const auto fragmentShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangleFrag.spv");

			auto ecsWorld = context.GetECSWorld();
			ecsWorld->RegisterSystem<RenderSystem>();

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

			api_ = RAL::CreateAPI();
			RAL::Camera::CreateInfo cameraCreateInfo;
			{
				cameraCreateInfo.position_ = Math::Vector3f{ 2.5f, 0.f, 0.f };
				cameraCreateInfo.direction_ = Math::Vector3f{ 0.f, 0.f, 0.f } - cameraCreateInfo.position_;
				cameraCreateInfo.size_ = windowInfo.size_;
			}

			std::shared_ptr<RAL::Camera> camera = api_->CreateCamera(cameraCreateInfo);

			RAL::Light::CreateInfo lightCreateInfo;
			{
				lightCreateInfo.intensity_ = 1.f;
				lightCreateInfo.position_ = { 0.f, 0.f, 0.f };
			}
			std::shared_ptr<RAL::Light> light = api_->CreateLight(lightCreateInfo);

			ResourceSubsystem::Resource vertexShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, vertexShaderTaskId);
			ResourceSubsystem::Resource fragmentShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, fragmentShaderTaskId);

			RAL::Shader vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
			RAL::Shader fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };

			RAL::Driver::CreateInfo driverCreateInfo{
				vertexShader,
				fragmentShader,
				renderSurface,
				true
			};

			driver_ = api_->CreateDriver(driverCreateInfo);
			driver_->SetCamera(camera);
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

			DS::Vector<Geometry::VertexCloud<RAL::Vertex3fc>> plane;

			for (int i = 0; i < 25; i++) {
				plane.PushBack(coloredBox);
			}


			Common::Index i = 0;
			for (int x = -2; x < 2; x++) {
				for (int y = -2; y < 2; y++) {
					Geometry::VertexCloud<RAL::Vertex3fc>& box = plane[i];
					Math::Vector3f offsetVector{ (float)x, (float)y , 0 };
					const Math::Matrix4x4f offset = Math::Matrix4x4f::GetTranslate(offsetVector);
					for (RAL::Vertex3fc& vertex : box) {
						vertex.position_ = Math::TransformPoint(vertex.position_, offset);
					}
					++i;
				}
			}

			for (int i = 0; i < 25; i++) {
				Geometry::VertexCloud<RAL::Vertex3fc>& coloredBox = plane[i];
				driver_->DrawIndexed(
					(RAL::Vertex3fc*)coloredBox.GetData(),
					coloredBox.GetVerticesNumber(),
					box.GetIndices().GetData(),
					box.GetIndicesNumber()/*, RAL::Color{ 1.f, 1.f, 1.f }*/);
			}
			
			

			//driver_->DrawIndexed(
			//	(RAL::Vertex3fc*)coloredBox.GetData(),
			//	coloredBox.GetVerticesNumber(),
			//	box.GetIndices().GetData(),
			//	box.GetIndicesNumber()/*, RAL::Color{ 1.f, 1.f, 1.f }*/);

			driver_->StartRender();
			driver_->Render();
			driver_->EndRender();

		}

	private:
		std::shared_ptr<RAL::API> api_ = nullptr;
		std::shared_ptr<RAL::Driver> driver_ = nullptr;
	};

}
