
#include <Sound/auto_OksEngine.UpdateSoundState.hpp>

namespace OksEngine {


	void UpdateSoundState::Update(
		ECS2::Entity::Id entityId,
		const Sound* sound) {

	}

	//[[nodiscard]]
	//std::string UpdateSoundState::GetName() const noexcept {
	//	return "UpdateSoundState";
	//}

	//std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateSoundState::GetFilter() const noexcept {
	//	static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
	//		.Include<Sound>()
	//		//.Exclude<LoadSoundRequest>(),
	//		,
	//		ECS::Entity::Filter{}.ExcludeAll() };
	//	return filter;
	//}

	//Common::TypeId UpdateSoundState::GetTypeId() const noexcept {
	//	return Common::TypeInfo<UpdateSoundState>().GetId();
	//}

}