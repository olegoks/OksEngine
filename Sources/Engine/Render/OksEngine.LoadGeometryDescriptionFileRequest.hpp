#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {
	struct LoadGeometryDescriptionFileRequest : public ECSComponent<LoadGeometryDescriptionFileRequest> {
		ECS::Entity::Id resourceTaskEntityId_ = ECS::Entity::Id::invalid_;
		std::string geomFileName_ = "";
		LoadGeometryDescriptionFileRequest() :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ ECS::Entity::Id::invalid_ } {

		}

		LoadGeometryDescriptionFileRequest(const std::string& geometryFileName) :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ ECS::Entity::Id::invalid_ },
			geomFileName_{ geometryFileName } {

		}
	};


	template<>
	inline void Edit<LoadGeometryDescriptionFileRequest>(LoadGeometryDescriptionFileRequest* loadGeometryDescriptionFileRequest) {
		ImGui::TextDisabled("geom file: \"%s\"", loadGeometryDescriptionFileRequest->geomFileName_.c_str());
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadGeometryDescriptionFileRequest->resourceTaskEntityId_);
	}
}