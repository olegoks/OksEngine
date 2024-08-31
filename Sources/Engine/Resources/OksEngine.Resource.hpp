#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <Resources/OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class Resource : public ECSComponent<Resource> {
	public:
		Resource(const std::string& name, const std::string& resourceData) : 
			ECSComponent{ nullptr },
			name_{ name }, 
			resourceData_{ resourceData } { }

		std::string name_;
		std::string resourceData_;
	};


	template<>
	inline void Edit<Resource>(Resource* resource) {
		ImGui::TextDisabled("Resource name: %s", resource->name_);
	}

}