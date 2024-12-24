#pragma once

#include <OS.Logger.hpp>
#include <Common/OksEngine.Subsystem.hpp>

namespace OksEngine {

	class [[nodiscard]] LogSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			OS::CommandLineParameters commandLineParameters_;
			Context& context_;
		};

		LogSubsystem(const CreateInfo& createInfo) :
			Subsystem{
				Subsystem::Type::Log,
				createInfo.context_
			} {

			OS::InitializeLogger(
				createInfo.commandLineParameters_.GetArgc(), 
				createInfo.commandLineParameters_.GetArgv());
		}

		void Update() override { }

	private:
	};

}