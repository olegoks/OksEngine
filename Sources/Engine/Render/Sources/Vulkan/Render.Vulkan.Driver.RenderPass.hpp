#pragma once

#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>

#include <Render.Vulkan.Driver.SwapChain.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {


	class RenderPass : public Abstraction<VkRenderPass> {
	public:

		struct CreateInfo {
			std::shared_ptr<SwapChain> swapchain_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			bool depthTest_ = true;
			struct DepthBufferInfo {
				VkFormat depthStencilFormat_ = VkFormat::VK_FORMAT_UNDEFINED;
			} depthBufferInfo_;

		};

		struct Subpass {

			struct CreateInfo {
				VkFormat colorAttachmentFormat_ = VkFormat::VK_FORMAT_MAX_ENUM;
				struct DepthBufferInfo {
					bool enable_ = false;
					VkFormat depthStencilFormat_ = VkFormat::VK_FORMAT_MAX_ENUM;
				} depthBufferInfo_;
			};

			Subpass(const CreateInfo& createInfo) noexcept {

				{
					colorAttachment_.format = createInfo.colorAttachmentFormat_;
					colorAttachment_.samples = VK_SAMPLE_COUNT_1_BIT; // We do not use multisampling that is why number of samples is 1.

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
					colorAttachment_.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

					colorAttachmentRef_.attachment = 0;
					colorAttachmentRef_.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}

				if (createInfo.depthBufferInfo_.enable_) {
					depthBufferAttachment_.format = VK_FORMAT_D32_SFLOAT;//VK_FORMAT_MAX_ENUM;//depthStencilFormat;
					depthBufferAttachment_.samples = VK_SAMPLE_COUNT_1_BIT;
					depthBufferAttachment_.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
					depthBufferAttachment_.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					depthBufferAttachment_.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					depthBufferAttachment_.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
					depthBufferAttachment_.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					depthBufferAttachment_.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					depthBufferAttachmentRef_.attachment = 1;
					depthBufferAttachmentRef_.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				}

				descriptor_.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				descriptor_.colorAttachmentCount = 1;
				descriptor_.pColorAttachments = &colorAttachmentRef_;
				if (createInfo.depthBufferInfo_.enable_) {
					descriptor_.pDepthStencilAttachment = &depthBufferAttachmentRef_;
				}

				dependency_.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency_.dstSubpass = 0;

				if (createInfo.depthBufferInfo_.enable_) {
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

			const VkAttachmentDescription& GetDepthStencilAttachment() const noexcept {
				return depthBufferAttachment_;
			}

			VkAttachmentDescription colorAttachment_{ 0 };
			VkAttachmentReference colorAttachmentRef_{ 0 };

			VkAttachmentDescription depthBufferAttachment_{ 0 };
			VkAttachmentReference depthBufferAttachmentRef_{ 0 };

			VkSubpassDependency dependency_{ 0 };
			VkSubpassDescription descriptor_{ 0 };

		};

		RenderPass(const CreateInfo& createInfo) noexcept : logicDevice_{ createInfo.logicDevice_ } {

			if (createInfo.depthTest_) {
				OS::AssertMessage(createInfo.depthBufferInfo_.depthStencilFormat_ != VK_FORMAT_UNDEFINED, "Format for depth stencil buffer was not set.");
			}

			{
				Subpass::CreateInfo subpassCreateInfo{ };
				{
					subpassCreateInfo.colorAttachmentFormat_ = createInfo.swapchain_->GetFormat().format;
					subpassCreateInfo.depthBufferInfo_.enable_ = createInfo.depthTest_;
					subpassCreateInfo.depthBufferInfo_.depthStencilFormat_ = createInfo.depthBufferInfo_.depthStencilFormat_;
				}
				Subpass subpass{ subpassCreateInfo };

				std::vector<VkAttachmentDescription> attachments;
				{
					attachments.push_back(subpass.GetColorAttachment());
					if (createInfo.depthTest_) {
						attachments.push_back(subpass.GetDepthStencilAttachment());
					}
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
				[[maybe_unused]]
				const VkResult result = vkCreateRenderPass(createInfo.logicDevice_->GetHandle(), &renderPassInfo, nullptr, &renderPass);
				OS::AssertMessage(result == VK_SUCCESS, "");
				SetHandle(renderPass);
			}

		}

		~RenderPass() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(logicDevice_ != nullptr, "Logic device is not initialized.");
			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkRenderPass.");
			vkDestroyRenderPass(logicDevice_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
	};
}