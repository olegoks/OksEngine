
#include <Behaviour/auto_OksEngine.CallUpdateMethod.hpp>


namespace OksEngine {



	void CallUpdateMethod::Update(
		ECS2::Entity::Id entityId,
		const Behaviour* behaviour,
		LuaContext* luaContext) {

		const std::string objectName = behaviour->objectName_;
		luabridge::LuaRef updater = luaContext->context_.GetGlobalAsRef(objectName + "Updater");
		luabridge::LuaRef object = luaContext->context_.GetGlobalAsRef("object");
		luabridge::LuaRef updateMethod = updater["Update"];
		const luabridge::LuaResult result = updateMethod(updater, object, 0);
		OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
	}


}

