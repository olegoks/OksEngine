#pragma once 

#include <OksEngine.Subsystem.hpp>
#include <RAL.hpp>

namespace OksEngine {

	class RenderSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			RAL::Shader vertexShader_;
			RAL::Shader fragmentShader_;
			RAL::RenderSurface surface;
		};

		RenderSubsystem(const CreateInfo& createInfo) {
			
			api_ = RAL::CreateAPI();
			
			RAL::Driver::CreateInfo driverCreateInfo{
				createInfo.vertexShader_,
				createInfo.fragmentShader_,
				createInfo.surface
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