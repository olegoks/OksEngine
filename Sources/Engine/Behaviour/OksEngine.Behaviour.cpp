#include <Behaviour/OksEngine.Behaviour.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <OksEngine.Config.hpp>
#include <filesystem>
#include <Render/OksEngine.ImmutableRenderGeometry.hpp>
#include <Render/OksEngine.SkinnedGeometry.hpp>
#include <Render/OksEngine.Light.hpp>
#include <Render/OksEngine.Camera.hpp>
#include <Geometry.Shapes.hpp>

namespace OksEngine {

	Behaviour::Behaviour(
		Context* context,
		ECS::Entity::Id entityId,
		std::string scriptName,
		std::string objectName) :
		ECSComponent{ context },
		scriptName_{ scriptName },
		objectName_{ objectName },
		state_{ luaL_newstate() },
		updater_{ state_ },
		inputProcessor_{ state_ },
		object_{ state_ },
		entity_{  } {

		luaL_openlibs(state_);
		lua_pcall(state_, 0, 0, 0);

		luabridge::getGlobalNamespace(state_)
			.beginClass<LuaEntity>("EngineEntity")
			.addConstructor<void(*)()>()
			.addFunction("GetPosition", &LuaEntity::GetPosition)
			.addFunction("GetImmutableRenderGeometry", &LuaEntity::GetImmutableRenderGeometry)
			.addFunction("GetCamera", &LuaEntity::GetCamera)
			.addFunction("GetRigidBodyBox", &LuaEntity::GetRigidBodyBox)
			.addFunction("GetRigidBodyCapsule", &LuaEntity::GetRigidBodyCapsule)
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

		luabridge::getGlobalNamespace(state_)
			.beginClass<ImmutableRenderGeometry>("ImmutableRenderGeometry")
			.addConstructor<void(*)(Context*,float, float, float, std::string, std::string, const std::vector<std::string>&)>()
			.addFunction("Rotate", &ImmutableRenderGeometry::Rotate)
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<SkinnedGeometry>("SkinnedGeometry")
			.addConstructor<void(*)(Context*, float, float, float, std::string, std::string, const std::vector<std::string>&)>()
			.addFunction("Rotate", &SkinnedGeometry::Rotate)
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<Camera>("Camera")
			.addConstructor<void(*)(Context* context,
				const glm::vec3& position,
				const glm::vec3& direction,
				const glm::vec3& up)>()
			.addFunction("GetDirectionX", &Camera::GetDirectionX)
			.addFunction("GetDirectionY", &Camera::GetDirectionY)
			.addFunction("GetDirectionZ", &Camera::GetDirectionZ)
			.addFunction("SetDirectionX", &Camera::SetDirectionX)
			.addFunction("SetDirectionY", &Camera::SetDirectionY)
			.addFunction("SetDirectionZ", &Camera::SetDirectionZ)
			.addFunction("SetDirection", &Camera::SetDirection)
			.addFunction("GetUpX", &Camera::GetUpX)
			.addFunction("GetUpY", &Camera::GetUpY)
			.addFunction("GetUpZ", &Camera::GetUpZ)
			.addFunction("SetUpX", &Camera::SetUpX)
			.addFunction("SetUpY", &Camera::SetUpY)
			.addFunction("SetUpZ", &Camera::SetUpZ)
			.addFunction("Forward", &Camera::Forward)
			.addFunction("Backward", &Camera::Backward)
			.addFunction("Right", &Camera::Right)
			.addFunction("Left", &Camera::Left)
			.addFunction("Up", &Camera::Up)
			.addFunction("Down", &Camera::Down)
			.addFunction("DirectionUp", &Camera::DirectionUp)
			.addFunction("DirectionDown", &Camera::DirectionDown)
			.addFunction("DirectionLeft", &Camera::DirectionLeft)
			.addFunction("DirectionRight", &Camera::DirectionRight)
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<StaticRigidBodyCustomMeshShape>("StaticRigidBodyCustomMeshShape")
			.addConstructor<void(*)()>()
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<RigidBodyBox>("RigidBodyBox")
			.addConstructor<void(*)()>()
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<RigidBodyCapsule>("RigidBodyCapsule")
			.addConstructor<void(*)()>()
			.addFunction("ApplyForce", &RigidBodyCapsule::ApplyForce)
			.addFunction("SetVelocity", &RigidBodyCapsule::SetVelocity)
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<DynamicRigidBodyCustomMeshShape>("RigidBodyCustomMeshShape")
			.addConstructor<void(*)()>()
			.addFunction("ApplyForce", &DynamicRigidBodyCustomMeshShape::ApplyForce)
			.endClass();

		luabridge::getGlobalNamespace(state_)
			.beginClass<PointLight>("PointLight")
			.addConstructor<void(*)(
				Context* context,
				const glm::vec3& color,
				float intensity,
				float radius)>()
			.addFunction("GetRadius", &PointLight::GetRadius)
			.endClass();


		auto resourceSubsystem = context->GetResourceSubsystem();

		const auto entityScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/Entity.lua");
		ResourceSubsystem::Resource entityScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, entityScriptTaskId);
		std::string entityScriptText{ entityScriptResource.GetData<char>(), entityScriptResource.GetSize() };
		if (luaL_dostring(state_, entityScriptText.c_str() )) {
			printf("Error: %s\n", lua_tostring(state_, -1));
			exit(-1);
		}

		//Set scripts path in lua object to use it to load scripts modules from scripts folder.
		{
			using namespace std::string_literals;
			const std::filesystem::path scriptsPath = GetContext().GetConfig()->GetValueAs<std::string>("ResourceSystem.scriptsRootDirectory");
			const std::filesystem::path configFilePath = GetContext().GetCommandLineParameters().GetValue("-cfg");
			const auto scriptsFullPath = configFilePath.parent_path() / scriptsPath;
			//const auto executableFullPath = GetContext().GetCommandLineParameters().GetExecutablePath();
			//const auto scriptsRelativePath = std::filesystem::relative(scriptsFullPath, std::filesystem::current_path());

			lua_getglobal(state_, "package");
			lua_getfield(state_, -1, "path"); // get field "path" from table at top of stack (-1)
			std::string cur_path = lua_tostring(state_, -1); // grab path string from top of stack
			cur_path.append(";"); // do your path magic here
			cur_path.append(scriptsFullPath.string() +  "?.lua");
			lua_pop(state_, 1); // get rid of the string on the stack we just pushed on line 5
			lua_pushstring(state_, cur_path.c_str()); // push the new one
			lua_setfield(state_, -2, "path"); // set the field "path" in table at -2 with value at top of stack
			lua_pop(state_, 1); // get rid of package table from top of stack

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
			entity.value()->SetWorld(context->GetECSWorld().get());
			entity.value()->SetId(entityId);

			std::string updaterName = std::string{ objectName_ } + "Updater";
			updater_ = luabridge::getGlobal(state_, updaterName.c_str());
			OS::AssertMessage(!updater_.isNil(), { "Updater is not found in {}", scriptName_});
			std::string inputProcessorName = std::string{ objectName_ } + "InputProcessor";
			inputProcessor_ = luabridge::getGlobal(state_, inputProcessorName.c_str());

		}



	}

	void Behaviour::CallInputProcessor(const char* inputKey, const char* inputEvent, double offsetX, double offsetY) {
		if (!inputProcessor_.isNil()) {
			luabridge::LuaRef processInputMethod = inputProcessor_["ProcessInput"];
			luabridge::LuaResult result = processInputMethod(updater_, object_, inputKey, inputEvent, offsetX, offsetY);
			if (result.hasFailed()) {
				OS::AssertFail();
			}
		}
	}


	void Behaviour::CallUpdater(Common::Size ms) {
		luabridge::LuaRef updateMethod = updater_["Update"];
		auto increaseCounter = object_["IncreaseCounter"];
		luabridge::LuaResult result = updateMethod(updater_, object_, ms);
		if (result.hasFailed()) {
			OS::AssertFail();
		}
	}


}