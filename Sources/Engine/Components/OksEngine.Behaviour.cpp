#include <Components/OksEngine.Behaviour.hpp>

#include <filesystem>

namespace OksEngine {

	Behaviour::Behaviour(
		ECS::World* world,
		ECS::Entity::Id entityId,
		std::filesystem::path scriptsPath,
		std::string scriptName,
		std::string objectName) :
		scriptName_{ scriptName },
		objectName_{ objectName },
		state_{ luaL_newstate() },
		updater_{ state_ },
		object_{ state_ },
		entity_{  }, context_{ } {

		OS::AssertMessage(std::filesystem::exists(scriptsPath), 
			{ "Scripts path is not exist %s", scriptsPath.c_str()});

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

		const std::filesystem::path entityScriptPath = scriptsPath / "Entity.lua";
		if (luaL_dofile(state_, entityScriptPath.string().c_str())) {
			printf("Error: %s\n", lua_tostring(state_, -1));
			exit(-1);
		}

		const std::filesystem::path objectScriptPath = scriptsPath / scriptName_;
		if (luaL_dofile(state_, objectScriptPath.string().c_str())) {
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
			entity.value()->SetWorld(world);
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