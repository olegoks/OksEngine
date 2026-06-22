#include <auto_OksEngine.ECS.Component.Utils.hpp>


#define COMPONENT_TYPE_COMMA(type) type,
#define COMPONENT_TYPES_LIST() FOR_EACH_COMPONENT(COMPONENT_TYPE_COMMA)
