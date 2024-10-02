#pragma once 

#include <OksEngine.ECS.Component.hpp>

#define YAML_CPP_STATIC_DEFINE
#define YAML_CPP_API
#include <yaml-cpp/yaml.h>

namespace OksEngine {

	struct GeometryFile : public ECSComponent<GeometryFile> {
	public:
		enum class Type {
			OBJ,
			FBX,
			Undefined
		};

		GeometryFile() : ECSComponent{ nullptr } {

		}
		GeometryFile(const std::string geomName, const std::string yamlText) :
			ECSComponent{ nullptr },
			geomName_{ geomName },
			yamlText_{ yamlText },
			mesh_{  } {
			mesh_ = YAML::Load(yamlText_);
		}

		std::string geomName_;
		std::string yamlText_;
		YAML::Node mesh_;

	};

	template<>
	inline void Edit<GeometryFile>(GeometryFile* geometryFile) {
		ImGui::TextDisabled(".geom file name: \"%s\"", geometryFile->geomName_.c_str());
		ImGui::TextDisabled("%s", geometryFile->yamlText_.c_str());
	}

}