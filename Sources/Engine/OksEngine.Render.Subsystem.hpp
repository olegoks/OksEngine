#pragma once 

#include <OksEngine.Subsystem.hpp>
#include <RAL.hpp>

#include <OksEngine.Resource.Subsystem.hpp>

#include "OksEngine.UI.Subsystem.hpp"

namespace OksEngine {

	class RenderSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
			//RAL::Shader vertexShader_;
			//RAL::Shader fragmentShader_;
			//RAL::RenderSurface surface;
		};

		RenderSubsystem(const CreateInfo& createInfo) : Subsystem{ Subsystem::Type::Render, createInfo.context_ } {

			api_ = RAL::CreateAPI();

			auto resourceSubsystem = GetResourceSubsystem();

			const auto vertexShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangle.vert");
			const auto fragmentShaderTaskId = resourceSubsystem->GetResource(Subsystem::Type::Render, "Root/triangle.frag");
			using namespace std::chrono_literals;
			OS::LogInfo("Engine/RenderSubsystem/", "Tasks to load resources sent.");
			std::this_thread::sleep_for(1s);
			ResourceSubsystem::Resource vertexShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, vertexShaderTaskId);
			ResourceSubsystem::Resource fragmentShaderResource = resourceSubsystem->GetResource(Subsystem::Type::Render, fragmentShaderTaskId);

			RAL::Shader vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
			RAL::Shader fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };

			auto uiSubsystem = GetUISubsystem();
			GLFWwindow* glfwWindow = uiSubsystem->GetWindow()->GetGLFWwindow();

			RAL::RenderSurface renderSurface;
			renderSurface.param1_ = glfwWindow;
			renderSurface.uiSubsystem_ = RAL::UISubsystem::GLFW;

			RAL::Driver::CreateInfo driverCreateInfo{
				vertexShader,
				fragmentShader,
				renderSurface
			};
			driver_ = api_->CreateDriver(driverCreateInfo);
			
		}

		virtual void Update() noexcept override {

		}

	private:
		std::shared_ptr<RAL::API> api_ = nullptr;
		std::shared_ptr<RAL::Driver> driver_ = nullptr;
	};

}
