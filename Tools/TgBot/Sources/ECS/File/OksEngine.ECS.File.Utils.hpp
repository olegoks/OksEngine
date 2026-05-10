#pragma once

#include <string>
#include <regex>
#include <luabridge3/LuaBridge/LuaBridge.h>

namespace OksEngine::ECS::File::Table {

#define ECS__FILE__TABLE__PARSE_CONSTANT(luaRef, name)\
    [this](luabridge::LuaRef constant,\
    const std::string& constantName){                               \
        const ECS2::Entity::Id constantEntityId = CreateEntity<ECS__FILE__TABLE__CONSTANT__CONSTANT>();   \
        luabridge::LuaRef type = constant["type"];                  \
        luabridge::LuaRef value = constant["value"];                \
        CreateComponent<ECS::File::Table::Tag>(constantEntityId);\
        CreateComponent<ECS::File::Table::Constant::Tag>(constantEntityId);\
        CreateComponent<ECS::File::Table::Name>(constantEntityId, constantName);\
        CreateComponent<Constant::TypeName>(constantEntityId, type.cast<std::string>().value());\
        CreateComponent<Constant::Value>(constantEntityId, value.cast<std::string>().value());\
        return  constantEntityId;       \
    }(luaRef, name);

}
