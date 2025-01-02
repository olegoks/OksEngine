
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

		::Lua::Context context;

		Bind<Lua::Entity>(context);
		Bind<Lua::Vector>(context);
		Bind<Lua::Math3D>(context);
		Bind<Position3D>(context);
		Bind<Direction3D>(context);
		Bind<Up>(context);
		Bind<ImmutableRenderGeometry>(context);
		Bind<Camera>(context);

		auto resourceSubsystem = GetContext().GetResourceSubsystem();

		/*const auto math3DScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/Math3D.lua");
		ResourceSubsystem::Resource math3DScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, math3DScriptTaskId);
		std::string math3DScriptText{ math3DScriptResource.GetData<char>(), math3DScriptResource.GetSize() };*/

		const auto entityScriptTaskId = resourceSubsystem->GetResource(Subsystem::Type::Engine, "Root/Entity.lua");
		Resources::ResourceData entityScriptResource = resourceSubsystem->GetResource(Subsystem::Type::Engine, entityScriptTaskId);
		std::string entityScriptText{ entityScriptResource.GetData<char>(), entityScriptResource.GetSize() };

		//context.LoadScript(math3DScriptText);
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
		context.LoadScript(scriptText->value_);

		using namespace std::string_literals;
		auto* behaviourScript = world->GetComponent<Behaviour>(entityId);
		std::string createObjectCode = "object = "s + behaviourScript->objectName_ + ":New()"s;
		context.ExecuteCode(createObjectCode);

		luabridge::LuaRef object = context.GetGlobalAsRef("object");
		luabridge::LuaRef luaEngineEntity = object["EngineEntity"];
		Lua::Entity* luaEntity = luaEngineEntity.cast<Lua::Entity*>().value();
		luaEntity->SetWorld(GetContext().GetECSWorld().get());
		luaEntity->SetId(entityId);

		if (!context.GetGlobalAsRef(behaviourScript->objectName_ + "InputProcessor").isNil()) {
			world->CreateComponent<HandleKeyboardInputMarker>(entityId);
			world->CreateComponent<HandleMouseInputMarker>(entityId);
		}

		world->CreateComponent<LuaContext>(entityId, std::move(context));

	}


	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateLuaContext::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = { ECS::Entity::Filter{}.Include<Behaviour>().Include<LuaScriptEntity>().Exclude<LuaContext>(), ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}

	Common::TypeId CreateLuaContext::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateLuaContext>().GetId();
	}


}