
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct ResourceName : public ECSComponent<ResourceName> {
		ECS::Entity::Id resourceTaskEntityId_ = ECS::Entity::Id::invalid_;
		ResourceName() :
			ECSComponent{ nullptr } {
		}


		ResourceName(const std::string& string) :
			ECSComponent{ nullptr },
			string_{ string } {

		}

		const std::string string_;
	};


	template<>
	inline void Edit<ResourceName>(ResourceName* resourceName) {
		ImGui::TextDisabled("Name: %s", resourceName->string_);
	}

}