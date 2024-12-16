
#include <Render/Texture/OksEngine.CreateLoadTextureRequest.hpp>

#include <Common/OksEngine.Name.hpp>
#include <Render/Model/ObjMtl/OksEngine.LoadObjRequest.hpp>
#include <Sound/OksEngine.CreateLoadSoundRequest.hpp>
#include <Sound/OksEngine.SoundInfo.hpp>
#include <Sound/OksEngine.LoadSoundRequest.hpp>

#include <Resources/OksEngine.LoadResourceRequest.hpp>

namespace OksEngine {


	void CreateLoadSoundRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* soundInfo = world->GetComponent<SoundInfo>(entityId);

		const ECS::Entity::Id loadSoundResourceEntityId = world->CreateEntity();

		world->CreateComponent<LoadResourceRequest>(loadSoundResourceEntityId);
		world->CreateComponent<Name>(loadSoundResourceEntityId, soundInfo->soundName_);

		world->CreateComponent<LoadSoundRequest>(entityId, loadSoundResourceEntityId);

	}

	[[nodiscard]]
	std::string CreateLoadSoundRequest::GetName() const noexcept {
		return "CreateLoadSoundRequest";
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadSoundRequest::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}
			.Include<SoundInfo>()
			.Exclude<LoadSoundRequest>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateLoadSoundRequest::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLoadSoundRequest>().GetId();
	}

}