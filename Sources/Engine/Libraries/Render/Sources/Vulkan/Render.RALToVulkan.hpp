#pragma once 

#include <RAL.Driver.hpp>
#include <vulkan/vulkan.hpp>

namespace Render::Vulkan {

	inline VkCullModeFlags			ToVulkanType(RAL::Driver::CullMode cullMode) {
		switch (cullMode) {
		case RAL::Driver::CullMode::None: {
			return VkCullModeFlagBits::VK_CULL_MODE_NONE;
			break;
		}
		case RAL::Driver::CullMode::Back: {
			return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
			break;
		}
		case RAL::Driver::CullMode::Front: {
			return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
			break;
		}
		case RAL::Driver::CullMode::FrontAndBack: {
			return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid CullMode value used.");
			return VkCullModeFlagBits::VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
		};
	}

	inline VkPrimitiveTopology		ToVulkanType(RAL::Driver::Pipeline::Topology topologyType) {
		switch (topologyType) {
		case RAL::Driver::Pipeline::Topology::LineList: {
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			break;
		}
		case RAL::Driver::Pipeline::Topology::TriangleList: {
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid TopologyType value used.");
			return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
		};
	}

	inline VkFrontFace				ToVulkanType(RAL::Driver::FrontFace frontFace) {
		switch (frontFace) {
		case RAL::Driver::FrontFace::Clockwise: {
			return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
			break;
		}
		case RAL::Driver::FrontFace::CounterClockwise: {
			return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid FrontFace value used.");
			return VkFrontFace::VK_FRONT_FACE_MAX_ENUM;
		};
	}

	inline VkCompareOp				ToVulkanType(RAL::Driver::Pipeline::DepthBuffer::CompareOperation co) {
		switch (co) {
		case RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Less: {
			return VkCompareOp::VK_COMPARE_OP_LESS;
			break;
		}
		case RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always: {
			return VkCompareOp::VK_COMPARE_OP_ALWAYS;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid Depth buffer compare operation value used.");
			return VkCompareOp::VK_COMPARE_OP_MAX_ENUM;
		};
	}

	inline VkDescriptorType			ToVulkanType(RAL::Driver::Shader::Binding::Type bindingType) {
		switch (bindingType) {
		case RAL::Driver::Shader::Binding::Type::Sampler: {
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		}
		case RAL::Driver::Shader::Binding::Type::Uniform: {
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		}
		case RAL::Driver::Shader::Binding::Type::InputAttachment: {
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			break;
		}
		case RAL::Driver::Shader::Binding::Type::Storage: {
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid ShaderBinding::Type value used.");
			return VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM;
		};
	}

	inline VkShaderStageFlags	ToVulkanType(RAL::Driver::Shader::Stage stage) {
		switch (stage) {
		case RAL::Driver::Shader::Stage::VertexShader: {
			return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
			break;
		}
		case RAL::Driver::Shader::Stage::FragmentShader: {
			return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		}
		case RAL::Driver::Shader::Stage::ComputeShader: {
			return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid ShaderBinding::Stage value used.");
			return VkShaderStageFlagBits::VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		};
	}

	inline VkAttachmentLoadOp		ToVulkanType(RAL::Driver::RP::AttachmentUsage::LoadOperation loadOperation) {
		switch (loadOperation) {
		case RAL::Driver::RP::AttachmentUsage::LoadOperation::Load: {
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
			break;
		}
		case RAL::Driver::RP::AttachmentUsage::LoadOperation::Clear: {
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
			break;
		}
		case RAL::Driver::RP::AttachmentUsage::LoadOperation::Ignore: {
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid ShaderBinding::Stage value used.");
			return VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
		};
	}

	inline VkAttachmentStoreOp		ToVulkanType(RAL::Driver::RP::AttachmentUsage::StoreOperation storeOperation) {
		switch (storeOperation) {
		case RAL::Driver::RP::AttachmentUsage::StoreOperation::Store: {
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
			break;
		}
		case RAL::Driver::RP::AttachmentUsage::StoreOperation::DontStore: {
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid ShaderBinding::Stage value used.");
			return VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_MAX_ENUM;
		};
	}

	inline VkPipelineStageFlags		ToVulkanType(RAL::Driver::Pipeline::Stage stage) {
		switch (stage) {
		case RAL::Driver::Pipeline::Stage::Top:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		case RAL::Driver::Pipeline::Stage::DrawIndirect:
			return VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		case RAL::Driver::Pipeline::Stage::VertexInput:
			return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		case RAL::Driver::Pipeline::Stage::VertexShader:
			return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		case RAL::Driver::Pipeline::Stage::TessellationControlShader:
			return VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		case RAL::Driver::Pipeline::Stage::TessellationEvaluationShader:
			return VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		case RAL::Driver::Pipeline::Stage::GeometryShader:
			return VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		case RAL::Driver::Pipeline::Stage::FragmentShader:
			return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		case RAL::Driver::Pipeline::Stage::EarlyFragmentTests:
			return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		case RAL::Driver::Pipeline::Stage::LateFragmentTests:
			return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		case RAL::Driver::Pipeline::Stage::ColorAttachmentOutput:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		case RAL::Driver::Pipeline::Stage::ComputeShader:
			return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		case RAL::Driver::Pipeline::Stage::Transfer:
			return VK_PIPELINE_STAGE_TRANSFER_BIT;
		case RAL::Driver::Pipeline::Stage::Bottom:
			return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		case RAL::Driver::Pipeline::Stage::Undefined:
		default:

#pragma region Assert
			OS::AssertFail();
#pragma endregion

			return VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM; // или 0, если предпочитаете
		}
	}

	inline VkImageLayout ToVulkanType(RAL::Driver::Texture::State state) {
		switch (state) {
		case RAL::Driver::Texture::State::DataIsUndefined: {
			return VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
			break;
		}
		case RAL::Driver::Texture::State::DataForAllOperations: {
			return VkImageLayout::VK_IMAGE_LAYOUT_GENERAL;
			break;
		}
		case RAL::Driver::Texture::State::DataForColorWrite: {
			return VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			break;
		}
		case RAL::Driver::Texture::State::DataForShaderRead: {
			return VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			break;
		}
		case RAL::Driver::Texture::State::DataForCopyingSource: {
			return VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			break;
		}
		case RAL::Driver::Texture::State::DataForCopyingDestination: {
			return VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			break;
		}
		case RAL::Driver::Texture::State::DataForPresentation: {
			return VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			break;
		}
		case RAL::Driver::Texture::State::DataForDepthStencilWrite: {
			return VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			break;
		}
		default:
			ASSERT_FAIL_MSG("Invalid ShaderBinding::Stage value used.");
			return  VkImageLayout::VK_IMAGE_LAYOUT_MAX_ENUM;
		};
	}

	inline VkFormat					ToVulkanType(RAL::Driver::Texture::Format format) {
		switch (format) {
		case RAL::Driver::Texture::Format::RGBA_32_UNORM: {
			return VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
			break;
		}
		case RAL::Driver::Texture::Format::RGBA_32_UINT: {
			return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
			break;
		}
		case RAL::Driver::Texture::Format::BGRA_32_UINT: {
			return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
			break;
		}
		case RAL::Driver::Texture::Format::BGRA_32_UNORM: {
			return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
			break;
		}
		case RAL::Driver::Texture::Format::D_32_SFLOAT: {
			return VkFormat::VK_FORMAT_D32_SFLOAT;
		}
		default:
			ASSERT_FAIL_MSG("Invalid ShaderBinding::Stage value used.");
			return VkFormat::VK_FORMAT_MAX_ENUM;
		};
	}

	inline VkImageUsageFlags		ToVulkanType(RAL::Driver::Texture::Usage usage) {

		switch (usage) {
		case RAL::Driver::Texture::Usage::ColorAttachment: {
			return VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			break;
		}
		case RAL::Driver::Texture::Usage::DepthStencilAttachment: {
			return VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		case RAL::Driver::Texture::Usage::TransferDestination: {
			return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			break;
		}
		case RAL::Driver::Texture::Usage::TransferSource: {
			return VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		case RAL::Driver::Texture::Usage::Sampled: {
			return VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		default:
			ASSERT_FAIL_MSG("Invalid ShaderBinding::Stage value used.");
			return VkImageUsageFlagBits::VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
		};
	}

	inline VkAccessFlags			ToVulkanType(RAL::Driver::Texture::Access access) {
		switch (access) {
		case RAL::Driver::Texture::Access::ColorRead:
			return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		case RAL::Driver::Texture::Access::ColorWrite:
			return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		case RAL::Driver::Texture::Access::DepthStencilRead:
			return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		case RAL::Driver::Texture::Access::DepthStencilWrite:
			return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		case RAL::Driver::Texture::Access::UniformRead:
			return VK_ACCESS_UNIFORM_READ_BIT;
		case RAL::Driver::Texture::Access::InputAttachmentRead:
			return VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		case RAL::Driver::Texture::Access::ShaderRead:
			return VK_ACCESS_SHADER_READ_BIT;
		case RAL::Driver::Texture::Access::ShaderWrite:
			return VK_ACCESS_SHADER_WRITE_BIT;
		default:

#pragma region Assert
			OS::AssertFail();
#pragma endregion
			return VK_ACCESS_FLAG_BITS_MAX_ENUM;
		}
	}


}