
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LoadObjRequest : public ECSComponent<LoadObjRequest> {
		ECS::Entity::Id resourceTaskEntityId_ = ECS::Entity::Id::invalid_;
		LoadObjRequest() :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ ECS::Entity::Id::invalid_ } {
		}


		LoadObjRequest(ECS::Entity::Id loadResourceEntity) :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ loadResourceEntity } {

		}
	};


	template<>
	inline void Edit<LoadObjRequest>(LoadObjRequest* loadObjRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadObjRequest->resourceTaskEntityId_);
	}

}