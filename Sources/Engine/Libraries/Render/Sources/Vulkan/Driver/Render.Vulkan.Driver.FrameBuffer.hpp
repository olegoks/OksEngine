#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>

namespace Render::Vulkan {


	class FrameBuffer {
	public:
		FrameBuffer() = delete;

		struct CreateInfo {
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::shared_ptr<ImageView> colorImageView_ = nullptr;
			std::shared_ptr<ImageView> depthBufferImageView_ = nullptr;
			std::shared_ptr<ImageView> colorAttachmentResolve_ = nullptr;
			VkRenderPass renderPass_ = VK_NULL_HANDLE;
			VkExtent2D extent_{ 0, 0 };
		};

	FrameBuffer(FrameBuffer&& moveFrameBuffer) :
		LD_{ nullptr },
		frameBuffer_{ VK_NULL_HANDLE } {
		std::swap(LD_, moveFrameBuffer.LD_);
		std::swap(frameBuffer_, moveFrameBuffer.frameBuffer_);
	}

	FrameBuffer(const CreateInfo& createInfo) :
		LD_{ createInfo.LD_ } {


		std::vector<VkImageView> attachments;
		{
			attachments.push_back(createInfo.colorAttachmentResolve_->GetHandle());
			
			if (createInfo.depthBufferImageView_ != nullptr) {
				attachments.push_back(*createInfo.depthBufferImageView_);
			}
			attachments.push_back(createInfo.colorImageView_->GetHandle());
		}

		VkFramebufferCreateInfo framebufferInfo{};
		{
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = createInfo.renderPass_;
			framebufferInfo.attachmentCount = static_cast<Common::UInt32>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = createInfo.extent_.width;
			framebufferInfo.height = createInfo.extent_.height;
			framebufferInfo.layers = 1;
		}
		VkCall(vkCreateFramebuffer(createInfo.LD_->GetHandle(), &framebufferInfo, nullptr, &frameBuffer_),
			"Framebuffer create info.");
	}


	FrameBuffer& operator=(const FrameBuffer& copyFrameBuffer) const = delete;
	FrameBuffer& operator=(FrameBuffer&& moveFrameBuffer) {
		if (this == &moveFrameBuffer) {
			return *this;
		}

		std::swap(frameBuffer_, moveFrameBuffer.frameBuffer_);
		std::swap(LD_, moveFrameBuffer.LD_);


		return *this;
	}
	void Destroy() {
		if (frameBuffer_ != VK_NULL_HANDLE) {
			OS::Assert(frameBuffer_ != VK_NULL_HANDLE && LD_ != nullptr);
			vkDestroyFramebuffer(LD_->GetHandle(), frameBuffer_, nullptr);
		}
	}

	~FrameBuffer() {
		Destroy();
	}

	VkFramebuffer GetNative() const { return frameBuffer_; }

private:
	std::shared_ptr<LogicDevice> LD_ = nullptr;
	VkFramebuffer frameBuffer_;
};


}