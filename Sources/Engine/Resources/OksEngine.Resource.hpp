#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <Resources/OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class Resource : public ECSComponent<Resource> {
	public:
		Resource(const std::string& resourceData) : 
			ECSComponent{ nullptr },
			resourceData_{ resourceData } { }

		std::string resourceData_;
	};


	template<>
	inline void Edit<Resource>(Resource* resource) {
	}

}