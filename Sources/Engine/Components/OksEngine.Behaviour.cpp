#include <Components/OksEngine.Behaviour.hpp>
#include <OksEngine.Resource.Subsystem.hpp>
#include <filesystem>

namespace OksEngine {

	Behaviour::Behaviour(
		Context& context,
		ECS::Entity::Id entityId,
		std::string scriptName,
		std::string objectName) :
		ECSComponent{ context },
		scriptName_{ scriptName },
		objectName_{ objectName },
		state_{ luaL_newstate() },
		updater_{ state_ },
		object_{ state_ },
		entity_{  } {

		luaL_openlibs(state_);
		lua_pcall(state_, 0, 0, 0);

		luabridge::getGlobalNamespace(state_)
			.beginClass<LuaEntity>("EngineEntity")
			.addConstructor<void(*)()>()
			.addFunction("GetPosition", &LuaEntity::GetPosition)
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<Position>("Position")
			.addConstructor<void(*)(int x, int y, int z)>()
			.addFunction("GetX", &Position::GetX)
			.addFunction("GetY", &Position::GetY)
			.addFunction("GetZ", &Position::GetZ)
			.addFunction("SetX", &Position::SetX)
			.addFunction("SetY", &Position::SetY)
			.addFunction("SetZ", &Position::SetZ)
			.endClass();

		auto resourceSubsystem = context.GetResourceSubsystem();

		const auto entityScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/Entity.lua");
		ResourceSubsystem::Resource entityScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, entityScriptTaskId);
		std::string entityScriptText{ entityScriptResource.GetData<char>(), entityScriptResource.GetSize() };
		if (luaL_dostring(state_, entityScriptText.c_str() )) {
			printf("Error: %s\n", lua_tostring(state_, -1));
			exit(-1);
		}

		const auto objectScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/" + scriptName);
		ResourceSubsystem::Resource objectScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, objectScriptTaskId);
		std::string objectScriptText{ objectScriptResource.GetData<char>(), objectScriptResource.GetSize() };

		if (luaL_dostring(state_, objectScriptText.c_str())) {
			printf("Error: %s\n", lua_tostring(state_, -1));
			exit(-1);
		}


		{
			using namespace std::string_literals;
			std::string createObjectCode = "object = "s + objectName_ + ":New()"s;
			int result = 0;
			result = luaL_dostring(state_, createObjectCode.c_str());
			if (result != LUA_OK) {
				printf("Error: %s\n", lua_tostring(state_, -1));
				lua_pop(state_, 1); // pop error message
			}
			object_ = luabridge::getGlobal(state_, "object");

			auto entity = object_["EngineEntity"].cast<LuaEntity*>();
			entity.value()->SetWorld(context.GetECSWorld().get());
			entity.value()->SetId(entityId);

			std::string updaterName = std::string{ objectName_ } + "Updater";
			updater_ = luabridge::getGlobal(state_, updaterName.c_str());
		}



	}

	void Behaviour::CallUpdater() {
		luabridge::LuaRef updateMethod = updater_["Update"];
		auto increaseCounter = object_["IncreaseCounter"];
		luabridge::LuaResult result = updateMethod(updater_, object_);
		if (result.hasFailed()) {
			//
		}
	}


}