#include <ECS\Scene\SaveScene\CreateSceneFile/auto_OksEngine.CreateSceneFile.hpp>

#include <OksEngine.ECS.hpp>

#include <luabridge3/LuaBridge/LuaBridge.h>

#include <auto_OksEngine.SerializeEntity.hpp>

namespace OksEngine {

	void CreateSceneFile::Update(ECS2::Entity::Id entity1Id, const Serializable* serializable,
		ECS2::Entity::Id entity2Id, const ECSController* eCSController,
		const SaveSceneRequest* saveSceneRequest,
		LuaScript* luaScript) {

		const ECS2::ComponentsFilter components = GetComponentsFilter(entity1Id);
		std::string parsedEntity;

		//If its not first entity add ",".
		{

			const Common::Index closingÑurlyBraceIndex = luaScript->text_.rfind('}');
			if (closingÑurlyBraceIndex != std::string::npos) {
				//Not first entity.
				const Common::Index openingÑurlyBraceIndex = luaScript->text_.rfind('{');
				if (closingÑurlyBraceIndex > openingÑurlyBraceIndex) {
					luaScript->text_ += ",";
				}
			}
			
		}

		luaScript->text_ += parsedEntity + "\n\t{\n";

		std::string serializedEntity = SerializeEntity(world_, entity1Id);

		//Remove last comma.
		serializedEntity.erase(serializedEntity.rfind(','));


		luaScript->text_ += serializedEntity;

		luaScript->text_ += parsedEntity + "\n\t}";


	};
}