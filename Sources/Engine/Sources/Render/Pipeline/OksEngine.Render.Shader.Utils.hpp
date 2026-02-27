#pragma once 

#include <Render/Pipeline/auto_OksEngine.Render.Shader.hpp>

#include <RAL.Driver.hpp>

#define RENDER__SHADER__GET_SHADER_TYPE(shaderEntityId)															\
	[this](ECS2::Entity::Id entityId) {																			\
		const ECS2::ComponentsFilter cf = GetComponentsFilter(entityId);										\
	if (cf.IsSet<Render::Shader::Type::GeometryShader>()) {														\
		return RAL::Driver::Shader::Type::Geometry;																\
	}																											\
	else if (cf.IsSet<Render::Shader::Type::VertexShader>()) {													\
		return RAL::Driver::Shader::Type::Vertex;																\
	}																											\
	else if (cf.IsSet<Render::Shader::Type::FragmentShader>()) {												\
		return RAL::Driver::Shader::Type::Fragment;																\
	}																											\
	else {																										\
		ASSERT_FAIL_MSG("Shader has unknown type. Maybe added new Shader::Type, need to update this macros.");	\
		return RAL::Driver::Shader::Type::Undefined;															\
	}																											\
	}(shaderEntityId);


namespace OksEngine::Render::Shader {

}