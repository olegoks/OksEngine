#include <ECS/Scene\SaveScene/SaveSceneFile/auto_OksEngine.SaveSceneFile.hpp>

namespace OksEngine {
	void SaveSceneFile::Update(

		ECS2::Entity::Id entity0id,
		const CommandLineParameters* commandLineParameters0,
		const ConfigFilePath* configFilePath0,

		ECS2::Entity::Id entity1id,
		const ECSController* eCSController1,
		const SaveSceneRequest* saveSceneRequest1,
		const LuaScript* luaScript1,

		ECS2::Entity::Id entity2id,
		ResourceSystem* resourceSystem2,

		ECS2::Entity::Id entity3id,
		const Config* config3,
		const LuaScript* luaScript3) {


		::Lua::Context context;

		::Lua::Script script{ luaScript3->text_ };
		context.LoadScript(script);

		std::filesystem::path scenesRootDirectory =
			std::filesystem::path{ configFilePath0->path_ }.parent_path() 
			/ context.GetGlobalAs<std::string>("scenesRootDirectory");

		Resources::ResourceData sceneFileData{
			luaScript1->text_.data(),
			luaScript1->text_.size()
		};

		auto createSceneFileTask
			= resourceSystem2->system_->CreateNewFile(
				Subsystem::Type::Debug, scenesRootDirectory / ("OksEngine.Test.scn"),
				std::move(sceneFileData)); // draw graph settings
		resourceSystem2->system_->CreateNewFile(Subsystem::Type::Debug, createSceneFileTask);



	};
}