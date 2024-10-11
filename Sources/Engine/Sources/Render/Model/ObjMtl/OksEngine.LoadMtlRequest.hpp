#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	class LoadMtlRequest : public ECSComponent<LoadMtlRequest> {
	public:

		LoadMtlRequest() noexcept : ECSComponent{ nullptr } { }

		LoadMtlRequest(ECS::Entity::Id resourceTaskEntityId) noexcept : ECSComponent{ nullptr }, loadResourceRequestEntityId_{ resourceTaskEntityId } { }

		ECS::Entity::Id loadResourceRequestEntityId_;

	};


	template<>
	inline void Edit<LoadMtlRequest>(LoadMtlRequest* loadObjRequest) {
		ImGui::TextDisabled("Task entity ID: \"%d\"", loadObjRequest->loadResourceRequestEntityId_);
	}

}