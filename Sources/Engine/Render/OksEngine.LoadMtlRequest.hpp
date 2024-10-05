#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	class LoadMtlRequest : public ECSComponent<LoadMtlRequest> {
	public:

		LoadMtlRequest() noexcept : ECSComponent{ nullptr } { }

		LoadMtlRequest(ECS::Entity::Id resourceTaskEntityId) noexcept : ECSComponent{ nullptr }, resourceTaskEntityId_{ resourceTaskEntityId } { }

		ECS::Entity::Id resourceTaskEntityId_;

	};


	template<>
	inline void Edit<LoadMtlRequest>(LoadMtlRequest* loadObjRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadObjRequest->resourceTaskEntityId_);
	}

}