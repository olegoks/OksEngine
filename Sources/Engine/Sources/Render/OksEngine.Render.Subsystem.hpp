#pragma once 

#include <Common/OksEngine.Subsystem.hpp>
#include <RE.RenderEngine.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class RenderSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
		};

		RenderSubsystem(const CreateInfo& createInfo);
		virtual void Update() noexcept override;

		std::shared_ptr<RAL::Driver> GetDriver() noexcept {
			return engine_->GetDriver();
		}
	private:


		std::shared_ptr<RE::RenderEngine> engine_ = nullptr;
	};

}
