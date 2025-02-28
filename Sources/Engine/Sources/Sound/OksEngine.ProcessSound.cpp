#include <Sound/auto_OksEngine.ProcessSound.hpp>

namespace OksEngine {


	void ProcessSound::Update(
		ECS2::Entity::Id entityId,
		const Sound* sound,
		const StartPlaySound* StartPlaySound) {

		//auto* sound = world->GetComponent<Sound>(entityId);
		//sound->sound_.setBuffer(sound->soundBuffer_);
		//sound->sound_.play();
		//world->CreateComponent<SoundInProgress>(entityId);

	}


	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> ProcessSound::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter =
	//	{ ECS::Entity::Filter{}
	//	.Include<Sound>()
	//	.Include<StartPlaySound>()
	//	.Exclude<SoundInProgress>(), ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

	//Common::TypeId ProcessSound::GetTypeId() const noexcept {
	//	return Common::TypeInfo<ProcessSound>().GetId();
	//}

}