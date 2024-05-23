#pragma once 

#include <OksEngine.Subsystem.hpp>
#include <RAL.hpp>

#include <OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class RenderSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
			//RAL::Shader vertexShader_;
			//RAL::Shader fragmentShader_;
			//RAL::RenderSurface surface;
		};

		RenderSubsystem(const CreateInfo& createInfo) : Subsystem{ createInfo.context_ } {
			
			api_ = RAL::CreateAPI();

			auto resourceSubsystem = GetResourceSubsystem();

			resourceSubsystem->GetResource("Root/triangle.vert");

			//auto vertexShaderResource = 
			//auto fragmentShaderResource = resourceSubsystem->GetResource("Root/triangle.frag");
			//RAL::Shader vertexShader{ vertexShaderResource.GetData<Common::Byte>(), vertexShaderResource.GetSize() };
			//RAL::Shader fragmentShader{ fragmentShaderResource.GetData<Common::Byte>(), fragmentShaderResource.GetSize() };

			//RAL::Driver::CreateInfo driverCreateInfo{
			//	vertexShader,
			//	fragmentShader,
			//	/*surface*/
			//};
			//driver_ = api_->CreateDriver(driverCreateInfo);
		}

		virtual void Update() noexcept override {

		}

	private:
		std::shared_ptr<RAL::API> api_ = nullptr;
		std::shared_ptr<RAL::Driver> driver_ = nullptr;
	};

}