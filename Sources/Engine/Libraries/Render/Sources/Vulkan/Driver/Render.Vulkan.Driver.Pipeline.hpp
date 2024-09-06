#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <RAL.Common.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.SwapChain.hpp>
#include <Render.Vulkan.Driver.ShaderModule.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>
#include <Render.Vulkan.Driver.RenderPass.hpp>
#include <Render.Vulkan.Driver.PipelineLayout.hpp>
#include <Render.Vulkan.Shader.hpp>
#include <Render.Vulkan.Driver.Image.hpp>

#include "Render.Vulkan.Driver.RenderPass.hpp"

namespace Render::Vulkan {

	class Pipeline : public Abstraction<VkPipeline> {
	public:

		struct DepthTestInfo {
			VkFormat bufferFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
			VkCompareOp compareOperation_ = VK_COMPARE_OP_LESS;
		};

		struct VertexInfo {
			VkVertexInputBindingDescription bindingDescription_;
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions_;
		};

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<RenderPass> renderPass_ = nullptr;
			std::vector<VkPushConstantRange> pushConstants_;
			std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts_;

			std::shared_ptr<ShaderModule> vertexShader_ = nullptr;
			std::shared_ptr<ShaderModule> fragmentShader_ = nullptr;
			std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
			glm::u32vec2 colorAttachmentSize_ = { 0, 0 };
			VkFormat colorAttachmentFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits multisamplingSamplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			VertexInfo vertexInfo_;
			VkPrimitiveTopology topology_ = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
			VkFrontFace frontFace_ = VK_FRONT_FACE_MAX_ENUM;
			VkCullModeFlags cullMode_ = VK_CULL_MODE_BACK_BIT;
			std::vector<VkDynamicState> dynamicStates_;
		};

		Pipeline(const CreateInfo& createInfo) : 
			createInfo_{ createInfo } {

			VkPipelineDepthStencilStateCreateInfo depthStencilState{};
			{
				depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				const bool enableDepthTest = (createInfo.depthTestInfo_ != nullptr);
				if (enableDepthTest) {
					depthStencilState.depthTestEnable = VK_TRUE;
					depthStencilState.depthWriteEnable = VK_TRUE;
					depthStencilState.depthCompareOp =
						(createInfo.depthTestInfo_ != nullptr)
						? (createInfo.depthTestInfo_->compareOperation_)
						: (VK_COMPARE_OP_LESS);
					depthStencilState.depthBoundsTestEnable = VK_FALSE;
					depthStencilState.minDepthBounds = 0.0f;
					depthStencilState.maxDepthBounds = 1.0f;
					depthStencilState.stencilTestEnable = VK_FALSE;
					depthStencilState.front = {};
					depthStencilState.back = {};
				}
			}

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			{
				vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertShaderStageInfo.module = *createInfo.vertexShader_;
				vertShaderStageInfo.pName = "main";
			}

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			{
				fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragShaderStageInfo.module = *createInfo.fragmentShader_;
				fragShaderStageInfo.pName = "main";
			}

			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			const auto bindingDescription = createInfo.vertexInfo_.bindingDescription_;
			const auto attributeDescriptions = createInfo.vertexInfo_.attributeDescriptions_;

			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			{
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

				vertexInputInfo.vertexBindingDescriptionCount = 1;
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
				vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			}
			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			{
				inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssembly.topology = createInfo.topology_;//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssembly.primitiveRestartEnable = VK_FALSE;
			}
			const glm::u32vec2 size = createInfo.colorAttachmentSize_;

			VkViewport viewport{};
			{
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)size.x;
				viewport.height = (float)size.y;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
			}
			VkRect2D scissor{};
			{
				scissor.offset = { 0, 0 };
				scissor.extent = { size.x, size.y };
			}
			VkPipelineViewportStateCreateInfo viewportState{};
			{
				viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportState.viewportCount = 1;
				viewportState.pViewports = &viewport;
				viewportState.scissorCount = 1;
				viewportState.pScissors = &scissor;
			}
			VkPipelineRasterizationStateCreateInfo rasterizer{};
			{
				rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizer.depthClampEnable = VK_FALSE;
				rasterizer.rasterizerDiscardEnable = VK_FALSE;
				rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
				rasterizer.cullMode = createInfo.cullMode_;// VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT;
				rasterizer.lineWidth = 1.f;
				rasterizer.frontFace = createInfo.frontFace_;//VK_FRONT_FACE_COUNTER_CLOCKWISE; VK_FRONT_FACE_CLOCKWISE;
				rasterizer.depthBiasEnable = VK_FALSE;
				rasterizer.depthBiasConstantFactor = 0.0f;
				rasterizer.depthBiasClamp = 0.0f;
				rasterizer.depthBiasSlopeFactor = 0.0f;
			}
			VkPipelineMultisampleStateCreateInfo multisampling{};
			{
				multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampling.sampleShadingEnable = VK_FALSE;
				multisampling.rasterizationSamples = createInfo.multisamplingSamplesCount_;
				multisampling.minSampleShading = 1.0f;
				multisampling.pSampleMask = nullptr;
				multisampling.alphaToCoverageEnable = VK_FALSE;
				multisampling.alphaToOneEnable = VK_FALSE;
			}
			VkPipelineColorBlendAttachmentState colorBlendAttachment{};
			{
				colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
				colorBlendAttachment.blendEnable = VK_TRUE;
				colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
				colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
				colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;//VK_BLEND_FACTOR_ZERO;
				colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
				//
				colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			}
			VkPipelineColorBlendStateCreateInfo colorBlending{};
			{
				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
				colorBlending.attachmentCount = 1;
				colorBlending.pAttachments = &colorBlendAttachment;
				colorBlending.blendConstants[0] = 0.f;
				colorBlending.blendConstants[1] = 0.f;
				colorBlending.blendConstants[2] = 0.f;
				colorBlending.blendConstants[3] = 0.f;
			}

			PipelineLayout::CreateInfo pipelineLayoutCreateInfo;
			{
				pipelineLayoutCreateInfo.logicDevice_ = createInfo.logicDevice_;
				pipelineLayoutCreateInfo.pushConstants_ = createInfo.pushConstants_;
				pipelineLayoutCreateInfo.descriptorSetLayouts_.insert(
					pipelineLayoutCreateInfo.descriptorSetLayouts_.begin(),
					createInfo.descriptorSetLayouts_.begin(),
					createInfo.descriptorSetLayouts_.end());
				pipelineLayout_ = std::make_shared<PipelineLayout>(pipelineLayoutCreateInfo);
			}

			VkPipelineDynamicStateCreateInfo dynamicState = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
				.dynamicStateCount = static_cast<Common::UInt32>(createInfo.dynamicStates_.size()),
				.pDynamicStates = createInfo.dynamicStates_.data()
			};

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			{
				pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineInfo.stageCount = 2;
				pipelineInfo.pStages = shaderStages;
				pipelineInfo.pVertexInputState = &vertexInputInfo;
				pipelineInfo.pInputAssemblyState = &inputAssembly;
				pipelineInfo.pViewportState = &viewportState;
				pipelineInfo.pRasterizationState = &rasterizer;
				pipelineInfo.pMultisampleState = &multisampling;
				pipelineInfo.pDepthStencilState = &depthStencilState;
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.pDynamicState = &dynamicState;
				pipelineInfo.layout = *pipelineLayout_;
				pipelineInfo.renderPass = *createInfo_.renderPass_;
				pipelineInfo.subpass = 0;
				pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
				pipelineInfo.basePipelineIndex = -1;
			}
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkCall(vkCreateGraphicsPipelines(
				createInfo.logicDevice_->GetHandle(),
				VK_NULL_HANDLE,
				1,
				&pipelineInfo,
				nullptr,
				&pipeline),
				"Error while creating graphics pipeline.");
			SetHandle(pipeline);
			OS::LogInfo("/render/vulkan/driver/", "Graphics pipeline was created successful.");
		}

		~Pipeline() noexcept {
			Destroy();
		}

		[[nodiscard]]
		std::shared_ptr<PipelineLayout> GetLayout() const noexcept { return pipelineLayout_; }

	private:

		void Destroy() noexcept {
			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkPipelineLayout.");
			vkDestroyPipeline(createInfo_.logicDevice_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		CreateInfo createInfo_{};
		std::shared_ptr<PipelineLayout> pipelineLayout_ = VK_NULL_HANDLE;
	};

	class TexturedModelPipeline : public Pipeline {
	public:

		using DepthTestInfo = Pipeline::DepthTestInfo;

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<RenderPass> renderPass_ = nullptr;
			std::shared_ptr<SwapChain> swapChain_ = nullptr;
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			glm::u32vec2 colorAttachmentSize_ = { 0, 0 };
			VkFormat colorAttachmentFormat_ = VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits multisamplingSamplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
		};

		TexturedModelPipeline(const CreateInfo& createInfo) :
			Pipeline{
				Pipeline::CreateInfo{
					createInfo.physicalDevice_,
					createInfo.logicDevice_,
					createInfo.renderPass_,
					{},
					std::vector<std::shared_ptr<DescriptorSetLayout>>{
					std::make_shared<DescriptorSetLayout>(
						DescriptorSetLayout::CreateInfo{
							"GlobalData",
							createInfo.logicDevice_,
							std::vector<VkDescriptorSetLayoutBinding>{{{
							0,
							VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
							1,
							VK_SHADER_STAGE_VERTEX_BIT,
							nullptr
						}}}
						}),std::make_shared<DescriptorSetLayout>(
						DescriptorSetLayout::CreateInfo{
							"Transform",
							createInfo.logicDevice_,
							std::vector<VkDescriptorSetLayoutBinding>{{{
							0,
							VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
							1,
							VK_SHADER_STAGE_VERTEX_BIT,
							nullptr
						}}}
						}),
						std::make_shared<DescriptorSetLayout>(
						DescriptorSetLayout::CreateInfo{
							"DiffuseMap",
							createInfo.logicDevice_,
							std::vector<VkDescriptorSetLayoutBinding>{{{
							0,
							VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
							1,
							VK_SHADER_STAGE_FRAGMENT_BIT,
							nullptr
						}}}
						})
				},
					std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
						createInfo.logicDevice_,
						createInfo.vertexShader_->GetSpirv()
						}),
					std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
						createInfo.logicDevice_,
						createInfo.fragmentShader_->GetSpirv()
						}),
					createInfo.depthTestInfo_,
					createInfo.colorAttachmentSize_,
					createInfo.colorAttachmentFormat_,
							createInfo.multisamplingSamplesCount_,
					VertexInfo{
						Vertex3fnt::GetBindingDescription(),
						Vertex3fnt::GetAttributeDescriptions()						
					},
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_FRONT_FACE_COUNTER_CLOCKWISE,
				VK_CULL_MODE_BACK_BIT,
				{}
				}
			} {	}

	};

	class FlatShadedModelPipeline : public Pipeline {
	public:

		using DepthTestInfo = Pipeline::DepthTestInfo;

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<RenderPass> renderPass_ = nullptr;
			std::shared_ptr<SwapChain> swapChain_ = nullptr;
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			glm::u32vec2 colorAttachmentSize_ = { 0, 0 };
			VkFormat colorAttachmentFormat_ = VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits multisamplingSamplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
		};


		FlatShadedModelPipeline(const CreateInfo& createInfo) :
			Pipeline{
			Pipeline::CreateInfo{
				createInfo.physicalDevice_,
				createInfo.logicDevice_,
				createInfo.renderPass_,
				{},
				std::vector<std::shared_ptr<DescriptorSetLayout>>{
					std::make_shared<DescriptorSetLayout>(
						DescriptorSetLayout::CreateInfo{
							"GlobalData",
							createInfo.logicDevice_,
							std::vector<VkDescriptorSetLayoutBinding>{{{
							0,
							VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
							1,
							VK_SHADER_STAGE_VERTEX_BIT,
							nullptr
						}}}
						}),std::make_shared<DescriptorSetLayout>(
						DescriptorSetLayout::CreateInfo{
							"Transform",
							createInfo.logicDevice_,
							std::vector<VkDescriptorSetLayoutBinding>{{{
							0,
							VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
							1,
							VK_SHADER_STAGE_VERTEX_BIT,
							nullptr
						}}}
						})
				},
				std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					createInfo.logicDevice_,
					createInfo.vertexShader_->GetSpirv()
					}),
				std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					createInfo.logicDevice_,
					createInfo.fragmentShader_->GetSpirv()
					}),
				createInfo.depthTestInfo_,
				createInfo.colorAttachmentSize_,
				createInfo.colorAttachmentFormat_,
							createInfo.multisamplingSamplesCount_,
				VertexInfo{
					Vertex3fnc::GetBindingDescription(),
					Vertex3fnc::GetAttributeDescriptions()
				},
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VK_CULL_MODE_BACK_BIT,
				{}
			}
		} { }
	
	
	};

	class LinesPipeline : public Pipeline {
	public:

		using DepthTestInfo = Pipeline::DepthTestInfo;

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<RenderPass> renderPass_ = nullptr;
			std::shared_ptr<SwapChain> swapChain_ = nullptr;
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			glm::u32vec2 colorAttachmentSize_ = { 0, 0 };
			VkFormat colorAttachmentFormat_ = VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits multisamplingSamplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts_;
			std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
		};


		LinesPipeline(const CreateInfo& createInfo) :
			Pipeline{
			Pipeline::CreateInfo{
				createInfo.physicalDevice_,
				createInfo.logicDevice_,
				createInfo.renderPass_,
				{},
				std::vector<std::shared_ptr<DescriptorSetLayout>>{
					std::make_shared<DescriptorSetLayout>(
						DescriptorSetLayout::CreateInfo{
							"GlobalData",
							createInfo.logicDevice_,
							std::vector<VkDescriptorSetLayoutBinding>{{{
							0,
							VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
							1,
							VK_SHADER_STAGE_VERTEX_BIT,
							nullptr
						}}}
						})
				},
				std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					createInfo.logicDevice_,
					createInfo.vertexShader_->GetSpirv()
					}),
				std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					createInfo.logicDevice_,
					createInfo.fragmentShader_->GetSpirv()
					}),
				createInfo.depthTestInfo_,
				createInfo.colorAttachmentSize_,
				createInfo.colorAttachmentFormat_,
				createInfo.multisamplingSamplesCount_,
				VertexInfo{
					Vertex3fc::GetBindingDescription(),
					Vertex3fc::GetAttributeDescriptions()
				},
				VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
				VK_FRONT_FACE_COUNTER_CLOCKWISE,
				VK_CULL_MODE_BACK_BIT,
				{}
			}
		} { }


	};

	class ImguiPipeline : public Pipeline {
	public:

		using DepthTestInfo = Pipeline::DepthTestInfo;

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<RenderPass> renderPass_ = nullptr;
			
			std::shared_ptr<SwapChain> swapChain_ = nullptr;
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			glm::u32vec2 colorAttachmentSize_ = { 0, 0 };
			VkFormat colorAttachmentFormat_ = VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits multisamplingSamplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
		};


		ImguiPipeline(const CreateInfo& createInfo);


	};

}

