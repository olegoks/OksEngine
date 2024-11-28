
#include <Behaviour/OksEngine.CreateLoadLuaScriptRequest.hpp>

#include <Common/OksEngine.Name.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>
#include <Resources/OksEngine.LoadResourceRequest.hpp>
#include <Behaviour/OksEngine.LoadLuaScriptRequest.hpp>

namespace OksEngine {


	void CreateLoadLuaScriptRequest::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		Behaviour* behaviour = world->GetComponent<Behaviour>(entityId);

		const ECS::Entity::Id loadResourceRequestEntity = world->CreateEntity();
		{
			world->CreateComponent<LoadResourceRequest>(loadResourceRequestEntity);
			world->CreateComponent<Name>(loadResourceRequestEntity, behaviour->scriptName_);
		}
		world->CreateComponent<LoadLuaScriptRequest>(entityId, loadResourceRequestEntity);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLoadLuaScriptRequest::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Behaviour>()
			.Exclude<LoadLuaScriptRequest>(),
			ECS::Entity::Filter{}
			.ExcludeAll() };
		return filter;
	}

}