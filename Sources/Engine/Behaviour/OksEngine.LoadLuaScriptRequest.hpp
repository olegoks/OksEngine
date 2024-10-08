#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct LoadLuaScriptRequest : public ECSComponent<LoadLuaScriptRequest> {
		ECS::Entity::Id loadResourceRequestEntityId_ = ECS::Entity::Id::invalid_;

		LoadLuaScriptRequest();

		LoadLuaScriptRequest(ECS::Entity::Id loadResourceRequestEntityId);

	};


}