#pragma once 

#include <RAL.Driver.hpp>
#include <Render/Pipeline/auto_OksEngine.Render.Pipeline.hpp>
#include <Render/Pipeline/OksEngine.Render.Shader.Utils.hpp>

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
	//RAL::Driver::RP::Id
#define RENDER__PIPELINEDESCRIPTION__CREATE_PIPELINE_CREATE_INFO(pipelineDescriptionEntityId, RPId)																							\
	[this](ECS2::Entity::Id entityId, RAL::Driver::RP::Id renderPassId){																													\
		const ECS2::ComponentsFilter pipelineDescriptionCF = GetComponentsFilter(entityId);																									\
	RAL::Driver::Pipeline::CI2 pipeline;																																					\
	{																																														\
		std::vector<RAL::Driver::PushConstant> pushConstants;																																\
		std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;																													\
																																															\
		auto addBindings = [&pushConstants, &shaderBindings, this](ECS2::Entity::Id shaderEntityId) {																						\
																																															\
			const ECS2::ComponentsFilter cf = GetComponentsFilter(shaderEntityId);																											\
			const RAL::Driver::Shader::Stage shaderStage = RENDER__SHADER__GET_SHADER_STAGE(shaderEntityId);																				\
			if (cf.IsSet<Render::Shader::PushConstants>()) {																																\
				const auto* ecsPushConstants = GetComponent<Render::Shader::PushConstants>(shaderEntityId);																					\
				for (auto ecsPushConstant : ecsPushConstants->constants_) {																													\
					pushConstants.emplace_back(																																				\
						shaderStage,																																						\
						ecsPushConstant.offset_,																																			\
						ecsPushConstant.size_																																				\
					);																																										\
				}																																											\
			}																																												\
																																															\
			if (cf.IsSet<Render::Shader::Bindings>()) {																																		\
				const auto* ecsBindings = GetComponent<Render::Shader::Bindings>(shaderEntityId);																							\
				for (auto ecsBinding : ecsBindings->bindings_) {																															\
					shaderBindings.push_back(RAL::Driver::Shader::Binding::Layout(																											\
						ecsBinding.name_,																																					\
						ecsBinding.set_,																																					\
						(Common::UInt32)ecsBinding.binding_,																																\
						Render::Shader::ToRALType(ecsBinding.resourceType_),																												\
						shaderStage																																							\
					));																																										\
				}																																											\
			}																																												\
		};																																													\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::GeometryShader::EntityId>()) {																							\
			const ECS2::Entity::Id shaderEntityId = GetComponent<Render::PipelineDescription::GeometryShader::EntityId>(entityId)->id_;														\
			const std::string shaderName = GetComponent<Render::Shader::ResourcePath>(entityId)->resourcePath_;																				\
			const std::vector<Common::UInt32> shaderBinary = GetComponent<Render::Shader::Binary>(shaderEntityId)->data_;																	\
			pipeline.geometryShaderBinary_ = shaderBinary;																																	\
			addBindings(shaderEntityId);																																					\
		}																																													\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::VertexShader::EntityId>()) {																							\
				const ECS2::Entity::Id shaderEntityId = GetComponent<Render::PipelineDescription::VertexShader::EntityId>(entityId)->id_;													\
				const std::string shaderName = GetComponent<Render::Shader::ResourcePath>(shaderEntityId)->resourcePath_;																	\
				const std::vector<Common::UInt32> shaderBinary = GetComponent<Render::Shader::Binary>(shaderEntityId)->data_;																\
				pipeline.vertexShaderBinary_ = shaderBinary;																																\
				addBindings(shaderEntityId);																																				\
		}																																													\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::FragmentShader::EntityId>()) {																							\
			const ECS2::Entity::Id shaderEntityId = GetComponent<Render::PipelineDescription::FragmentShader::EntityId>(entityId)->id_;														\
			const std::string shaderName = GetComponent<Render::Shader::ResourcePath>(shaderEntityId)->resourcePath_;																		\
			const std::vector<Common::UInt32> shaderBinary = GetComponent<Render::Shader::Binary>(shaderEntityId)->data_;																	\
			pipeline.fragmentShaderBinary_ = shaderBinary;																																	\
			addBindings(shaderEntityId);																																					\
		}																																													\
		pipeline.pushConstants_ = pushConstants;																																		\
		pipeline.shaderBindings_ = shaderBindings;																																		\
		pipeline.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList;																											\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::InputAssembler>()) {																								\
				const auto* inputAssembler = GetComponent<Render::PipelineDescription::InputAssembler>(entityId);																		\
				pipeline.vertexType_ = Render::PipelineDescription::ToRALType(inputAssembler->vertexType_);																				\
				pipeline.indexType_ = Render::PipelineDescription::ToRALType(inputAssembler->indexType_);																				\
				pipeline.frontFace_ = Render::PipelineDescription::ToRALType(inputAssembler->frontFace_);																				\
				pipeline.cullMode_ = Render::PipelineDescription::ToRALType(inputAssembler->cullMode_);																					\
		}																																												\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::Multisampling>()) {																								\
				const auto* multisampling = GetComponent<Render::PipelineDescription::Multisampling>(entityId);																			\
				auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();																					\
																																														\
				multisamplingInfo->samplesCount_ = Render::PipelineDescription::ToRALType(multisampling->samplesCount_);																\
				pipeline.multisamplingInfo_ = multisamplingInfo;																																							\
		}																																												\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::DepthTest>()) {																										\
			const auto* depthTest = GetComponent<Render::PipelineDescription::DepthTest>(entityId);																							\
			pipeline.enableDepthTest_ = depthTest->enable_;																																	\
			pipeline.dbCompareOperation_ = Render::PipelineDescription::ToRALType(depthTest->compareOperation_);																			\
																																															\
		}																																													\
		pipeline.renderPassId_ = renderPassId;																																				\
	}																																														\
	return pipeline;																																										\
	}(pipelineDescriptionEntityId, RPId);


#define RENDER__PIPELINEDESCRIPTION__CREATE_PIPELINE_CREATE_INFO2(pipelineDescriptionEntityId, colorAttachmentFormats_, depthAttachmentFormat_, stencilAttachmentFormat_)						\
	[this](ECS2::Entity::Id entityId,																																							\
std::vector<RAL::Driver::Texture::Format> colorAttachmentFormats,																															\
RAL::Driver::Texture::Format  depthAttachmentFormat,																																		\
RAL::Driver::Texture::Format stencilAttachmentFormat){																																		\
		const ECS2::ComponentsFilter pipelineDescriptionCF = GetComponentsFilter(entityId);																									\
	RAL::Driver::Pipeline::CI2 pipeline;																																					\
	{																																														\
		std::vector<RAL::Driver::PushConstant> pushConstants;																																\
		std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;																													\
																																															\
		auto addBindings = [&pushConstants, &shaderBindings, this](ECS2::Entity::Id shaderEntityId) {																						\
																																															\
			const ECS2::ComponentsFilter cf = GetComponentsFilter(shaderEntityId);																											\
			const RAL::Driver::Shader::Stage shaderStage = RENDER__SHADER__GET_SHADER_STAGE(shaderEntityId);																				\
			if (cf.IsSet<Render::Shader::PushConstants>()) {																																\
				const auto* ecsPushConstants = GetComponent<Render::Shader::PushConstants>(shaderEntityId);																					\
				for (auto ecsPushConstant : ecsPushConstants->constants_) {																													\
					pushConstants.emplace_back(																																				\
						shaderStage,																																						\
						ecsPushConstant.offset_,																																			\
						ecsPushConstant.size_																																				\
					);																																										\
				}																																											\
			}																																												\
																																															\
			if (cf.IsSet<Render::Shader::Bindings>()) {																																		\
				const auto* ecsBindings = GetComponent<Render::Shader::Bindings>(shaderEntityId);																							\
				for (auto ecsBinding : ecsBindings->bindings_) {																															\
					shaderBindings.push_back(RAL::Driver::Shader::Binding::Layout(																											\
						ecsBinding.name_,																																					\
						ecsBinding.set_,																																					\
						(Common::UInt32)ecsBinding.binding_,																																\
						Render::Shader::ToRALType(ecsBinding.resourceType_),																												\
						shaderStage																																							\
					));																																										\
				}																																											\
			}																																												\
		};																																													\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::GeometryShader::EntityId>()) {																							\
			const ECS2::Entity::Id shaderEntityId = GetComponent<Render::PipelineDescription::GeometryShader::EntityId>(entityId)->id_;														\
			const std::string shaderName = GetComponent<Render::Shader::ResourcePath>(entityId)->resourcePath_;																				\
			const std::vector<Common::UInt32> shaderBinary = GetComponent<Render::Shader::Binary>(shaderEntityId)->data_;																	\
			pipeline.geometryShaderBinary_ = shaderBinary;																																	\
			addBindings(shaderEntityId);																																					\
		}																																													\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::VertexShader::EntityId>()) {																							\
				const ECS2::Entity::Id shaderEntityId = GetComponent<Render::PipelineDescription::VertexShader::EntityId>(entityId)->id_;													\
				const std::string shaderName = GetComponent<Render::Shader::ResourcePath>(shaderEntityId)->resourcePath_;																	\
				const std::vector<Common::UInt32> shaderBinary = GetComponent<Render::Shader::Binary>(shaderEntityId)->data_;																\
				pipeline.vertexShaderBinary_ = shaderBinary;																																\
				addBindings(shaderEntityId);																																				\
		}																																													\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::FragmentShader::EntityId>()) {																							\
			const ECS2::Entity::Id shaderEntityId = GetComponent<Render::PipelineDescription::FragmentShader::EntityId>(entityId)->id_;														\
			const std::string shaderName = GetComponent<Render::Shader::ResourcePath>(shaderEntityId)->resourcePath_;																		\
			const std::vector<Common::UInt32> shaderBinary = GetComponent<Render::Shader::Binary>(shaderEntityId)->data_;																	\
			pipeline.fragmentShaderBinary_ = shaderBinary;																																	\
			addBindings(shaderEntityId);																																					\
		}																																													\
		pipeline.pushConstants_ = pushConstants;																																		\
		pipeline.shaderBindings_ = shaderBindings;																																		\
		pipeline.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList;																											\
																																												\
																																																\
																																																		\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::InputAssembler>()) {																								\
				const auto* inputAssembler = GetComponent<Render::PipelineDescription::InputAssembler>(entityId);																		\
				pipeline.vertexType_ = Render::PipelineDescription::ToRALType(inputAssembler->vertexType_);																				\
				pipeline.indexType_ = Render::PipelineDescription::ToRALType(inputAssembler->indexType_);																				\
				pipeline.frontFace_ = Render::PipelineDescription::ToRALType(inputAssembler->frontFace_);																				\
				pipeline.cullMode_ = Render::PipelineDescription::ToRALType(inputAssembler->cullMode_);																					\
		}																																												\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::Multisampling>()) {																								\
				const auto* multisampling = GetComponent<Render::PipelineDescription::Multisampling>(entityId);																			\
				auto multisamplingInfo = std::make_shared<RAL::Driver::Pipeline::MultisamplingInfo>();																					\
																																														\
				multisamplingInfo->samplesCount_ = Render::PipelineDescription::ToRALType(multisampling->samplesCount_);																\
				pipeline.multisamplingInfo_ = multisamplingInfo;																														\
		}																																												\
		if (pipelineDescriptionCF.IsSet<Render::PipelineDescription::DepthTest>()) {																										\
			const auto* depthTest = GetComponent<Render::PipelineDescription::DepthTest>(entityId);																							\
			pipeline.enableDepthTest_ = depthTest->enable_;																																	\
			pipeline.dbCompareOperation_ = Render::PipelineDescription::ToRALType(depthTest->compareOperation_);																			\
																																															\
		}																																													\
																																															\
		pipeline.attachmentsInfo_ = std::make_shared<RAL::Driver::Pipeline::AttachmentsInfo>(																								\
			colorAttachmentFormats,																																							\
			depthAttachmentFormat,																																							\
			stencilAttachmentFormat																																							\
		);																																													\
																																															\
																																															\
	}																																														\
	return pipeline;																																										\
	}(pipelineDescriptionEntityId, colorAttachmentFormats_, depthAttachmentFormat_, stencilAttachmentFormat_);


	inline RAL::Driver::Pipeline::DepthBuffer::CompareOperation ToRALType(DepthBufferCompareOperation op) {
		switch (op) {
		case DepthBufferCompareOperation::Never:          return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Never;
		case DepthBufferCompareOperation::Less:           return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less;
		case DepthBufferCompareOperation::Equal:          return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Equal;
		case DepthBufferCompareOperation::LessOrEqual:    return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::LessOrEqual;
		case DepthBufferCompareOperation::Greater:        return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Greater;
		case DepthBufferCompareOperation::NotEqual:       return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::NotEqual;
		case DepthBufferCompareOperation::GreaterOrEqual: return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::GreaterOrEqual;
		case DepthBufferCompareOperation::Always:         return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always;
		}
		NOT_IMPLEMENTED();
		return RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Undefined;
	}

	inline RAL::Driver::VertexType ToRALType(VertexType type) {
		switch (type) {
		case VertexType::VF3:                           return RAL::Driver::VertexType::VF3;
		case VertexType::VF3_NF3_TF2:                    return RAL::Driver::VertexType::VF3_NF3_TF2;
		case VertexType::VF3_NF3_TF2_BIDUB4_WUB4:        return RAL::Driver::VertexType::VF3_NF3_TF2_BIDUB4_WUB4;
		case VertexType::VF3_CF3:                         return RAL::Driver::VertexType::VF3_CF3;
		case VertexType::VF3_NF3_CF4:                     return RAL::Driver::VertexType::VF3_NF3_CF4;
		case VertexType::VF2_TF2_CF3:                     return RAL::Driver::VertexType::VF2_TF2_CF3;
		case VertexType::Undefined:                       return RAL::Driver::VertexType::Undefined;
		}
		NOT_IMPLEMENTED();
		return RAL::Driver::VertexType::Undefined;
	}

	inline RAL::Driver::FrontFace ToRALType(FrontFace face) {
		switch (face) {
		case FrontFace::Clockwise:        return RAL::Driver::FrontFace::Clockwise;
		case FrontFace::CounterClockwise: return RAL::Driver::FrontFace::CounterClockwise;
		}
		NOT_IMPLEMENTED();
		return RAL::Driver::FrontFace::Undefined;
	}

	inline RAL::Driver::IndexType ToRALType(IndexType type) {
		switch (type) {
		case IndexType::UI16: return RAL::Driver::IndexType::UI16;
		case IndexType::UI32: return RAL::Driver::IndexType::UI32;
		}
		NOT_IMPLEMENTED();
		return RAL::Driver::IndexType::Undefined;
	}

	inline RAL::Driver::CullMode ToRALType(CullMode mode) {
		switch (mode) {
		case CullMode::None:          return RAL::Driver::None;
		case CullMode::Front:         return RAL::Driver::Front;
		case CullMode::Back:          return RAL::Driver::Back;
		case CullMode::FrontAndBack:  return RAL::Driver::FrontAndBack;
		}
		NOT_IMPLEMENTED();
		return RAL::Driver::Undefined;
	}

	inline RAL::Driver::SamplesCount ToRALType(SamplesCount samplesCount) {
		switch (samplesCount) {
		case SamplesCount::SamplesCount_1:		return RAL::Driver::SamplesCount::SamplesCount_1;
		case SamplesCount::SamplesCount_2:		return RAL::Driver::SamplesCount::SamplesCount_2;
		case SamplesCount::SamplesCount_4:		return RAL::Driver::SamplesCount::SamplesCount_4;
		case SamplesCount::SamplesCount_8:		return RAL::Driver::SamplesCount::SamplesCount_8;
		case SamplesCount::SamplesCount_16:		return RAL::Driver::SamplesCount::SamplesCount_16;
		case SamplesCount::SamplesCount_32:		return RAL::Driver::SamplesCount::SamplesCount_32;
		case SamplesCount::SamplesCount_64:		return RAL::Driver::SamplesCount::SamplesCount_64;
		}
		NOT_IMPLEMENTED();
		return RAL::Driver::SamplesCount::Undefined;
	}

}

