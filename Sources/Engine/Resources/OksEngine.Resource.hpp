#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <Resources/OksEngine.Resource.Subsystem.hpp>

namespace OksEngine {

	class Resource : public ECSComponent<Resource> {
	public:
		Resource(const std::string& name, ResourceSubsystem::Resource&& resource) : 
			ECSComponent{ nullptr },
			name_{ name }, 
			resource_ { std::move(resource) } { }

		std::string name_;
		ResourceSubsystem::Resource resource_;
	};

}