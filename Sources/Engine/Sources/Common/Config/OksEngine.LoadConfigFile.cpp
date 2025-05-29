#include <Common/Config\auto_OksEngine.LoadConfigFile.hpp>

namespace OksEngine {
void LoadConfigFile::Update(ECS2::Entity::Id entity1Id,
    const CommandLineParameters* commandLineParameters,
    const ConfigFilePath* configFilePath, ECS2::Entity::Id entity2Id,
    ResourceSystem* resourceSystem) {

    auto resourceData = resourceSystem->system_->GetResourceSynch(Subsystem::Type::Engine, configFilePath->path_);

    const ECS2::Entity::Id entityId = CreateEntity();

    CreateComponent<Config>(entityId);
    CreateComponent<LuaScript>(entityId, 
        std::string{ resourceData.GetData<Common::Byte>(), resourceData.GetSize() } );


};
}