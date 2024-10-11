#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <Resources/OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class Resource : public ECSComponent<Resource> {
	public:
		Resource() : 
			ECSComponent{ nullptr } { }

	};


	template<>
	inline void Edit<Resource>(Resource* resource) {
	}

}