#pragma once 

#include <Render/Pipeline/auto_OksEngine.Render.Shader.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine::Render::Shader {

	inline RAL::Driver::Shader::Binding::Type ToRALType(ResourceType type) {
		switch (type) {
		case ResourceType::Uniform:          return RAL::Driver::Shader::Binding::Type::Uniform;
		case ResourceType::Storage:          return RAL::Driver::Shader::Binding::Type::Storage;
		case ResourceType::Sampler:          return RAL::Driver::Shader::Binding::Type::Sampler;
		case ResourceType::InputAttachment:  return RAL::Driver::Shader::Binding::Type::InputAttachment;
		}
		NOT_IMPLEMENTED();
		return RAL::Driver::Shader::Binding::Type::Undefined;
	}

	//RAL::Driver::Shader::Stage ToRALType(Stage stage) {
	//	switch (stage) {
	//	case Stage::GeometryShader:   return RAL::Driver::Shader::Stage::GeometryShader;
	//	case Stage::VertexShader:   return RAL::Driver::Shader::Stage::VertexShader;
	//	case Stage::FragmentShader: return RAL::Driver::Shader::Stage::FragmentShader;
	//	case Stage::ComputeShader:  return RAL::Driver::Shader::Stage::ComputeShader;
	//	default:
	//		NOT_IMPLEMENTED();
	//		return RAL::Driver::Shader::Stage::Undefined;
	//	};
	//}

}

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
	}(shaderEntityId)

#define RENDER__SHADER__GET_SHADER_STAGE(shaderEntityId)														\
	[this](ECS2::Entity::Id entityId) {																			\
		const ECS2::ComponentsFilter cf = GetComponentsFilter(entityId);										\
	if (cf.IsSet<Render::Shader::Type::GeometryShader>()) {														\
		return RAL::Driver::Shader::Stage::GeometryShader;														\
	}																											\
	else if (cf.IsSet<Render::Shader::Type::VertexShader>()) {													\
		return RAL::Driver::Shader::Stage::VertexShader;														\
	}																											\
	else if (cf.IsSet<Render::Shader::Type::FragmentShader>()) {												\
		return RAL::Driver::Shader::Stage::FragmentShader;														\
	}																											\
	else {																										\
		ASSERT_FAIL_MSG("Shader has unknown type. Maybe added new Shader::Type, need to update this macros.");	\
		return RAL::Driver::Shader::Stage::Undefined;															\
	}																											\
	}(shaderEntityId)

#define RENDER__SHADER__IS_SHADER_READY(shaderEntityId)															\
	[this](ECS2::Entity::Id entityId) {																			\
		const ECS2::ComponentsFilter cf = GetComponentsFilter(entityId);										\
		ASSERT(cf.IsSet<Shader::Tag>());																		\
		return cf.IsSet<Shader::Ready>();																		\
	}(shaderEntityId)																					

namespace OksEngine::Render::Shader {

}