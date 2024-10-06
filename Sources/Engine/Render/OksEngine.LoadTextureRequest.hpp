
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LoadTextureRequest : public ECSComponent<LoadTextureRequest> {
		ECS::Entity::Id resourceTaskEntityId_ = ECS::Entity::Id::invalid_;
		LoadTextureRequest() :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ ECS::Entity::Id::invalid_ } {
		}


		LoadTextureRequest(ECS::Entity::Id loadResourceEntity) :
			ECSComponent{ nullptr },
			resourceTaskEntityId_{ loadResourceEntity } {

		}
	};


	template<>
	inline void Edit<LoadTextureRequest>(LoadTextureRequest* loadTextureRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadTextureRequest->resourceTaskEntityId_);
	}

}