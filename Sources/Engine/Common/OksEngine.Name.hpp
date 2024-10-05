#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct Name : public ECSComponent<Name> {
		Name() = default;
		Name(const std::string& value) :
			ECSComponent{ nullptr },
			value_{} {
			
			OS::AssertMessage(value.size() < sizeof(value_), "Name length is too big.");

			std::memcpy(value_, value.data(), value.length());
		
		}
		char value_[64]{ 0 };
	};

	template<>
	inline void Edit<Name>(Name* name) {
		ImGui::TextDisabled(name->value_);
	}

}