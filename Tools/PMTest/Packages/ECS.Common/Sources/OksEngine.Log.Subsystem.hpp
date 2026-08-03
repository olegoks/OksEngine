#pragma once

#include <OS.Logger.hpp>
#include <Common/OksEngine.Subsystem.hpp>

namespace OksEngine {

	class [[nodiscard]] LogSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			int argc_ = 0;
			char** argv_ = nullptr;
			Context& context_;
		};

		LogSubsystem(const CreateInfo& createInfo) :
			Subsystem{
				Subsystem::Type::Log,
				createInfo.context_
			} {

			OS::InitializeLogger(
				createInfo.argc_, 
				createInfo.argv_);
		}

		void Update() override { }

	private:
	};

}