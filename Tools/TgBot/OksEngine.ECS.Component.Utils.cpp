
#include <OksEngine.ECS.Component.Utils.hpp>

#define COMPONENT_TYPE_ID_TO_FULL_NAME_LAST(type)\
    {type::GetTypeId(), type::GetFullName()}

#define COMPONENT_TYPE_ID_TO_FULL_NAME(type)\
    COMPONENT_TYPE_ID_TO_FULL_NAME_LAST(type),

#define COMPONENT_FULL_NAME_TO_TYPE_ID__LAST(type)\
    {type::GetFullName(), type::GetTypeId()}

#define COMPONENT_FULL_NAME_TO_TYPE_ID(type)\
    COMPONENT_FULL_NAME_TO_TYPE_ID__LAST(type),

//void BindComponents(lua_State* state);
void BindWorld(lua_State* state);


#define SERIALIZE_COMPONENT(type)\
    if(IsSerializable<type>()) {\
        serializedEntity +=  "\t\t" + Serialize<type>(GetComponent<type>(entityId));\
    }

#define SERIALIZE_COMPONENT_COMMA(type)\
    SERIALIZE_COMPONENT(type);\
    if (IsSerializable<type>()) {\
        serializedEntity += ",\n"; \
    }

#define SERIALIZE_ENTITY(entity)\
    [this](ECS2::Entity::Id entityId){\
        const ECS2::ComponentsFilter components = GetEntityComponentsFilter(entityId);\
        std::string serializedEntity;\
        serializedEntity += "\t\tID = " + std::to_string(entityId) + ",\n";\
        FOR_EACH_COMPONENT(SERIALIZE_COMPONENT_COMMA, SERIALIZE_COMPONENT);\
        serializedEntity;\
    }(entity)

#define PARSE_COMPONENT(type)\
    {\
        luabridge::LuaRef ref = luaEntity[GET_COMPONENT_NAME(type)];\
        if (!ref.isNil())\
        {\
            world->CreateComponent<type>(id, type::Parse(ref));\
        }\
    }

//Entity must have ID = {id},   id - new entity id created  with CreateEntity()
// Need to preproces scene  file.
#define PARSE_ENTITY(entity)\
 [this](luabridge::LuaRef luaEntity){\
       const auto id = luaEntity["ID"].cast<ECS2::Entity::Id::ValueType>().value();\
       FOR_EACH_COMPONENT(PARSE_COMPONENT, PARSE_COMPONENT);\
    }(entity)

std::string GetFullNameByComponentTypeId(ECS2::ComponentTypeId componentTypeId) {
    static std::unordered_map<ECS2::ComponentTypeId, std::string> componentTypeIdToFullName{
        FOR_EACH_COMPONENT(COMPONENT_TYPE_ID_TO_FULL_NAME, COMPONENT_TYPE_ID_TO_FULL_NAME_LAST)
    };
    return componentTypeIdToFullName[componentTypeId];
};

ECS2::ComponentTypeId GetComponentTypeIdByFullName(const char* componentName) {
    static std::unordered_map<std::string, ECS2::ComponentTypeId> fullNameToComponentTypeId{
        FOR_EACH_COMPONENT(COMPONENT_FULL_NAME_TO_TYPE_ID, COMPONENT_FULL_NAME_TO_TYPE_ID__LAST)
    };
    return fullNameToComponentTypeId[componentName];
};