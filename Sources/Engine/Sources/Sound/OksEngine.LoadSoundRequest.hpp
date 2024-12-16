
#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LoadSoundRequest : public ECSComponent<LoadSoundRequest> {
		ECS::Entity::Id resourceLoadRequestEntityId_ = ECS::Entity::Id::invalid_;
		LoadSoundRequest() :
			ECSComponent{ nullptr },
			resourceLoadRequestEntityId_{ ECS::Entity::Id::invalid_ } {
		}


		LoadSoundRequest(ECS::Entity::Id loadResourceEntity) :
			ECSComponent{ nullptr },
			resourceLoadRequestEntityId_{ loadResourceEntity } {

		}
	};


	template<>
	inline void Edit<LoadSoundRequest>(LoadSoundRequest* loadTextureRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadTextureRequest->resourceLoadRequestEntityId_);
	}

}