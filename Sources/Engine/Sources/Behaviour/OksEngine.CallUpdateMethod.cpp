
#include <Behaviour/OksEngine.CallUpdateMethod.hpp>

#include <Behaviour/OksEngine.LuaContext.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>

#pragma optimize("", off)

namespace OksEngine {

	CallUpdateMethod::CallUpdateMethod(Context& context) noexcept :
		ECSSystem{ context } {

	}

	void CallUpdateMethod::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* context = world->GetComponent<LuaContext>(entityId);
		auto* behaviour = world->GetComponent<Behaviour>(entityId);
		const std::string objectName = behaviour->objectName_;
		luabridge::LuaRef updater = context->context_.GetGlobalAsRef(objectName + "Updater");
		luabridge::LuaRef object = context->context_.GetGlobalAsRef("object");
		luabridge::LuaRef updateMethod = updater["Update"];
		const luabridge::LuaResult result = updateMethod(updater, object, 0);
		OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CallUpdateMethod::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<Behaviour>().Include<LuaContext>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CallUpdateMethod::GetTypeId() const noexcept {
		return Common::TypeInfo<CallUpdateMethod>().GetId();
	}


}

#pragma optimize("", on)
