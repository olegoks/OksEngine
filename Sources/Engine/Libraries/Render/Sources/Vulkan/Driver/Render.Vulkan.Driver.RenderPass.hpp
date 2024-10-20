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

					//loadOp � storeOp ����������� � ������� ����� � �������.
					//VK_ATTACHMENT_LOAD_OP_LOAD: ����� ����� ��������� �� ������, ������� ���� �������� � ���� �� ����� �������(��������, �� ����� ����������� �������)
					//VK_ATTACHMENT_LOAD_OP_CLEAR : ����� ��������� � ������ ������� �������
					//VK_ATTACHMENT_LOAD_OP_DONT_CARE : ���������� ������ �� ����������; ��� ��� ��� �� ����� ��������
					colorAttachment_.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // What will be done with buffer before render. We will fill frame with black color.

					//VK_ATTACHMENT_STORE_OP_STORE: ���������� ������ ����������� � ������ ��� ����������� �������������
					//VK_ATTACHMENT_STORE_OP_DONT_CARE : ����� ���������� ����� ������ �� ������������, � ��� ���������� �� ����� ��������
					colorAttachment_.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // What will be done with buffer after render.

					//��� ������ ��������� ������������ ���� stencilLoadOp / stencilStoreOp.�� �� ���������� ����� ���������,
					//������� ���������� �������� � ���������� ��� �� ����������.
					colorAttachment_.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					colorAttachment_.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

					//�������� VK_IMAGE_LAYOUT_UNDEFINED � ���� initialLayout ����������,
					//��� ��� �� ���������� ���������� layout, � ������� ��� image. 
					//������������� ����� �������� �� ����������� ���������� ����������� image, 
					//�� ��� � �� �����, ��������� �� ��� ����� ������� ���. 
					// ����� ���������� ��� ����� ������� ��� image �� �����, 
					//������� � ���� finalLayout ������ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR.
					colorAttachment_.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					//You'll notice that we have changed the finalLayout from VK_IMAGE_LAYOUT_PRESENT_SRC_KHR to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL. 
					//That's because multisampled images cannot be presented directly.
					//We first need to resolve them to a regular image.
					colorAttachment_.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					colorAttachmentRef_.attachment = 0;
					colorAttachmentRef_.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				if (createInfo.depthTestInfo_ != nullptr) {
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
				}


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
				descriptor_.pResolveAttachments = &colorAttachmentRefResolve_;
				if (createInfo.depthTestInfo_ != nullptr) {
					descriptor_.pDepthStencilAttachment = &depthBufferAttachmentRef_;
				}

				dependency_.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency_.dstSubpass = 0;

				if (createInfo.depthTestInfo_ != nullptr) {
					dependency_.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
					dependency_.srcAccessMask = 0;
					dependency_.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
					dependency_.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				}
				else {
					dependency_.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT /*| VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT*/;
					dependency_.srcAccessMask = 0;
					dependency_.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT/* | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT*/;
					dependency_.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT/* | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT*/;
				}
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
					if(createInfo.depthTestInfo_ != nullptr) {
						auto depthTestInfo = std::make_shared<Subpass::DepthTestInfo>();
						depthTestInfo->depthStencilBufferFormat_ = createInfo.depthTestInfo_->depthStencilBufferFormat_;
						subpassCreateInfo.depthTestInfo_ = depthTestInfo;
					}
					subpassCreateInfo.samplesCount_ = createInfo.samplesCount_;
				}
				Subpass subpass{ subpassCreateInfo };

				std::vector<VkAttachmentDescription> attachments;
				{
					attachments.push_back(subpass.GetColorAttachment());
					if (createInfo.depthTestInfo_ != nullptr) {
						attachments.push_back(subpass.GetDepthStencilAttachment());
					}
					attachments.push_back(subpass.GetColorAttachmentResolve());
				}

				VkRenderPassCreateInfo renderPassInfo{};
				{
					renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
					renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
					renderPassInfo.pAttachments = attachments.data();
					renderPassInfo.subpassCount = 1;
					renderPassInfo.pSubpasses = &subpass.descriptor_;
					renderPassInfo.dependencyCount = 1;
					renderPassInfo.pDependencies = &subpass.dependency_;
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