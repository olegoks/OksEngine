#pragma once 

#include <Common/OksEngine.Subsystem.hpp>
#include <Debug/OksEngine.Debug.Components.hpp>

namespace OksEngine {

	class DebugSubsystem : public Subsystem {
	public:

		struct CreateInfo {
			Context& context_;
		};
		
		DebugSubsystem(const CreateInfo& createInfo);

		virtual void Update() override { }

	};

}