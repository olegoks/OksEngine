#include <Sound/OksEngine.ProcessSound.hpp>

#include <Sound/OksEngine.LoadSoundRequest.hpp>
#include <Sound/OksEngine.Sound.hpp>
#include <Sound/OksEngine.PlaySound.hpp>
#include <Sound/OksEngine.SoundInProgress.hpp>

namespace OksEngine {


	void ProcessSound::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* sound = world->GetComponent<Sound>(entityId);
		sound->sound_.setBuffer(sound->soundBuffer_);
		sound->sound_.play();
		world->CreateComponent<SoundInProgress>(entityId);

	}

	[[nodiscard]]
	std::string ProcessSound::GetName() const noexcept {
		return "ProcessSound";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ProcessSound::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
		{ ECS::Entity::Filter{}
		.Include<Sound>()
		.Include<PlaySound>()
		.Exclude<SoundInProgress>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId ProcessSound::GetTypeId() const noexcept {
		return Common::TypeInfo<ProcessSound>().GetId();
	}

}