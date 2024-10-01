
#include <Behaviour/OksEngine.LoadLuaScript.hpp>


#include <Resources/OksEngine.LoadResourceRequest.hpp>

#include <Behaviour/OksEngine.LuaScript.hpp>
#include <Behaviour/OksEngine.LuaScriptLoadRequest.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>
#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {


	void LoadLuaScript::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		Behaviour* behaviour = world->GetComponent<Behaviour>(entityId);
		if (!world->IsComponentExist<LuaScriptLoadRequest>(entityId)) {
			const ECS::Entity::Id resourceEntityId = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(resourceEntityId, behaviour->scriptName_);
			world->CreateComponent<LuaScriptLoadRequest>(entityId, resourceEntityId);
		}
		else {
			auto* request = world->GetComponent<LuaScriptLoadRequest>(entityId);
			if (world->IsComponentExist<Resource>(request->resourceEntityId_)) {
				Resource* resource = world->GetComponent<Resource>(request->resourceEntityId_);
				const Lua::Script::Id scriptId = GetContext().GetScriptStorage()->Add(behaviour->scriptName_, Lua::Script{ resource->resourceData_ });
				world->CreateComponent<LuaScript>(entityId, resource->name_, scriptId);
			}
		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> LoadLuaScript::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Behaviour>().Exclude<LuaScript>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

}