
#include <Behaviour/OksEngine.CallInputProcessor.hpp>

#include <Behaviour/OksEngine.LuaContext.hpp>
#include <Behaviour/OksEngine.Behaviour.hpp>
#include <UI/OksEngine.UI.KeyboardInput.hpp>
#include <UI/OksEngine.UI.MouseInput.hpp>

namespace OksEngine {

	CallInputProcessor::CallInputProcessor(Context& context) noexcept : ECSSystem{ context } {

	}

	void CallInputProcessor::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* context = world->GetComponent<LuaContext>(entityId);
		auto* behaviour = world->GetComponent<Behaviour>(entityId);
		const std::string objectInputProcessorName = behaviour->objectName_ + "InputProcessor";
		luabridge::LuaRef inputProcessor = context->context_.GetGlobalAsRef(objectInputProcessorName);
		auto* keyboardInput = world->GetComponent<KeyboardInput>(entityId);
		auto* mouseInput = world->GetComponent<MouseInput>(entityId);
		luabridge::LuaRef processInput = inputProcessor["ProcessInput"];
		luabridge::LuaRef object = context->context_.GetGlobalAsRef("object");
		while (keyboardInput->HasEvent()) {
			std::string keyboardKey = "";
			std::string keyboardEvent = "";
			if (keyboardInput->HasEvent()) {
				auto event = keyboardInput->GetEvent();
				keyboardKey = magic_enum::enum_name(event.key_).data();
				keyboardEvent = magic_enum::enum_name(event.event_).data();
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
		while (mouseInput->HasEvent()) {
			std::string keyboardKey = "";
			std::string keyboardEvent = "";
			float xOffset = 0.f;
			float yOffset = 0.f;
			if (mouseInput->HasEvent()) {
				auto event = mouseInput->GetEvent();
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

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CallInputProcessor::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Behaviour>()
			.Include<LuaContext>()
			.Include<KeyboardInput>()
			.Include<MouseInput>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CallInputProcessor::GetTypeId() const noexcept {
		return Common::TypeInfo<CallInputProcessor>().GetId();
	}


}