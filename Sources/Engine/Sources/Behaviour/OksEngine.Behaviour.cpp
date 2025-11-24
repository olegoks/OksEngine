#pragma once
#include <Behaviour\auto_OksEngine.Behaviour.hpp>

#include <Behaviour\OksEngine.LuaEntity.hpp>
#include <Behaviour\OksEngine.Math3D.hpp>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Common/auto_OksEngine.Direction3D.hpp>
#include <Render/Camera/auto_OksEngine.Camera.hpp>
#include <Physics/auto_OksEngine.Physics.hpp>

#include <magic_enum/magic_enum.hpp>

#include <auto_OksEngine.BindECSWorld.hpp>

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

			//Resources::ResourceData math3DResourceData
			//	= resourceSystem0->system_->GetResourceSynch(
			//		Subsystem::Type::ChildThread, "Root/Math3D.lua");

			CreateComponent<LuaScript>(entity1id,
				/*std::string{
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
			Physics::BindRigidBodyEntityId(*luaContext);
			Physics::BindSetVelocityRequests(*luaContext);
			Physics::BindSetAngularVelocityRequests(*luaContext);
			BindUp3D(*luaContext);
			BindECSWorld(*luaContext);

			luabridge::setGlobal(luaContext->state_, world_.get(), "ECSWorld");

			luaContext->LoadScript(luaScript0->text_);


			luabridge::LuaRef luaClass = luaContext->GetGlobalAsRef(objectName0->name_);

			const std::string createObjectCode = "object = " + objectName0->name_ + ":New()";
			luaContext->ExecuteCode(createObjectCode);

			luabridge::LuaRef luaObject = luaContext->GetGlobalAsRef("object");

			luabridge::LuaRef luaEngineEntity = luaObject["engineEntity"];
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
			const auto result = luaUpdateMethod(luaObjectUpdater, luaObject, 1);
			ASSERT_MSG(!result.hasFailed() && result.wasOk(), (result.errorCode().message() + result.errorMessage()).c_str());
		};

		void CallKeyboardEventsProcessor::Update(
			ECS2::Entity::Id entity0id, 
			const ScriptName* scriptName0,
			const ObjectName* objectName0,
			LuaContext* luaContext0,
			Input::KeyboardEvents* keyboardEvents0,
			const Input::KeyMappings* input__KeyMappings0) {

			luabridge::LuaRef luaObject = luaContext0->context_->GetGlobalAsRef("object");

			luabridge::LuaRef luaObjectInputProcessor = luaContext0->context_->GetGlobalAsRef(objectName0->name_ + "InputProcessor");
			luabridge::LuaRef processKeyboardInputMethod = luaObjectInputProcessor["ProcessKeyboardInput"];



			for (auto [key, event] : keyboardEvents0->events_) {

				std::string keyboardKey = "";
				std::string keyboardEvent = "";
				keyboardKey = magic_enum::enum_name(key).data();		// "W"
				keyboardEvent = magic_enum::enum_name(event).data();	// "Pressed"

				auto it = input__KeyMappings0->mappings_.find(keyboardKey);
				if (it != input__KeyMappings0->mappings_.cend()) {
					keyboardKey = it->second;
					const auto result = processKeyboardInputMethod(luaObjectInputProcessor, luaObject, keyboardKey, keyboardEvent);

					ASSERT_MSG(!result.hasFailed() && result.wasOk(), (result.errorCode().message() + result.errorMessage()).c_str());
				}
			}

		};

		void CallCursorEventsProcessor::Update(
			ECS2::Entity::Id entity0id,
			const ScriptName* scriptName0,
			const ObjectName* objectName0,
			LuaContext* luaContext0,
			CursorEvents* cursorInput0) {


			luabridge::LuaRef luaObject = luaContext0->context_->GetGlobalAsRef("object");

			luabridge::LuaRef luaObjectInputProcessor = luaContext0->context_->GetGlobalAsRef(objectName0->name_ + "InputProcessor");
			luabridge::LuaRef processMouseInputMethod = luaObjectInputProcessor["ProcessMouseInput"];

			auto cursorEvents = cursorInput0->events_;

			while (!cursorEvents.empty()) {
				const auto& event = cursorEvents.back();

				std::string mouseKey = "";
				std::string mouseEvent = "";
				//mouseKey = magic_enum::enum_name(event.key_).data();		// "LEFT_BUTTON"
				//mouseEvent = magic_enum::enum_name(event.event_).data();	// "Pressed"
				const auto result = processMouseInputMethod(luaObjectInputProcessor, luaObject, mouseKey, mouseEvent, event.offset_.x, event.offset_.y, 0.0f);
				ASSERT_MSG(!result.hasFailed() && result.wasOk(), (result.errorCode().message() + result.errorMessage()).c_str());
				cursorEvents.pop();
			}


		}

		void CallMouseEventsProcessor::Update(
			ECS2::Entity::Id entity0id,
			const ScriptName* scriptName0,
			const ObjectName* objectName0,
			LuaContext* luaContext0,
			MouseEvents* mouseEvents0) {


			luabridge::LuaRef luaObject = luaContext0->context_->GetGlobalAsRef("object");

			luabridge::LuaRef luaObjectInputProcessor = luaContext0->context_->GetGlobalAsRef(objectName0->name_ + "InputProcessor");
			luabridge::LuaRef processMouseInputMethod = luaObjectInputProcessor["ProcessMouseInput"];

			auto mouseEvents = mouseEvents0->events_;

			while (!mouseEvents.empty()) {
				const auto& event = mouseEvents.back();

				std::string mouseKey = "";
				std::string mouseEvent = "";
				//mouseKey = magic_enum::enum_name(event.key_).data();		// "LEFT_BUTTON"
				//mouseEvent = magic_enum::enum_name(event.event_).data();	// "Pressed"
				const auto result = processMouseInputMethod(luaObjectInputProcessor, luaObject, mouseKey, mouseEvent, 0.0f, 0.0f, event.scroll_);
				ASSERT_MSG(!result.hasFailed() && result.wasOk(), (result.errorCode().message() + result.errorMessage()).c_str());
				mouseEvents.pop();
			}

		}

	} // namespace Behaviour

} // namespace OksEngine