#include <Resources\auto_OksEngine.CreateResourceSystem.hpp>

namespace OksEngine {
void CreateResourceSystem::Update() {


	auto resourceSubsystem = std::make_shared<AsyncResourceSubsystem>();

	const ECS2::Entity::Id resourceSystemEntity = CreateEntity();
	CreateComponent<ResourceSystem>(resourceSystemEntity, resourceSubsystem);


};
}