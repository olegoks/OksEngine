#pragma once
#include <Behaviour\auto_OksEngine.Behaviour.hpp>

#include <Behaviour\OksEngine.LuaEntity.hpp>
#include <Behaviour\OksEngine.Math3D.hpp>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Direction3D.hpp>
#include <Render/Camera/auto_OksEngine.Camera.hpp>


#include <magic_enum/magic_enum.hpp>

namespace OksEngine
{
	namespace Behaviour
	{
		void CreateLuaScript::Update(
			ECS2::Entity::Id entity0id, ResourceSystem* resourceSystem0,
			ECS2::Entity::Id entity1id, const ScriptName* scriptName1) {

			Resources::ResourceData scriptResourceData
				= resourceSystem0->system_->GetResourceSynch(
					Subsystem::Type::ChildThread, "Root/" + scriptName1->name_);

			Resources::ResourceData entityResourceData
				= resourceSystem0->system_->GetResourceSynch(
					Subsystem::Type::ChildThread, "Root/Entity.lua");

			Resources::ResourceData math3DResourceData
				= resourceSystem0->system_->GetResourceSynch(
					Subsystem::Type::ChildThread, "Root/Math3D.lua");

			CreateComponent<LuaScript>(entity1id,
	/*			std::string{
					math3DResourceData.GetData<char>(),
					math3DResourceData.GetSize()
				} +*/
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
			BindVector(*luaContext);
			BindMath3D(*luaContext);
			BindLuaEntity(*luaContext);
			BindWorldPosition3D(*luaContext);
			BindDirection3D(*luaContext);
			BindUp3D(*luaContext);

			luaContext->LoadScript(luaScript0->text_);


			luabridge::LuaRef luaClass = luaContext->GetGlobalAsRef(objectName0->name_);

			const std::string createObjectCode = "object = " + objectName0->name_ + ":New()";
			luaContext->ExecuteCode(createObjectCode);

			luabridge::LuaRef luaObject = luaContext->GetGlobalAsRef("object");
			luabridge::LuaRef luaEngineEntity = luaObject["EngineEntity"];
			Entity* luaEntity = luaEngineEntity.cast<Entity*>().value();
			luaEntity->SetWorld(world_.get());
			luaEntity->SetId((std::uint64_t)entity0id);

			CreateComponent<LuaContext>(entity0id, luaContext);


		}

		void CallUpdateMethod::Update(
			ECS2::Entity::Id entity0id,
			const ScriptName* scriptName0,
			const ObjectName* objectName0,
			LuaContext* luaContext0) {

			luabridge::LuaRef luaObject = luaContext0->context_->GetGlobalAsRef("object");

			luabridge::LuaRef luaObjectUpdater = luaContext0->context_->GetGlobalAsRef(objectName0->name_ + "Updater");
			luabridge::LuaRef luaUpdateMethod = luaObjectUpdater["Update"];
			luaUpdateMethod(luaObjectUpdater, luaObject, 1);

		};

		void CallKeyboardInputProcessor::Update(ECS2::Entity::Id entity0id, const ScriptName* scriptName0, const ObjectName* objectName0,
			LuaContext* luaContext0, KeyboardInput* keyboardInput0) {

			luabridge::LuaRef luaObject = luaContext0->context_->GetGlobalAsRef("object");

			luabridge::LuaRef luaObjectInputProcessor = luaContext0->context_->GetGlobalAsRef(objectName0->name_ + "InputProcessor");
			luabridge::LuaRef processKeyboardInputMethod = luaObjectInputProcessor["ProcessKeyboardInput"];



			for (auto [key, event] : keyboardInput0->events_) {
				std::string keyboardKey = "";
				std::string keyboardEvent = "";
				keyboardKey = magic_enum::enum_name(key).data();		// "W"
				keyboardEvent = magic_enum::enum_name(event).data();	// "Pressed"
				const auto result = processKeyboardInputMethod(luaObjectInputProcessor, luaObject, keyboardKey, keyboardEvent);
			
				OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
			}



		};

		void CallMouseInputProcessor::Update(
			ECS2::Entity::Id entity0id,
			const ScriptName* scriptName0,
			const ObjectName* objectName0,
			LuaContext* luaContext0,
			MouseInput* mouseInput0) {


			luabridge::LuaRef luaObject = luaContext0->context_->GetGlobalAsRef("object");

			luabridge::LuaRef luaObjectInputProcessor = luaContext0->context_->GetGlobalAsRef(objectName0->name_ + "InputProcessor");
			luabridge::LuaRef processMouseInputMethod = luaObjectInputProcessor["ProcessMouseInput"];

			auto mouseEvents = mouseInput0->events_;

			while (!mouseEvents.empty()) {
				const auto& event = mouseEvents.back();

				std::string mouseKey = "";
				std::string mouseEvent = "";
				//mouseKey = magic_enum::enum_name(event.key_).data();		// "LEFT_BUTTON"
				//mouseEvent = magic_enum::enum_name(event.event_).data();	// "Pressed"
				const auto result = processMouseInputMethod(luaObjectInputProcessor, luaObject, mouseKey, mouseEvent, event.offset_.x, event.offset_.y);
				OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
				mouseEvents.pop();
			}


		}

	} // namespace Behaviour

} // namespace OksEngine