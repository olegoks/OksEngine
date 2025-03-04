
#include <Behaviour/auto_OksEngine.CallInputProcessor.hpp>

namespace OksEngine {

	void CallInputProcessor::Update(
		ECS2::Entity::Id entityId,
		const Behaviour* behaviour,
		LuaContext* luaContext,
		KeyboardInput* keyboardInput,
		MouseInput* mouseInput) {

		//const std::string objectInputProcessorName = behaviour->objectName_ + "InputProcessor";
		//luabridge::LuaRef inputProcessor = context->context_.GetGlobalAsRef(objectInputProcessorName);
		//auto* keyboardInput = world->GetComponent<KeyboardInput>(entityId);
		//auto* mouseInput = world->GetComponent<MouseInput>(entityId);
		//luabridge::LuaRef processInput = inputProcessor["ProcessInput"];
		//luabridge::LuaRef object = context->context_.GetGlobalAsRef("object");
		//while (keyboardInput->HasEvent()) {
		//	std::string keyboardKey = "";
		//	std::string keyboardEvent = "";
		//	if (keyboardInput->HasEvent()) {
		//		auto event = keyboardInput->GetEvent();
		//		keyboardKey = magic_enum::enum_name(event.key_).data();
		//		keyboardEvent = magic_enum::enum_name(event.event_).data();
		//	}
		//	float xOffset = 0.f;
		//	float yOffset = 0.f;
		//	const luabridge::LuaResult result = processInput(inputProcessor,
		//		object,
		//		keyboardKey.c_str(),
		//		keyboardEvent.c_str(),
		//		xOffset,
		//		yOffset);
		//	OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
		//}
		//while (mouseInput->HasEvent()) {
		//	std::string keyboardKey = "";
		//	std::string keyboardEvent = "";
		//	float xOffset = 0.f;
		//	float yOffset = 0.f;
		//	if (mouseInput->HasEvent()) {
		//		auto event = mouseInput->GetEvent();
		//		xOffset = event.offset_.x;
		//		yOffset = event.offset_.y;
		//	}
		//	const luabridge::LuaResult result = processInput(inputProcessor,
		//		object,
		//		keyboardKey.c_str(),
		//		keyboardEvent.c_str(),
		//		xOffset,
		//		yOffset);
		//	OS::AssertMessage(!result.hasFailed() && result.wasOk(), result.errorCode().message() + result.errorMessage());
		//}
	}

}