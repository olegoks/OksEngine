
#include <string>

#include <Behaviour/OksEngine.CreateLuaContext.hpp>

#include <OksEngine.Config.hpp>
#include <OksEngine.Components.hpp>
#include <Behaviour/OksEngine.LuaEntity.hpp>
#include <Behaviour/OksEngine.Math3D.hpp>

namespace OksEngine {


	CreateLuaContext::CreateLuaContext(Context& context) noexcept : ECSSystem{ context } {

	}

	void CreateLuaContext::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		Lua::Context context;

		context.GetGlobalNamespace()
			.beginClass<LuaEntity>("EngineEntity")
			.addConstructor<void(*)()>()
			.addFunction("GetPosition", &LuaEntity::GetPosition)
			.addFunction("GetDirection", &LuaEntity::GetDirection)
			.addFunction("GetImmutableRenderGeometry", &LuaEntity::GetImmutableRenderGeometry)
			.addFunction("GetCamera", &LuaEntity::GetCamera)
			.addFunction("GetActive", &LuaEntity::GetActive)
			.addFunction("GetUp", &LuaEntity::GetUp)
			.addFunction("GetHeight", &LuaEntity::GetHeight)
			.addFunction("GetWidth", &LuaEntity::GetWidth)
			//.addFunction("GetRigidBodyBox", &LuaEntity::GetRigidBodyBox)
			//.addFunction("GetRigidBodyCapsule", &LuaEntity::GetRigidBodyCapsule)
			.endClass();

		context.GetGlobalNamespace()
			.beginClass<LuaVector>("Vector")
			.addConstructor<void(*)(float x, float y, float z)>()
			.addFunction("Normalize", &LuaVector::Normalize)
			.addFunction("GetX", &LuaVector::GetX)
			.addFunction("GetY", &LuaVector::GetY) 
			.addFunction("GetZ", &LuaVector::GetZ)

			.endClass();


		context.GetGlobalNamespace()
			.beginClass<Math3D>("Math3D")
			.addConstructor<void(*)()>()
			.addFunction("RotateVector", &Math3D::RotateVector)

			.endClass();

		Bind<Position>(context);
		Bind<Direction>(context);
		Bind<ImmutableRenderGeometry>(context);
		Bind<Camera>(context);

		auto resourceSubsystem = GetContext().GetResourceSubsystem();

		const auto math3DScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/Math3D.lua");
		ResourceSubsystem::Resource math3DScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, math3DScriptTaskId);
		std::string math3DScriptText{ math3DScriptResource.GetData<char>(), math3DScriptResource.GetSize() };

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

		const ECS::Entity::Id luaScriptEntity = world->GetComponent<LuaScriptEntity>(entityId)->id_;
		auto* scriptComponent = world->GetComponent<LuaScript>(luaScriptEntity);
		auto* scriptName = world->GetComponent<Name>(luaScriptEntity);
		auto* scriptText = world->GetComponent<Text>(luaScriptEntity);
		//const Lua::Script& script = GetContext().GetScriptStorage()->Get(scriptName->value_);
		context.LoadScript(scriptText->text_);

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

	}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLuaContext::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Behaviour>().Include<LuaScriptEntity>().Exclude<LuaContext>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	inline Common::TypeId CreateLuaContext::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLuaContext>().GetId();
	}


}