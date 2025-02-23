
#include <Behaviour/auto_OksEngine.CreateLuaScriptEntity.hpp>

//#include <Behaviour/auto_OksEngine.LuaScript.hpp>
//#include <Behaviour/OksEngine.LuaScriptEntity.hpp>
//#include <Behaviour/OksEngine.LoadLuaScriptRequest.hpp>
//
//#include <Resources/OksEngine.Resource.hpp>
//#include <Resources/OksEngine.ResourceEntity.hpp>
//#include <Common/auto_OksEngine.Completed.hpp>
//#include <Common/auto_OksEngine.Name.hpp>
//#include <Common/auto_OksEngine.Text.hpp>
//#include <Common/auto_OksEngine.BinaryData.hpp>
//#include <Behaviour/OksEngine.LuaEntity.hpp>


namespace OksEngine {


	void CreateLuaScriptEntity::Update(
		ECS2::Entity::Id entityId,
		const LoadLuaScriptRequest* loadLuaScriptRequest) {
		const ECS2::Entity::Id loadResourceRequestEntityId = loadLuaScriptRequest->loadResourceRequestEntityId_;
		/*if (world->IsComponentExist<Completed>(loadResourceRequestEntityId)) {
			auto* resourceEntity = world->GetComponent<ResourceEntity>(loadResourceRequestEntityId);
			auto* binaryData = world->GetComponent<BinaryData>(resourceEntity->id_);
			auto* scriptName = world->GetComponent<Name>(loadResourceRequestEntityId);
			const ECS::Entity::Id luaScriptEntityId = world->CreateEntity();
			{
				world->CreateComponent<LuaScript>(luaScriptEntityId);
				world->CreateComponent<Name>(luaScriptEntityId, scriptName->value_);
				world->CreateComponent<Text>(luaScriptEntityId, std::string{ binaryData->data_.data(), binaryData->data_.size() });
			}
			world->CreateComponent<LuaScriptEntity>(entityId, luaScriptEntityId);
		}*/
	}


}