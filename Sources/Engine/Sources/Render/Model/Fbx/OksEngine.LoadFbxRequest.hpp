
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LoadFbxRequest : public ECSComponent<LoadFbxRequest> {
		ECS::Entity::Id loadResourceRequestEntityId_ = ECS::Entity::Id::invalid_;
		LoadFbxRequest() :
			ECSComponent{ nullptr },
			loadResourceRequestEntityId_{ ECS::Entity::Id::invalid_ } {
		}


		LoadFbxRequest(ECS::Entity::Id loadResourceRequestEntityId) :
			ECSComponent{ nullptr },
			loadResourceRequestEntityId_{ loadResourceRequestEntityId } {

		}
	};


	template<>
	inline void Edit<LoadFbxRequest>(LoadFbxRequest* loadFbxRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadFbxRequest->loadResourceRequestEntityId_);
	}

}