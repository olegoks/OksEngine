
#include <Behaviour/OksEngine.LuaEntity.hpp>


namespace OksEngine {


	void BindEntityId(::Lua::Context& context) {

		context.GetGlobalNamespace()
			.beginClass<EntityId>("EntityId")
			.addConstructor<void(*)()>()
			.addFunction("SetValue", &EntityId::SetId)
			.addFunction("GetIdValue", &EntityId::GetId)
			.endClass();
	}

	void EntityId::SetId(std::uint64_t id) { 
		id_ = id;
	}

	std::uint64_t EntityId::GetId() {
		return	id_;
	}
};
