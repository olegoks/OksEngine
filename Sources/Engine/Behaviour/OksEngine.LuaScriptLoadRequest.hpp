#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct LuaScriptLoadRequest : public ECSComponent<LuaScriptLoadRequest> {
		ECS::Entity::Id resourceEntityId_ = ECS::Entity::Id::invalid_;

		LuaScriptLoadRequest();

		LuaScriptLoadRequest(ECS::Entity::Id resourceEntityId);

	};


}