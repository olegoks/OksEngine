
#include <Behaviour/OksEngine.LuaScriptLoadRequest.hpp>

namespace OksEngine {

	LuaScriptLoadRequest::LuaScriptLoadRequest() :
		ECSComponent{ nullptr },
		resourceEntityId_{ ECS::Entity::Id::invalid_ } {

	}

	LuaScriptLoadRequest::LuaScriptLoadRequest(ECS::Entity::Id resourceEntityId) :
		ECSComponent{ nullptr },
		resourceEntityId_{ resourceEntityId } {

	}

}