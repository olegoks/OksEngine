#include <Behaviour/OksEngine.Behaviour.hpp>
#include <Resources/OksEngine.Resource.Subsystem.hpp>
#include <OksEngine.Config.hpp>
#include <Geometry.Shapes.hpp>
#include <OksEngine.Components.hpp>

namespace OksEngine {

	//BehaviourDeprecated::BehaviourDeprecated(
	//	Context* context,
	//	ECS::Entity::Id entityId,
	//	std::string scriptName,
	//	std::string objectName) :
	//	ECSComponent{ context },
	//	scriptName_{ scriptName },
	//	objectName_{ objectName },
	//	state_{ luaL_newstate() },
	//	updater_{ state_ },
	//	inputProcessor_{ state_ },
	//	object_{ state_ },
	//	entity_{  } {

	//	luaL_openlibs(state_);
	//	lua_pcall(state_, 0, 0, 0);

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<LuaEntity>("EngineEntity")
	//		.addConstructor<void(*)()>()
	//		.addFunction("GetPosition", &LuaEntity::GetPosition)
	//		.addFunction("GetImmutableRenderGeometry", &LuaEntity::GetImmutableRenderGeometry)
	//		.addFunction("GetCamera", &LuaEntity::GetCamera)
	//		.addFunction("GetRigidBodyBox", &LuaEntity::GetRigidBodyBox)
	//		.addFunction("GetRigidBodyCapsule", &LuaEntity::GetRigidBodyCapsule)
	//		.endClass();

	//	Bind<Position>(context_);

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<ImmutableRenderGeometry>("ImmutableRenderGeometry")
	//		.addConstructor<void(*)(std::string)>()
	//		.endClass();

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<SkinnedGeometry>("SkinnedGeometry")
	//		.addConstructor<void(*)(Context*, std::string, std::string, const std::vector<std::string>&)>()
	//		.addFunction("Rotate", &SkinnedGeometry::Rotate)
	//		.endClass();

	//	Bind<Camera>(context_);

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<StaticRigidBodyCustomMeshShape>("StaticRigidBodyCustomMeshShape")
	//		.addConstructor<void(*)()>()
	//		.endClass();

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<DynamicRigidBodyBox>("RigidBodyBox")
	//		.addConstructor<void(*)()>()
	//		.endClass();

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<DynamicRigidBodyCapsule>("RigidBodyCapsule")
	//		.addConstructor<void(*)()>()
	//		.addFunction("ApplyForce", &DynamicRigidBodyCapsule::ApplyForce)
	//		.addFunction("SetVelocity", &DynamicRigidBodyCapsule::SetVelocity)
	//		.endClass();

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<DynamicRigidBodyCustomMeshShape>("RigidBodyCustomMeshShape")
	//		.addConstructor<void(*)()>()
	//		.addFunction("ApplyForce", &DynamicRigidBodyCustomMeshShape::ApplyForce)
	//		.endClass();

	//	luabridge::getGlobalNamespace(state_)
	//		.beginClass<PointLight>("PointLight")
	//		.addConstructor<void(*)(
	//			Context* context,
	//			const glm::vec3& color,
	//			float intensity,
	//			float radius)>()
	//		.addFunction("GetRadius", &PointLight::GetRadius)
	//		.endClass();


	//	auto resourceSubsystem = context->GetResourceSubsystem();

	//	const auto entityScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/Entity.lua");
	//	ResourceSubsystem::Resource entityScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, entityScriptTaskId);
	//	std::string entityScriptText{ entityScriptResource.GetData<char>(), entityScriptResource.GetSize() };
	//	if (luaL_dostring(state_, entityScriptText.c_str())) {
	//		printf("Error: %s\n", lua_tostring(state_, -1));
	//		exit(-1);
	//	}

	//	//Set scripts path in lua object to use it to load scripts modules from scripts folder.
	//	{
	//		using namespace std::string_literals;
	//		const std::filesystem::path scriptsPath = GetContext().GetConfig()->GetValueAs<std::string>("ResourceSystem.scriptsRootDirectory");
	//		const std::filesystem::path configFilePath = GetContext().GetCommandLineParameters().GetValue("-cfg");
	//		const auto scriptsFullPath = configFilePath.parent_path() / scriptsPath;
	//		//const auto executableFullPath = GetContext().GetCommandLineParameters().GetExecutablePath();
	//		//const auto scriptsRelativePath = std::filesystem::relative(scriptsFullPath, std::filesystem::current_path());

	//		lua_getglobal(state_, "package");
	//		lua_getfield(state_, -1, "path"); // get field "path" from table at top of stack (-1)
	//		std::string cur_path = lua_tostring(state_, -1); // grab path string from top of stack
	//		cur_path.append(";"); // do your path magic here
	//		cur_path.append(scriptsFullPath.string() + "?.lua");
	//		lua_pop(state_, 1); // get rid of the string on the stack we just pushed on line 5
	//		lua_pushstring(state_, cur_path.c_str()); // push the new one
	//		lua_setfield(state_, -2, "path"); // set the field "path" in table at -2 with value at top of stack
	//		lua_pop(state_, 1); // get rid of package table from top of stack

	//	}
	//	const auto objectScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/" + scriptName);
	//	ResourceSubsystem::Resource objectScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, objectScriptTaskId);
	//	std::string objectScriptText{ objectScriptResource.GetData<char>(), objectScriptResource.GetSize() };

	//	if (luaL_dostring(state_, objectScriptText.c_str())) {
	//		printf("Error: %s\n", lua_tostring(state_, -1));
	//		exit(-1);
	//	}



	//	{
	//		using namespace std::string_literals;
	//		std::string createObjectCode = "object = "s + objectName_ + ":New()"s;
	//		int result = 0;
	//		result = luaL_dostring(state_, createObjectCode.c_str());
	//		if (result != LUA_OK) {
	//			printf("Error: %s\n", lua_tostring(state_, -1));
	//			lua_pop(state_, 1); // pop error message
	//		}

	//		object_ = luabridge::getGlobal(state_, "object");

	//		auto entity = object_["EngineEntity"].cast<LuaEntity*>();
	//		entity.value()->SetWorld(context->GetECSWorld().get());
	//		entity.value()->SetId(entityId);

	//		std::string updaterName = std::string{ objectName_ } + "Updater";
	//		updater_ = luabridge::getGlobal(state_, updaterName.c_str());
	//		OS::AssertMessage(!updater_.isNil(), { "Updater is not found in {}", scriptName_ });
	//		std::string inputProcessorName = std::string{ objectName_ } + "InputProcessor";
	//		inputProcessor_ = luabridge::getGlobal(state_, inputProcessorName.c_str());

	//	}



	//}

	//void BehaviourDeprecated::CallInputProcessor(const char* inputKey, const char* inputEvent, double offsetX, double offsetY) {
	//	if (!inputProcessor_.isNil()) {
	//		luabridge::LuaRef processInputMethod = inputProcessor_["ProcessInput"];
	//		luabridge::LuaResult result = processInputMethod(updater_, object_, inputKey, inputEvent, offsetX, offsetY);
	//		if (result.hasFailed()) {
	//			OS::AssertFail();
	//		}
	//	}
	//}


	//void BehaviourDeprecated::CallUpdater(Common::Size ms) {
	//	luabridge::LuaRef updateMethod = updater_["Update"];
	//	auto increaseCounter = object_["IncreaseCounter"];
	//	luabridge::LuaResult result = updateMethod(updater_, object_, ms);
	//	if (result.hasFailed()) {
	//		OS::AssertFail();
	//	}
	//}

	template<>
	void Edit<Behaviour>(Behaviour* behaviour) {
		ImGui::TextDisabled("Script name: %s", behaviour->scriptName_.c_str());
		ImGui::TextDisabled("Object name: %s", behaviour->objectName_.c_str());
	}


	template<>
	void Add<Behaviour>(ECS::World* world, ECS::Entity::Id id) {
		static char scriptName[100]{ "Camera.lua" };
		static char objectName[100]{ "Camera" };
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Script name:", scriptName, 100);
			ImGui::InputText("Object name:", objectName, 100);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Behaviour>(id)) {
				world->CreateComponent<Behaviour>(id, std::string{ scriptName }, std::string{ objectName });
			}
		}
	}

	template<>
	void Edit<LuaScript>(LuaScript* luaScript) {
		ImGui::TextDisabled("Script id: %d", luaScript->id_);
	}


	template<>
	void Add<LuaScript>(ECS::World* world, ECS::Entity::Id id) {
		static char scriptName[100]{ "Camera.lua" };
		static char objectName[100]{ "Camera" };
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Script name:", scriptName, 100);
			ImGui::InputText("Object name:", objectName, 100);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Behaviour>(id)) {
				world->CreateComponent<Behaviour>(id, std::string{ scriptName }, std::string{ objectName });
			}
		}
	}

	//template<>
	//void Edit(BehaviourDeprecated* behaviour) {
	//	ImGui::TextDisabled("Script name: %s", behaviour->scriptName_.c_str());
	//	ImGui::TextDisabled("Object name: %s", behaviour->objectName_.c_str());
	//}

	void CreateLuaContext::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		Lua::Context context;

		context.GetGlobalNamespace()
			.beginClass<LuaEntity>("EngineEntity")
			.addConstructor<void(*)()>()
			.addFunction("GetPosition", &LuaEntity::GetPosition)
			.addFunction("GetImmutableRenderGeometry", &LuaEntity::GetImmutableRenderGeometry)
			.addFunction("GetCamera", &LuaEntity::GetCamera)
			.addFunction("GetRigidBodyBox", &LuaEntity::GetRigidBodyBox)
			.addFunction("GetRigidBodyCapsule", &LuaEntity::GetRigidBodyCapsule)
			.endClass();

		Bind<Position>(context);
		Bind<ImmutableRenderGeometry>(context);

		//context.GetGlobalNamespace()
		//	.beginClass<SkinnedGeometry>("SkinnedGeometry")
		//	.addConstructor<void(*)(Context*, std::string, std::string, const std::vector<std::string>&)>()
		//	.addFunction("Rotate", &SkinnedGeometry::Rotate)
		//	.endClass();

		Bind<Camera>(context);

		//context.GetGlobalNamespace()
		//	.beginClass<StaticRigidBodyCustomMeshShape>("StaticRigidBodyCustomMeshShape")
		//	.addConstructor<void(*)()>()
		//	.endClass();

		//context.GetGlobalNamespace()
		//	.beginClass<DynamicRigidBodyBox>("RigidBodyBox")
		//	.addConstructor<void(*)()>()
		//	.endClass();

		//context.GetGlobalNamespace()
		//	.beginClass<DynamicRigidBodyCapsule>("RigidBodyCapsule")
		//	.addConstructor<void(*)()>()
		//	.addFunction("ApplyForce", &DynamicRigidBodyCapsule::ApplyForce)
		//	.addFunction("SetVelocity", &DynamicRigidBodyCapsule::SetVelocity)
		//	.endClass();

		//context.GetGlobalNamespace()
		//	.beginClass<DynamicRigidBodyCustomMeshShape>("RigidBodyCustomMeshShape")
		//	.addConstructor<void(*)()>()
		//	.addFunction("ApplyForce", &DynamicRigidBodyCustomMeshShape::ApplyForce)
		//	.endClass();

		//context.GetGlobalNamespace()
		//	.beginClass<PointLight>("PointLight")
		//	.addConstructor<void(*)(
		//		Context* context,
		//		const glm::vec3& color,
		//		float intensity,
		//		float radius)>()
		//	.addFunction("GetRadius", &PointLight::GetRadius)
		//	.endClass();


		auto resourceSubsystem = GetContext().GetResourceSubsystem();

		const auto entityScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/Entity.lua");
		ResourceSubsystem::Resource entityScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, entityScriptTaskId);
		std::string entityScriptText{ entityScriptResource.GetData<char>(), entityScriptResource.GetSize() };
		context.LoadScript(entityScriptText);

		////Set scripts path in lua object to use it to load scripts modules from scripts folder.

		using namespace std::string_literals;
		const std::filesystem::path scriptsPath = GetContext().GetConfig()->GetValueAs<std::string>("ResourceSystem.scriptsRootDirectory");
		const std::filesystem::path configFilePath = GetContext().GetCommandLineParameters().GetValue("-cfg");
		const auto scriptsFullPath = configFilePath.parent_path() / scriptsPath;

		context.AddPackagePath(scriptsFullPath.string());

		auto* scriptComponent = world->GetComponent<LuaScript>(entityId);
		const Lua::Script& script = GetContext().GetScriptStorage()->Get(scriptComponent->tag_);
		context.LoadScript(script.text_);

		using namespace std::string_literals;
		auto* behaviourScript = world->GetComponent<Behaviour>(entityId);
		std::string createObjectCode = "object = "s + behaviourScript->objectName_ + ":New()"s;
		context.ExecuteCode(createObjectCode);

		luabridge::LuaRef object = context.GetGlobalAsRef("object");
		luabridge::LuaRef luaEngineEntity = object["EngineEntity"];
		LuaEntity* luaEntity = luaEngineEntity.cast<LuaEntity*>().value();
		luaEntity->SetWorld(GetContext().GetECSWorld().get());
		luaEntity->SetId(entityId);

		if (!context.GetGlobalAsRef(behaviourScript->objectName_ + "InputProcessor").isNil()) {
			world->CreateComponent<HandleKeyboardInputMarker>(entityId);
			world->CreateComponent<HandleMouseInputMarker>(entityId);
		}

		world->CreateComponent<LuaContext>(entityId, std::move(context));
		/// MOVE TYO SEPARATE SYSTEMS 
		//std::string updaterName = std::string{ objectName_ } + "Updater";
		//updater_ = luabridge::getGlobal(state_, updaterName.c_str());
		//OS::AssertMessage(!updater_.isNil(), { "Updater is not found in {}", scriptName_ });
		//std::string inputProcessorName = std::string{ objectName_ } + "InputProcessor";
		//inputProcessor_ = luabridge::getGlobal(state_, inputProcessorName.c_str());

		//}


	}

	void LoadLuaScript::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		Behaviour* behaviour = world->GetComponent<Behaviour>(entityId);
		if (!world->IsComponentExist<LuaScriptLoadRequest>(entityId)) {
			const ECS::Entity::Id resourceEntityId = world->CreateEntity();
			world->CreateComponent<LoadResourceRequest>(resourceEntityId, behaviour->scriptName_);
			world->CreateComponent<LuaScriptLoadRequest>(entityId, resourceEntityId);
		}
		else {
			auto* request = world->GetComponent<LuaScriptLoadRequest>(entityId);
			if (world->IsComponentExist<Resource>(request->resourceEntityId_)) {
				Resource* resource = world->GetComponent<Resource>(request->resourceEntityId_);
				const Lua::Script::Id scriptId = GetContext().GetScriptStorage()->Add(behaviour->scriptName_, Lua::Script{ resource->resourceData_ });
				world->CreateComponent<LuaScript>(entityId, resource->name_, scriptId);
			}
		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> LoadLuaScript::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Behaviour>().Exclude<LuaScript>(), ECS::Entity::Filter{}.ExcludeAll() };
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
		std::string keyboardKey = "";
		std::string keyboardEvent = "";
		if (keyboardInput->HasEvent()) {
			auto event = keyboardInput->GetEvent();
			keyboardKey = magic_enum::enum_name(event.key_).data();
			keyboardEvent = magic_enum::enum_name(event.event_).data();
		}
		float xOffset = 0.f;
		float yOffset = 0.f;
		if (mouseInput->HasEvent()) {
			auto event = mouseInput->GetEvent();
			xOffset = event.offset_.x;
			yOffset = event.offset_.y;
		}
		processInput(inputProcessor,
			object,
			keyboardKey.c_str(),
			keyboardEvent.c_str(),
			xOffset,
			yOffset);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CallInputProcessor::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<Behaviour>()
			.Include<LuaContext>()
			.Include<KeyboardInput>()
			.Include<MouseInput>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}

	void CallUpdateMethod::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* context = world->GetComponent<LuaContext>(entityId);
		auto* behaviour = world->GetComponent<Behaviour>(entityId);
		const std::string objectName = behaviour->objectName_;
		luabridge::LuaRef updater = context->context_.GetGlobalAsRef(objectName + "Updater");
		luabridge::LuaRef object = context->context_.GetGlobalAsRef("object");
		luabridge::LuaRef updateMethod = updater["Update"];
		updateMethod(updater, object, 0);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CallUpdateMethod::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Behaviour>().Include<LuaContext>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	void BehaviourSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		//BehaviourDeprecated* behaviour = world->GetComponent<BehaviourDeprecated>(entityId);
		//if (behaviour == nullptr) return;
		//auto now = std::chrono::high_resolution_clock::now();
		//auto delta = now - behaviour->previousUpdateTimePoint_;
		//behaviour->CallUpdater(std::chrono::duration_cast<std::chrono::milliseconds>(delta).count());
		//behaviour->previousUpdateTimePoint_ = now;
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> BehaviourSystem::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Behaviour>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

}