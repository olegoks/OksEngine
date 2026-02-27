#pragma once 

#include <RAL.Driver.hpp>
#include <Render/Pipeline/auto_OksEngine.Render.Pipeline.hpp>


namespace OksEngine::Render::PipelineDescription {

	//RAL::Driver::Shader::Binding::Type ToRALType(ResourceType type) {
	//	switch (type) {
	//	case ResourceType::Uniform:          return RAL::Driver::Shader::Binding::Type::Uniform;
	//	case ResourceType::Storage:          return RAL::Driver::Shader::Binding::Type::Storage;
	//	case ResourceType::Sampler:          return RAL::Driver::Shader::Binding::Type::Sampler;
	//	case ResourceType::InputAttachment:  return RAL::Driver::Shader::Binding::Type::InputAttachment;
	//	}
	//	NOT_IMPLEMENTED();
	//	return RAL::Driver::Shader::Binding::Type::Undefined;
	//}

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

	//RAL::Driver::Pipeline::DepthBuffer::CompareOperation ToRALType(DepthBufferCompareOperation op) {
	//	switch (op) {
	//	case DepthBufferCompareOperation::Never:          return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Never;
	//	case DepthBufferCompareOperation::Less:           return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less;
	//	case DepthBufferCompareOperation::Equal:          return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Equal;
	//	case DepthBufferCompareOperation::LessOrEqual:    return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::LessOrEqual;
	//	case DepthBufferCompareOperation::Greater:        return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Greater;
	//	case DepthBufferCompareOperation::NotEqual:       return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::NotEqual;
	//	case DepthBufferCompareOperation::GreaterOrEqual: return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::GreaterOrEqual;
	//	case DepthBufferCompareOperation::Always:         return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always;
	//	}
	//	NOT_IMPLEMENTED();
	//	return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Undefined;
	//}

	//RAL::Driver::VertexType ToRALType(VertexType type) {
	//	switch (type) {
	//	case VertexType::VF3:                           return RAL::Driver::VertexType::VF3;
	//	case VertexType::VF3_NF3_TF2:                    return RAL::Driver::VertexType::VF3_NF3_TF2;
	//	case VertexType::VF3_NF3_TF2_BIDUB4_WUB4:        return RAL::Driver::VertexType::VF3_NF3_TF2_BIDUB4_WUB4;
	//	case VertexType::VF3_CF3:                         return RAL::Driver::VertexType::VF3_CF3;
	//	case VertexType::VF3_NF3_CF4:                     return RAL::Driver::VertexType::VF3_NF3_CF4;
	//	case VertexType::VF2_TF2_CF3:                     return RAL::Driver::VertexType::VF2_TF2_CF3;
	//	case VertexType::Undefined:                       return RAL::Driver::VertexType::Undefined;
	//	}
	//	NOT_IMPLEMENTED();
	//	return RAL::Driver::VertexType::Undefined;
	//}

	//RAL::Driver::FrontFace ToRALType(FrontFace face) {
	//	switch (face) {
	//	case FrontFace::Clockwise:        return RAL::Driver::FrontFace::Clockwise;
	//	case FrontFace::CounterClockwise: return RAL::Driver::FrontFace::CounterClockwise;
	//	}
	//	NOT_IMPLEMENTED();
	//	return RAL::Driver::FrontFace::Undefined;
	//}

	//RAL::Driver::IndexType ToRALType(IndexType type) {
	//	switch (type) {
	//	case IndexType::UI16: return RAL::Driver::IndexType::UI16;
	//	case IndexType::UI32: return RAL::Driver::IndexType::UI32;
	//	}
	//	NOT_IMPLEMENTED();
	//	return RAL::Driver::IndexType::Undefined;
	//}

	//RAL::Driver::CullMode ToRALType(CullMode mode) {
	//	switch (mode) {
	//	case CullMode::None:          return RAL::Driver::None;
	//	case CullMode::Front:         return RAL::Driver::Front;
	//	case CullMode::Back:          return RAL::Driver::Back;
	//	case CullMode::FrontAndBack:  return RAL::Driver::FrontAndBack;
	//	}
	//	NOT_IMPLEMENTED();
	//	return RAL::Driver::Undefined;
	//}

	//RAL::Driver::SamplesCount ToRALType(SamplesCount samplesCount) {
	//	switch (samplesCount) {
	//		case SamplesCount::SamplesCount_1:		return RAL::Driver::SamplesCount::SamplesCount_1;
	//		case SamplesCount::SamplesCount_2:		return RAL::Driver::SamplesCount::SamplesCount_2;
	//		case SamplesCount::SamplesCount_4:		return RAL::Driver::SamplesCount::SamplesCount_4;
	//		case SamplesCount::SamplesCount_8:		return RAL::Driver::SamplesCount::SamplesCount_8;
	//		case SamplesCount::SamplesCount_16:		return RAL::Driver::SamplesCount::SamplesCount_16;
	//		case SamplesCount::SamplesCount_32:		return RAL::Driver::SamplesCount::SamplesCount_32;
	//		case SamplesCount::SamplesCount_64:		return RAL::Driver::SamplesCount::SamplesCount_64;
	//	}
	//	NOT_IMPLEMENTED();
	//	return RAL::Driver::SamplesCount::Undefined;
	//}

}

