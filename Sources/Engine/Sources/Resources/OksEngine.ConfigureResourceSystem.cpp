#include <Resources\auto_OksEngine.ConfigureResourceSystem.hpp>

namespace OksEngine {
	void ConfigureResourceSystem::Update(ECS2::Entity::Id entityId1,
		const CommandLineParameters* commandLineParameters,
		const ConfigFilePath* configFilePath, ECS2::Entity::Id entityId2,
		ResourceSystem* resourceSystem, ECS2::Entity::Id entityId,
		const Config* config, const Behaviour::LuaScript* luaScript) {

		::Lua::Context context;

		::Lua::Script script{ luaScript->text_ };
		context.LoadScript(script);

		const std::filesystem::path configFileOsPath = configFilePath->path_;

		auto resourcesRootPath = context.GetGlobalAs<std::string>("ResourceSystem.resourcesRootDirectory");
		auto fullResourcesPath = configFileOsPath.parent_path() / resourcesRootPath;

		auto scriptsRootPath = context.GetGlobalAs<std::string>("ResourceSystem.scriptsRootDirectory");
		auto scriptsFullResourcesPath = configFileOsPath.parent_path() / scriptsRootPath;

		auto ecsFilesRootPath = context.GetGlobalAs<std::string>("ECSCallGraphEditor.ecsFilesRootDirectory");
		auto ecsFilesFullResourcesPath = configFileOsPath.parent_path() / ecsFilesRootPath;

		auto scenesFilesRootPath = context.GetGlobalAs<std::string>("scenesRootDirectory");
		auto scenesFilesFullResourcesPath = configFileOsPath.parent_path() / scenesFilesRootPath;


		resourceSystem->system_->SetRoot(Subsystem::Type::Engine, 
			{ 	scenesFilesFullResourcesPath, 
				scriptsFullResourcesPath,
				fullResourcesPath, 
				ecsFilesFullResourcesPath });


	};
}