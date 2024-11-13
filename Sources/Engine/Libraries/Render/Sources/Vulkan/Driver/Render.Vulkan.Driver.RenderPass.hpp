#pragma once

#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>

#include <Render.Vulkan.Driver.SwapChain.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {


	class RenderPass : public Abstraction<VkRenderPass> {
	public:

		struct DepthTestInfo {
			VkFormat depthStencilBufferFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
		};

		struct CreateInfo {
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			VkFormat colorAttachmentFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
			VkSampleCountFlagBits samplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
			std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
		};

		struct Subpass {

			struct DepthTestInfo {
				VkFormat depthStencilBufferFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
			};

			struct CreateInfo {
				VkFormat colorAttachmentFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
				VkSampleCountFlagBits samplesCount_ = VkSampleCountFlagBits::VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
				std::shared_ptr<DepthTestInfo> depthTestInfo_ = nullptr;
			};

			Subpass(const CreateInfo& createInfo) noexcept {

				{
					colorAttachment_.format = createInfo.colorAttachmentFormat_;
					colorAttachment_.samples = createInfo.samplesCount_;

					//loadOp и storeOp применяются к буферам цвета и глубины.
					//VK_ATTACHMENT_LOAD_OP_LOAD: буфер будет содержать те данные, которые были помещены в него до этого прохода(например, во время предыдущего прохода)
					//VK_ATTACHMENT_LOAD_OP_CLEAR : буфер очищается в начале прохода рендера
					//VK_ATTACHMENT_LOAD_OP_DONT_CARE : содержимое буфера не определено; для нас оно не имеет значения
					colorAttachment_.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // What will be done with buffer before render. We will fill frame with black color.

					//VK_ATTACHMENT_STORE_OP_STORE: содержимое буфера сохраняется в память для дальнейшего использования
					//VK_ATTACHMENT_STORE_OP_DONT_CARE : после рендеринга буфер больше не используется, и его содержимое не имеет значения
					colorAttachment_.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // What will be done with buffer after render.

					//Для буфера трафарета используются поля stencilLoadOp / stencilStoreOp.Мы не используем буфер трафарета,
					//поэтому результаты загрузки и сохранения нас не интересуют.
					colorAttachment_.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					colorAttachment_.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

					//Значение VK_IMAGE_LAYOUT_UNDEFINED в поле initialLayout обозначает,
					//что нас не интересует предыдущий layout, в котором был image. 
					//Использование этого значения не гарантирует сохранение содержимого image, 
					//но это и не важно, поскольку мы все равно очистим его. 
					// После рендеринга нам нужно вывести наш image на экран, 
					//поэтому в поле finalLayout укажем VK_IMAGE_LAYOUT_PRESENT_SRC_KHR.
					colorAttachment_.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					//You'll notice that we have changed the finalLayout from VK_IMAGE_LAYOUT_PRESENT_SRC_KHR to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL. 
					//That's because multisampled images cannot be presented directly.
					//We first need to resolve them to a regular image.
					colorAttachment_.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					colorAttachmentRef_.attachment = 0;
					colorAttachmentRef_.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				//if (createInfo.depthTestInfo_ != nullptr) {
				depthBufferAttachment_.format = createInfo.depthTestInfo_->depthStencilBufferFormat_;
				depthBufferAttachment_.samples = createInfo.samplesCount_;
				depthBufferAttachment_.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthBufferAttachment_.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthBufferAttachment_.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				depthBufferAttachment_.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				depthBufferAttachment_.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				depthBufferAttachment_.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

				depthBufferAttachmentRef_.attachment = 1;
				depthBufferAttachmentRef_.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				//}


				{
					colorAttachmentResolve_.format = createInfo.colorAttachmentFormat_;
					colorAttachmentResolve_.samples = VK_SAMPLE_COUNT_1_BIT;
					colorAttachmentResolve_.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					colorAttachmentResolve_.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
					colorAttachmentResolve_.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					colorAttachmentResolve_.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					colorAttachmentResolve_.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					colorAttachmentResolve_.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

					colorAttachmentRefResolve_.attachment = 2;
					colorAttachmentRefResolve_.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}


				descriptor_.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				descriptor_.colorAttachmentCount = 1;
				descriptor_.pColorAttachments = &colorAttachmentRef_;
				//descriptor_.
				descriptor_.pResolveAttachments = &colorAttachmentRefResolve_;
				//if (createInfo.depthTestInfo_ != nullptr) {
				descriptor_.pDepthStencilAttachment = &depthBufferAttachmentRef_;
				//}

				dependency_.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency_.dstSubpass = 0;

				//if (createInfo.depthTestInfo_ != nullptr) {
				dependency_.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency_.srcAccessMask = 0;
				dependency_.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				dependency_.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				//}
				//else {
				//	dependency_.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT /*| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT*/;
				//	dependency_.srcAccessMask = 0;
				//	dependency_.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT/* | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT*/;
				//	dependency_.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT/* | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT*/;
				//}
			}

			const VkAttachmentDescription& GetColorAttachment() const noexcept {
				return colorAttachment_;
			}

			const VkAttachmentDescription& GetColorAttachmentResolve() const noexcept {
				return colorAttachmentResolve_;
			}

			const VkAttachmentDescription& GetDepthStencilAttachment() const noexcept {
				return depthBufferAttachment_;
			}

			VkAttachmentDescription colorAttachment_{ 0 };
			VkAttachmentReference colorAttachmentRef_{ 0 };

			VkAttachmentDescription depthBufferAttachment_{ 0 };
			VkAttachmentReference depthBufferAttachmentRef_{ 0 };

			VkAttachmentDescription colorAttachmentResolve_{ 0 };
			VkAttachmentReference colorAttachmentRefResolve_{ 0 };

			VkSubpassDependency dependency_{ 0 };
			VkSubpassDescription descriptor_{ 0 };

		};

		RenderPass(const CreateInfo& createInfo) noexcept : LD_{ createInfo.LD_ } {

			{
				Subpass::CreateInfo subpassCreateInfo{ };
				{
					subpassCreateInfo.colorAttachmentFormat_ = createInfo.colorAttachmentFormat_;
					if (createInfo.depthTestInfo_ != nullptr) {
						auto depthTestInfo = std::make_shared<Subpass::DepthTestInfo>();
						depthTestInfo->depthStencilBufferFormat_ = createInfo.depthTestInfo_->depthStencilBufferFormat_;
						subpassCreateInfo.depthTestInfo_ = depthTestInfo;
					}
					subpassCreateInfo.samplesCount_ = createInfo.samplesCount_;
				}
				//Subpass subpass{ subpassCreateInfo };

				//Multisample attachment 
				VkAttachmentDescription colorAttachment{
					.flags = 0,
					.format = createInfo.colorAttachmentFormat_,
					.samples = createInfo.samplesCount_,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				};
				VkAttachmentReference colorAttachmentRef{
					.attachment = 0,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				};

				//Depth attachment
				VkAttachmentDescription depthAttachment{
					.flags = 0,
					.format = createInfo.depthTestInfo_->depthStencilBufferFormat_,
					.samples = VK_SAMPLE_COUNT_1_BIT,//createInfo.samplesCount_,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				};
				VkAttachmentReference depthAttachmentRef{
					.attachment = 1,
					.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				};



				//swap chain
				VkAttachmentDescription swapChainAttachment{
					.flags = 0,
					.format = createInfo.colorAttachmentFormat_,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				};
				VkAttachmentReference swapChainAttachmentRef{
					.attachment = 2,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				};

				//Rendered buffer
				VkAttachmentDescription renderedAttachment{
					.flags = 0,
					.format = createInfo.colorAttachmentFormat_,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL//VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				};

				VkAttachmentReference renderedAttachmentRef{
					.attachment = 3,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				};

				VkAttachmentReference renderedAttachmentSubpass2Ref{
					.attachment = 3,
					.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
				};

				VkSubpassDependency dependency_{
					.srcSubpass = VK_SUBPASS_EXTERNAL,
					.dstSubpass = 0,
					.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,//VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstAccessMask = VK_ACCESS_INDEX_READ_BIT,
				};

				VkSubpassDependency postProcessSubpassDependency_{
					.srcSubpass = 0,
					.dstSubpass = 1,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,//VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstAccessMask = VK_ACCESS_INDEX_READ_BIT//VK_ACCESS_INPUT_ATTACHMENT_READ_BIT
				};


				VkSubpassDescription renderSubpassDesc{
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					//.inputAttachmentCount = 1,
					//.pInputAttachments = &renderedAttachmentRef,
					.colorAttachmentCount = 1,
					.pColorAttachments = &renderedAttachmentRef,
					.pResolveAttachments = nullptr,
					.pDepthStencilAttachment = &depthAttachmentRef
				};


				VkSubpassDescription postProcessSubpassDesc{
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.inputAttachmentCount = 1,
					.pInputAttachments = &renderedAttachmentSubpass2Ref,
					.colorAttachmentCount = 1,
					.pColorAttachments = &swapChainAttachmentRef,
					.pResolveAttachments = nullptr,//&multisampleAttachmentRef
					.pDepthStencilAttachment = nullptr// &depthAttachmentRef
				};

				VkSubpassDescription subpasses[2]{ renderSubpassDesc, postProcessSubpassDesc };


				std::vector<VkAttachmentDescription> attachments;
				{
					attachments.push_back(colorAttachment);
					attachments.push_back(depthAttachment);
					attachments.push_back(swapChainAttachment);
					attachments.push_back(renderedAttachment);
				}

				VkRenderPassCreateInfo renderPassInfo{};
				{
					renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
					renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
					renderPassInfo.pAttachments = attachments.data();
					renderPassInfo.subpassCount = 2;
					renderPassInfo.pSubpasses = &subpasses[0];
					//renderPassInfo.dependencyCount = 1;
					//renderPassInfo.pDependencies = &dependency_;
				}
				VkRenderPass renderPass = VK_NULL_HANDLE;
				VkCall(vkCreateRenderPass(createInfo.LD_->GetHandle(), &renderPassInfo, nullptr, &renderPass),
					"Error while creating render pass.");
				SetHandle(renderPass);
			}

		}

		~RenderPass() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(LD_ != nullptr, "Logic device is not initialized.");
			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkRenderPass.");
			vkDestroyRenderPass(LD_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		std::shared_ptr<LogicDevice> LD_ = nullptr;
	};
}