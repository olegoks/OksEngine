#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct LuaScriptEntity : public ECSComponent<LuaScriptEntity> {
		LuaScriptEntity(ECS::Entity::Id scriptEntityId) : ECSComponent{ nullptr }, id_{ scriptEntityId } {

		}

		ECS::Entity::Id id_;

	};


	template<>
	inline void Edit<LuaScriptEntity>(LuaScriptEntity* luaScriptEntity) {
		ImGui::TextDisabled("Id: %d", luaScriptEntity->id_);
	}

}