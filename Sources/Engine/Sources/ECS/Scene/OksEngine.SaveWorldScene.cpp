#include <ECS\Scene\auto_OksEngine.SaveWorldScene.hpp>

#include <OksEngine.Components.hpp>


namespace OksEngine {
void SaveWorldScene::Update(ECS2::Entity::Id entity1Id, const Serializable* serializable,
	ECS2::Entity::Id entity2Id, const ECSController* eCSController,
	const SaveSceneRequest* saveSceneRequest,
	const LuaScript* luaScript) {

	const ECS2::ComponentsFilter components = GetComponentsFilter(entity1Id);
	std::string parsedEntity;
	if (components.IsSet<Position2D>()) {
		const auto* component = GetComponent<Position2D>(entity1Id);
		parsedEntity += SerializePosition2D(component);
	}
	if (components.IsSet<CallGraphNode>()) {
		const auto* component = GetComponent<CallGraphNode>(entity1Id);
		parsedEntity += SerializeCallGraphNode(component);
	}
	if (components.IsSet<Name>()) {
		const auto* component = GetComponent<Name>(entity1Id);
		parsedEntity += SerializeName(component);
	}

};
}