#include <Sound/OksEngine.CreateSound.hpp>

#include <Sound/OksEngine.LoadSoundRequest.hpp>
#include <Sound/OksEngine.Sound.hpp>
#include <Resources/OksEngine.ResourceEntity.hpp>

#include <Common/auto_OksEngine.BinaryData.hpp>

#include <SFML/Audio.hpp>

namespace OksEngine {


	void CreateSound::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* loadSoundRequest = world->GetComponent<LoadSoundRequest>(entityId);
		if (world->IsComponentExist<ResourceEntity>(loadSoundRequest->resourceLoadRequestEntityId_)) {
			auto* resourceEntity = world->GetComponent<ResourceEntity>(loadSoundRequest->resourceLoadRequestEntityId_);
			auto* binaryData = world->GetComponent<BinaryData>(resourceEntity->id_);

			

			sf::SoundBuffer buffer;
			const bool success = buffer.loadFromMemory(binaryData->data_.data(), binaryData->data_.size());
			OS::AssertMessage(success, "");
			world->CreateComponent<Sound>(entityId, buffer);

			
		}

	}

	[[nodiscard]]
	std::string CreateSound::GetName() const noexcept {
		return "CreateSound";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateSound::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
		{ ECS::Entity::Filter{}
		.Include<LoadSoundRequest>()
		.Exclude<Sound>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateSound::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateSound>().GetId();
	}

}