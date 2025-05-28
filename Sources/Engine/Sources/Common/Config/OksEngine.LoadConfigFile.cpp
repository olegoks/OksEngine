#include <Common/Config\auto_OksEngine.LoadConfigFile.hpp>

namespace OksEngine {
void LoadConfigFile::Update(ECS2::Entity::Id entityId) {

    auto resourceData = resourceSystem->system_->GetResourceSynch(ResourceSubsystem::Type::Engine, configFilePath->path_);

    const ECS2::Entity::Id entityId = CreateEntity();

    CreateComponent<Config>(entityId);
    CreateComponent<LuaScript>(entityId, 
        std::string{ resourceData.GetData<Common::Byte>(), resourceData.GetSize() } );


};
}