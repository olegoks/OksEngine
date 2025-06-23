#include <ECS\Scene\SaveScene\CreateSceneFile/auto_OksEngine.CreateSceneFile.hpp>

#include <OksEngine.ECS.hpp>

#include <luabridge3/LuaBridge/LuaBridge.h>

namespace OksEngine {
	void CreateSceneFile::Update(ECS2::Entity::Id entity1Id, const Serializable* serializable,
		ECS2::Entity::Id entity2Id, const ECSController* eCSController,
		const SaveSceneRequest* saveSceneRequest,
		LuaScript* luaScript) {

		const ECS2::ComponentsFilter components = GetComponentsFilter(entity1Id);
		std::string parsedEntity;
		luaScript->text_ += parsedEntity + "{\n";
		if (components.IsSet<Position2D>()) {
			const auto* component = GetComponent<Position2D>(entity1Id);
			parsedEntity += SerializePosition2D(component);
			parsedEntity += ",\n";
		}
		if (components.IsSet<CallGraphNode>()) {
			const auto* component = GetComponent<CallGraphNode>(entity1Id);
			parsedEntity += SerializeCallGraphNode(component);
			parsedEntity += ",\n";
		}
		if (components.IsSet<Name>()) {
			const auto* component = GetComponent<Name>(entity1Id);
			parsedEntity += SerializeName(component);
		}

		luaScript->text_ += parsedEntity + "},\n";


	};
}