#include <Resources\auto_OksEngine.CreateResourceSystem.hpp>

namespace OksEngine {
void CreateResource::Update() {


	auto resourceSubsystem = std::make_shared<AsyncResourceSubsystem>();

	const ECS2::Entity::Id resourceSystemEntity = CreateEntity();
	CreateComponent<ResourceSystem>(resourceSystemEntity, resourceSubsystem);


};
}