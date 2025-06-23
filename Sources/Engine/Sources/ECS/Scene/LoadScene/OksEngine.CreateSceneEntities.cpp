#include <ECS/Scene/LoadScene\auto_OksEngine.CreateSceneEntities.hpp>


#include <OksEngine.ECS.hpp>

namespace OksEngine {
void CreateSceneEntities::Update(ECS2::Entity::Id entityId,
                                 const ECSController* eCSController,
                                 const LoadSceneRequest* loadSceneRequest,
                                 const LuaScript* luaScript) {
    

	::Lua::Context context;

	::Lua::Script script{ luaScript->text_ };
	context.LoadScript(script);

    luabridge::LuaRef scene = context.GetGlobalAsRef("scene");


    luabridge::LuaRef entities = scene["entities"];

	for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
		ECS2::Entity::Id newEntityId = CreateEntity();
		luabridge::LuaRef entity = it.value();

		{
			luabridge::LuaRef position2DRef = entity["position2D"];
			if (!position2DRef.isNil()) {
				Position2D position2D = ParsePosition2D(position2DRef);
				CreateComponent<Position2D>(newEntityId, position2D.x_, position2D.y_);
			}
		}
		{
			luabridge::LuaRef callGraphNodeRef = entity["callGraphNode"];
			if (!callGraphNodeRef.isNil()) {
				CallGraphNode callGraphNode = ParseCallGraphNode(callGraphNodeRef);
				CreateComponent<CallGraphNode>(newEntityId);
			}
		}
		{
			luabridge::LuaRef nameRef = entity["name"];
			if (!nameRef.isNil()) {
				Name name = ParseName(nameRef);
				CreateComponent<Name>(newEntityId, name.value_);
			}
		}
	}

	RemoveComponent<LoadSceneRequest>(entityId);
	RemoveComponent<LuaScript>(entityId);

};
}