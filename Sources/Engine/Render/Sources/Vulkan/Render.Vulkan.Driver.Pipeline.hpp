#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.SwapChain.hpp>
#include <Render.Vulkan.Driver.ShaderModule.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>
#include <Render.Vulkan.Driver.RenderPass.hpp>
#include <Render.Vulkan.Driver.PipelineLayout.hpp>
#include <Render.Vulkan.Driver.Image.hpp>

namespace Render::Vulkan {

	template<class VertexType>
	class Pipeline : public Abstraction<VkPipeline> {
	public:

		struct DepthTestData {
			std::shared_ptr<Image> image_ = nullptr;
			std::shared_ptr<ImageView> imageView_ = nullptr;
			std::shared_ptr<DeviceMemory> imageMemory_ = nullptr;
			VkPipelineDepthStencilStateCreateInfo depthStencilState_{ 0 };
		};

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<SwapChain> swapChain_ = nullptr;
			std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts_;
			std::shared_ptr<ShaderModule> vertexShader_ = nullptr;
			std::shared_ptr<ShaderModule> fragmentShader_ = nullptr;
			bool depthTest_ = true;
		};

		Pipeline(const CreateInfo& createInfo) : logicDevice_{ createInfo.logicDevice_ } {

			if (createInfo.depthTest_) {

				depthTestData_ = std::make_shared<DepthTestData>();
				{
					Image::CreateInfo depthImageCreateInfo;
					{
						depthImageCreateInfo.physicalDevice_ = createInfo.physicalDevice_;
						depthImageCreateInfo.logicDevice_ = logicDevice_;
						depthImageCreateInfo.format_ = VK_FORMAT_D32_SFLOAT;
						depthImageCreateInfo.extent_ = createInfo.swapChain_->GetExtent();
						depthImageCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
						depthImageCreateInfo.usage_ = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
					}
					depthTestData_->image_ = std::make_shared<AllocatedImage>(depthImageCreateInfo);
					depthTestData_->imageView_ = CreateImageViewByImage(logicDevice_, depthTestData_->image_, VK_IMAGE_ASPECT_DEPTH_BIT);
				}
				
				VkPipelineDepthStencilStateCreateInfo depthStencil{};
				{
					depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
					depthStencil.depthTestEnable = VK_TRUE;
					depthStencil.depthWriteEnable = VK_TRUE;
					depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
					depthStencil.depthBoundsTestEnable = VK_FALSE;
					depthStencil.minDepthBounds = 0.0f;
					depthStencil.maxDepthBounds = 1.0f;
					depthStencil.stencilTestEnable = VK_FALSE;
					depthStencil.front = {};
					depthStencil.back = {};
				}
				depthTestData_->depthStencilState_ = depthStencil;
			}

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			{
				vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vertShaderStageInfo.module = *createInfo.vertexShader_;
				vertShaderStageInfo.pName = "main";
			}
			vertexShader_ = createInfo.vertexShader_;

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
			{
				fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				fragShaderStageInfo.module = *createInfo.fragmentShader_;
				fragShaderStageInfo.pName = "main";
			}
			fragmentShader_ = createInfo.fragmentShader_;

			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			const auto bindingDescription = VertexType::GetBindingDescription();
			const auto attributeDescriptions = VertexType::GetAttributeDescriptions();

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
				inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				inputAssembly.primitiveRestartEnable = VK_FALSE;
			}
			const VkExtent2D extent = createInfo.swapChain_->GetExtent();

			VkViewport viewport{};
			{
				viewport.x = 0.0f;
				viewport.y = 0.0f;
				viewport.width = (float)extent.width;
				viewport.height = (float)extent.height;
				viewport.minDepth = 0.0f;
				viewport.maxDepth = 1.0f;
			}
			VkRect2D scissor{};
			{
				scissor.offset = { 0, 0 };
				scissor.extent = extent;
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
				rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
				rasterizer.lineWidth = 1.f;
				rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;// VK_FRONT_FACE_COUNTER_CLOCKWISE;
				rasterizer.depthBiasEnable = VK_FALSE;
				rasterizer.depthBiasConstantFactor = 0.0f;
				rasterizer.depthBiasClamp = 0.0f;
				rasterizer.depthBiasSlopeFactor = 0.0f;
			}
			VkPipelineMultisampleStateCreateInfo multisampling{};
			{
				multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampling.sampleShadingEnable = VK_FALSE;
				multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
				colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
				colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
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

			RenderPass::CreateInfo renderPassCreateInfo{ 0 };
			{
				renderPassCreateInfo.logicDevice_ = createInfo.logicDevice_;
				renderPassCreateInfo.swapchain_ = createInfo.swapChain_;
				renderPassCreateInfo.depthTest_ = createInfo.depthTest_;
				renderPassCreateInfo.depthBufferInfo_.depthStencilFormat_ = (depthTestData_) ? (depthTestData_->image_->GetFormat()) : (VkFormat::VK_FORMAT_UNDEFINED);//VK_FORMAT_MAX_ENUM;//createInfo.depthBufferFeature_->GetDepthImageFormat();
				renderPass_ = std::make_shared<RenderPass>(renderPassCreateInfo);
			}

			PipelineLayout::CreateInfo pipelineLayoutCreateInfo;
			{
				pipelineLayoutCreateInfo.logicDevice_ = createInfo.logicDevice_;	
				pipelineLayoutCreateInfo.descriptorSetLayouts_.insert(pipelineLayoutCreateInfo.descriptorSetLayouts_.begin(), 
					createInfo.descriptorSetLayouts_.begin(), createInfo.descriptorSetLayouts_.end());
				pipelineLayout_ = std::make_shared<PipelineLayout>(pipelineLayoutCreateInfo);
			}


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
				pipelineInfo.pDepthStencilState = (depthTestData_) ? (&depthTestData_->depthStencilState_) : (nullptr);
				pipelineInfo.pColorBlendState = &colorBlending;
				pipelineInfo.pDynamicState = nullptr; // Optional
				pipelineInfo.layout = *pipelineLayout_;
				pipelineInfo.renderPass = *renderPass_;
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

		[[deprecated]]
		[[nodiscard]]
		VkRenderPass CreateRenderPass(std::shared_ptr<LogicDevice> logicDevice, std::shared_ptr<SwapChain> swapChain) {




			//Color attachment.
			VkAttachmentDescription colorAttachment{};
			{
				colorAttachment.format = swapChain->GetFormat().format;
				colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			}
			VkAttachmentReference colorAttachmentRef{};
			{
				colorAttachmentRef.attachment = 0;
				colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}

			//Depth buffer attachment.
			//VkAttachmentDescription depthAttachment{};
			//{
			//	depthAttachment.format = /*findDepthFormat()*/;
			//	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			//	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			//	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			//	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			//	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			//	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			//	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			//}
			//VkAttachmentReference depthAttachmentRef{};
			//{
			//	depthAttachmentRef.attachment = 1;
			//	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			//}

			VkSubpassDescription subpass{};
			{
				subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass.colorAttachmentCount = 1;
				subpass.pColorAttachments = &colorAttachmentRef;
			}
			VkSubpassDependency dependency{};
			{
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = 0;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			}
			VkRenderPass renderPass;
			{
				VkRenderPassCreateInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderPassInfo.attachmentCount = 1;
				renderPassInfo.pAttachments = &colorAttachment;
				renderPassInfo.subpassCount = 1;
				renderPassInfo.pSubpasses = &subpass;
				renderPassInfo.dependencyCount = 1;
				renderPassInfo.pDependencies = &dependency;

				if (vkCreateRenderPass(logicDevice->GetHandle(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
					throw std::runtime_error("failed to create render pass!");
				}
			}
			return renderPass;
		}

		[[nodiscard]]
		std::shared_ptr<RenderPass> GetRenderPass() const noexcept { return renderPass_; }

		[[nodiscard]]
		bool IsDepthTestEnabled() const noexcept { return depthTestData_ != nullptr; }
		[[nodiscard]]
		std::shared_ptr<ImageView> GetDepthBufferImageView() noexcept { return depthTestData_->imageView_; }

		[[nodiscard]]
		std::shared_ptr<PipelineLayout> GetLayout() const noexcept { return pipelineLayout_; }

	private:

		void Destroy() noexcept {
			OS::AssertMessage(logicDevice_ != nullptr, "Logic device is not initialized.");
			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkPipelineLayout.");
			vkDestroyPipeline(logicDevice_->GetHandle(), GetHandle(), nullptr);
		}

	private:


		std::shared_ptr<ShaderModule> vertexShader_ = nullptr;
		std::shared_ptr<ShaderModule> fragmentShader_ = nullptr;
		std::shared_ptr<DepthTestData> depthTestData_ = nullptr;

		std::shared_ptr<RenderPass> renderPass_ = nullptr;
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		std::shared_ptr<PipelineLayout> pipelineLayout_ = VK_NULL_HANDLE;
	};

}

