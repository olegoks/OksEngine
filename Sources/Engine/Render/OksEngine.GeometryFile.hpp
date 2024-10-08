#pragma once 

#include <OksEngine.ECS.Component.hpp>


namespace OksEngine {

	struct GeometryDescriptionFile : public ECSComponent<GeometryDescriptionFile> {
	public:
		GeometryDescriptionFile() : ECSComponent{ nullptr } { }
	};

}