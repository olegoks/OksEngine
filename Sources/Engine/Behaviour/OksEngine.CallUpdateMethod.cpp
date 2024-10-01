
#include <Behaviour/OksEngine.CallUpdateMethod.hpp>

#include <Behaviour/OksEngine.LuaContext.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>

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
		updateMethod(updater, object, 0);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CallUpdateMethod::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Behaviour>().Include<LuaContext>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CallUpdateMethod::GetTypeId() const noexcept {
		return Common::TypeInfo<CallUpdateMethod>().GetId();
	}


}