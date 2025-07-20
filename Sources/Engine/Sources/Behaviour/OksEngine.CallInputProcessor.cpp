
#include <Behaviour/auto_OksEngine.CallInputProcessor.hpp>

#include <magic_enum/magic_enum.hpp>

namespace OksEngine {

	void CallInputProcessor::Update(
		ECS2::Entity::Id entityId,
		const Behaviour* behaviour,
		LuaContext* luaContext,
		KeyboardInput* keyboardInput,
		MouseInput* mouseInput) {

		const std::string objectInputProcessorName = behaviour->objectName_ + "InputProcessor";
		luabridge::LuaRef inputProcessor = luaContext->context_.GetGlobalAsRef(objectInputProcessorName);
		luabridge::LuaRef processInput = inputProcessor["ProcessInput"];
		luabridge::LuaRef object = luaContext->context_.GetGlobalAsRef("object");
		while (!keyboardInput->events_.empty()) {
			std::string keyboardKey = "";
			std::string keyboardEvent = "";
			if (!keyboardInput->events_.empty()) {
				auto event = keyboardInput->events_[0];
				keyboardKey = magic_enum::enum_name(event.first).data();
				keyboardEvent = magic_enum::enum_name(event.second).data();
			}
			float xOffset = 0.f;
			float yOffset = 0.f;
			const luabridge::LuaResult result = processInput(inputProcessor,
				object,
				keyboardKey.c_str(),
				keyboardEvent.c_str(),
				xOffset,
				yOffset);
			OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
		}
		while (!mouseInput->events_.empty()) {
			std::string keyboardKey = "";
			std::string keyboardEvent = "";
			float xOffset = 0.f;
			float yOffset = 0.f;
			if (!mouseInput->events_.empty()) {
				auto event = mouseInput->events_.back();
				xOffset = event.offset_.x;
				yOffset = event.offset_.y;
			}
			const luabridge::LuaResult result = processInput(inputProcessor,
				object,
				keyboardKey.c_str(),
				keyboardEvent.c_str(),
				xOffset,
				yOffset);
			OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
		}
	}

}