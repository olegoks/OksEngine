
#include <Behaviour/OksEngine.LoadLuaScriptRequest.hpp>

namespace OksEngine {

	LoadLuaScriptRequest::LoadLuaScriptRequest() :
		ECSComponent{ nullptr },
		loadResourceRequestEntityId_{ ECS::Entity::Id::invalid_ } {

	}

	LoadLuaScriptRequest::LoadLuaScriptRequest(ECS::Entity::Id loadResourceRequestEntityId) :
		ECSComponent{ nullptr },
		loadResourceRequestEntityId_{ loadResourceRequestEntityId } {

	}

}