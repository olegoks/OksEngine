
#include <Behaviour/OksEngine.LoadLuaScript.hpp>


#include <Resources/OksEngine.LoadResourceRequest.hpp>

#include <Behaviour/OksEngine.LuaScript.hpp>
#include <Behaviour/OksEngine.LuaScriptEntity.hpp>
#include <Behaviour/OksEngine.LoadLuaScriptRequest.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>
#include <Resources/OksEngine.Resource.hpp>
#include <Resources/OksEngine.ResourceEntity.hpp>
#include <Common/OksEngine.Completed.hpp>
#include <Common/OksEngine.Name.hpp>
#include <Common/OksEngine.Text.hpp>
#include <Common/OksEngine.BinaryData.hpp>
#include <Behaviour/OksEngine.LuaEntity.hpp>


namespace OksEngine {


	void CreateLuaScriptEntity::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		Behaviour* behaviour = world->GetComponent<Behaviour>(entityId);
		auto* loadLuaScriptRequest = world->GetComponent<LoadLuaScriptRequest>(entityId);
		const ECS::Entity::Id loadResourceRequestEntityId = loadLuaScriptRequest->loadResourceRequestEntityId_;
		if (world->IsComponentExist<Completed>(loadResourceRequestEntityId)) {
			auto* resourceEntity = world->GetComponent<ResourceEntity>(loadResourceRequestEntityId);
			auto* binaryData = world->GetComponent<BinaryData>(resourceEntity->id_);
			auto* scriptName = world->GetComponent<Name>(loadResourceRequestEntityId);
			const ECS::Entity::Id luaScriptEntityId = world->CreateEntity();
			{
				world->CreateComponent<Name>(luaScriptEntityId, scriptName->value_);
				world->CreateComponent<Text>(luaScriptEntityId, std::string{ binaryData->data_.data(), binaryData->data_.size() });
			}
			world->CreateComponent<LuaScriptEntity>(entityId, luaScriptEntityId);
		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLuaScriptEntity::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<LoadLuaScriptRequest>().Exclude<LuaScript>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

}