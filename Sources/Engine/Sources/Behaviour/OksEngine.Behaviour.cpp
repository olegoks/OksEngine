#pragma once
#include <Behaviour\auto_OksEngine.Behaviour.hpp>

#include <Behaviour\OksEngine.LuaEntity.hpp>


#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Direction3D.hpp>

namespace OksEngine
{
	namespace Behaviour
	{
		void CreateLuaScript::Update(
			ECS2::Entity::Id entity0id, ResourceSystem* resourceSystem0, 
			ECS2::Entity::Id entity1id, const ScriptName* scriptName1) {
			
			Resources::ResourceData scriptResourceData = resourceSystem0->system_->GetResourceSynch(Subsystem::Type::ChildThread, "Root/" + scriptName1->name_);
			Resources::ResourceData entityResourceData = resourceSystem0->system_->GetResourceSynch(Subsystem::Type::ChildThread, "Root/Entity.lua");
			CreateComponent<LuaScript>(entity1id, 
				std::string{
					entityResourceData.GetData<char>(),
					entityResourceData.GetSize()
				} +
				std::string{
					scriptResourceData.GetData<char>(),
					scriptResourceData.GetSize() });

		};

		void CreateLuaContext::Update(
			ECS2::Entity::Id entity0id,
			const ScriptName* scriptName0,
			const ObjectName* objectName0,
			const LuaScript* luaScript0) {

			auto luaContext = std::make_shared<::Lua::Context>();
			BindLuaEntity(*luaContext);
			BindWorldPosition3D(*luaContext);
			BindDirection3D(*luaContext);
			luaContext->LoadScript(luaScript0->text_);
			CreateComponent<LuaContext>(entity0id, luaContext);
		}

		void CallUpdateMethod::Update(
			ECS2::Entity::Id entity0id, 
			const ScriptName* scriptName0, 
			const ObjectName* objectName0,
			LuaContext* luaContext0) {
			
			luabridge::LuaRef cameraLuaObject = luaContext0->context_->GetGlobalAsRef(objectName0->name_);

			luabridge::LuaRef cameraObject = cameraLuaObject["New"](cameraLuaObject)[0];

			luabridge::LuaRef luaEngineEntity = cameraObject["EngineEntity"];
			Entity* luaEntity = luaEngineEntity.cast<Entity*>().value();	
			luaEntity->SetWorld(world_.get());
			luaEntity->SetId((std::uint64_t)entity0id);

			luabridge::LuaRef cameraInputProcessor = luaContext0->context_->GetGlobalAsRef("CameraInputProcessor");
			luabridge::LuaRef processKeyboardInputMethod = cameraInputProcessor["ProcessKeyboardInput"];
			processKeyboardInputMethod(cameraInputProcessor, cameraObject, "W", "Pressed");

			luabridge::LuaRef cameraUpdater = luaContext0->context_->GetGlobalAsRef("CameraUpdater");
			luabridge::LuaRef updateMethod = cameraUpdater["Update"];
			updateMethod(cameraUpdater, cameraObject, 1);

		};

		void CallInputProcessor::Update(ECS2::Entity::Id entity0id, const ScriptName* scriptName0, const ObjectName* objectName0,
			LuaContext* luaContext0, KeyboardInput* keyboardInput0, MouseInput* mouseInput0) {
		
		
		
		
		};

	} // namespace Behaviour

} // namespace OksEngine