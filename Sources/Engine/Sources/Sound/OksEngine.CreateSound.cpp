#include <Sound/auto_OksEngine.CreateSound.hpp>

#include <SFML/Audio.hpp>

namespace OksEngine {


	void CreateSound::Update(
		ECS2::Entity::Id entityId,
		const LoadSoundRequest* loadSoundRequest) {

		//auto* loadSoundRequest = world->GetComponent<LoadSoundRequest>(entityId);
		//if (world->IsComponentExist<ResourceEntity>(loadSoundRequest->resourceLoadRequestEntityId_)) {
		//	auto* resourceEntity = world->GetComponent<ResourceEntity>(loadSoundRequest->resourceLoadRequestEntityId_);
		//	auto* binaryData = world->GetComponent<BinaryData>(resourceEntity->id_);

		//	

		//	sf::SoundBuffer buffer;
		//	const bool success = buffer.loadFromMemory(binaryData->data_.data(), binaryData->data_.size());
		//	ASSERT_FMSG(success, "");
		//	world->CreateComponent<Sound>(entityId, buffer);

		//	
		//}

	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateSound::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
	//	{ ECS::Entity::Filter{}
	//	.Include<LoadSoundRequest>()
	//	.Exclude<Sound>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

}