
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LoadObjRequest : public ECSComponent<LoadObjRequest> {
		ECS::Entity::Id loadResourceRequestEntityId_ = ECS::Entity::Id::invalid_;
		LoadObjRequest() :
			ECSComponent{ nullptr },
			loadResourceRequestEntityId_{ ECS::Entity::Id::invalid_ } {
		}


		LoadObjRequest(ECS::Entity::Id loadResourceRequestEntityId) :
			ECSComponent{ nullptr },
			loadResourceRequestEntityId_{ loadResourceRequestEntityId } {

		}
	};


	template<>
	inline void Edit<LoadObjRequest>(LoadObjRequest* loadObjRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadObjRequest->loadResourceRequestEntityId_);
	}

}