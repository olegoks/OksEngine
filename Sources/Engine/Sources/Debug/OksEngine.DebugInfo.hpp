#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class DebugInfo : public ECSComponent<DebugInfo>{
	public:

		DebugInfo(Context* context, std::string name) : ECSComponent{ context }, name_ { name } { }

		std::string name_;


	};

	template<>
	inline void Edit(DebugInfo* debugInfo) {
		ImGui::TextDisabled("Name: %s", debugInfo->GetName());
	}

}