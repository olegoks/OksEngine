#pragma once
#include <Common\Config\auto_OksEngine.Config.hpp>

namespace OksEngine
{
void LoadConfigFile::Update(ECS2::Entity::Id entity0id, const CommandLineParameters *commandLineParameters0,
                            const ConfigFilePath *configFilePath0, ECS2::Entity::Id entity1id,
                            ResourceSystem *resourceSystem1) {

    auto resourceData = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, configFilePath0->path_);

    const ECS2::Entity::Id entityId = CreateEntity();

    CreateComponent<Config>(entityId);
    CreateComponent<LuaScript>(entityId,
        std::string{ resourceData.GetData<Common::Byte>(), resourceData.GetSize() });


};

}