#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct Name : public ECSComponent<Name> {
		Name() = default;
		Name(const std::string& value) :
			ECSComponent{ nullptr },
			value_{ value } {
		
		}
		std::string value_;
	};

	template<>
	inline void Edit<Name>(Name* name) {
		ImGui::TextDisabled(name->value_.c_str());
	}

}