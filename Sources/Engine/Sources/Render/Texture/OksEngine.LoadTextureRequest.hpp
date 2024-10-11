
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LoadTextureRequest : public ECSComponent<LoadTextureRequest> {
		ECS::Entity::Id resourceLoadRequestEntityId_ = ECS::Entity::Id::invalid_;
		LoadTextureRequest() :
			ECSComponent{ nullptr },
			resourceLoadRequestEntityId_{ ECS::Entity::Id::invalid_ } {
		}


		LoadTextureRequest(ECS::Entity::Id loadResourceEntity) :
			ECSComponent{ nullptr },
			resourceLoadRequestEntityId_{ loadResourceEntity } {

		}
	};


	template<>
	inline void Edit<LoadTextureRequest>(LoadTextureRequest* loadTextureRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadTextureRequest->resourceLoadRequestEntityId_);
	}

}