
#include <Sound/auto_OksEngine.CreateLoadSoundRequest.hpp>

namespace OksEngine {


	void CreateLoadSoundRequest::Update(
		ECS2::Entity::Id entityId, 
		const SoundInfo* soundInfo) {

		//auto* soundInfo = world->GetComponent<SoundInfo>(entityId);

		//const ECS::Entity::Id loadSoundResourceEntityId = world->CreateEntity();

		//world->CreateComponent<LoadResourceRequest>(loadSoundResourceEntityId);
		//world->CreateComponent<Name>(loadSoundResourceEntityId, soundInfo->soundName_);

		//world->CreateComponent<LoadSoundRequest>(entityId, loadSoundResourceEntityId);

	}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadSoundRequest::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<SoundInfo>()
	//		.Exclude<LoadSoundRequest>(),
	//		ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}


}