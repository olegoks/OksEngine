#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class DebugInfo : public ECSComponent<DebugInfo>{
	public:

		DebugInfo(std::string name) : ECSComponent{ nullptr }, name_ { name } { }

		std::string name_;


	};

}