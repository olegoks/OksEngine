#include <Sound/auto_OksEngine.ProcessSound.hpp>

namespace OksEngine {


	void ProcessSound::Update(Sound* sound, PlaySound* playSound) {

		//auto* sound = world->GetComponent<Sound>(entityId);
		//sound->sound_.setBuffer(sound->soundBuffer_);
		//sound->sound_.play();
		//world->CreateComponent<SoundInProgress>(entityId);

	}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ProcessSound::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
	//	{ ECS::Entity::Filter{}
	//	.Include<Sound>()
	//	.Include<PlaySound>()
	//	.Exclude<SoundInProgress>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

	//Common::TypeId ProcessSound::GetTypeId() const noexcept {
	//	return Common::TypeInfo<ProcessSound>().GetId();
	//}

}