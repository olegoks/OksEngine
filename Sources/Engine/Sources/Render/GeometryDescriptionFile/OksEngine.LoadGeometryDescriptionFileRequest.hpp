#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {
	struct LoadGeometryDescriptionFileRequest : public ECSComponent<LoadGeometryDescriptionFileRequest> {
		ECS::Entity::Id loadResourceRequestEntityId_ = ECS::Entity::Id::invalid_;
		LoadGeometryDescriptionFileRequest() :
			ECSComponent{ nullptr },
			loadResourceRequestEntityId_{ ECS::Entity::Id::invalid_ } { }

		

		LoadGeometryDescriptionFileRequest(ECS::Entity::Id loadResourceEntity) :
			ECSComponent{ nullptr },
			loadResourceRequestEntityId_{ loadResourceEntity } {

		}
	};


	template<>
	inline void Edit<LoadGeometryDescriptionFileRequest>(LoadGeometryDescriptionFileRequest* loadGeometryDescriptionFileRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadGeometryDescriptionFileRequest->loadResourceRequestEntityId_);
	}
}