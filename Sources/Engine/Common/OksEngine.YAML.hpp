#pragma once 

#include <OksEngine.ECS.Component.hpp>

#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

namespace OksEngine {

	struct YAML : public ECSComponent<YAML> {
	public:
		YAML() : ECSComponent{ nullptr } { }
	};

}