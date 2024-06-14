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
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<ImageView> colorImageView_ = nullptr;
			std::shared_ptr<ImageView> depthBufferImageView_ = nullptr;
			VkRenderPass renderPass_ = VK_NULL_HANDLE;
			VkExtent2D extent_{ 0, 0 };
		};

	FrameBuffer(FrameBuffer&& moveFrameBuffer) :
		logicDevice_{ nullptr },
		frameBuffer_{ VK_NULL_HANDLE } {
		std::swap(logicDevice_, moveFrameBuffer.logicDevice_);
		std::swap(frameBuffer_, moveFrameBuffer.frameBuffer_);
	}

	FrameBuffer(const CreateInfo& createInfo) :
		logicDevice_{ createInfo.logicDevice_ } {


		std::vector<VkImageView> attachments;
		{
			attachments.push_back(createInfo.colorImageView_->GetHandle());
			if (createInfo.depthBufferImageView_ != nullptr) {
				attachments.push_back(*createInfo.depthBufferImageView_);
			}

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
		VkCall(vkCreateFramebuffer(createInfo.logicDevice_->GetHandle(), &framebufferInfo, nullptr, &frameBuffer_),
			"Framebuffer create info.");
	}


	FrameBuffer& operator=(const FrameBuffer& copyFrameBuffer) const = delete;
	FrameBuffer& operator=(FrameBuffer&& moveFrameBuffer) {
		if (this == &moveFrameBuffer) {
			return *this;
		}

		std::swap(frameBuffer_, moveFrameBuffer.frameBuffer_);
		std::swap(logicDevice_, moveFrameBuffer.logicDevice_);


		return *this;
	}
	void Destroy() {
		if (frameBuffer_ != VK_NULL_HANDLE) {
			OS::Assert(frameBuffer_ != VK_NULL_HANDLE && logicDevice_ != nullptr);
			vkDestroyFramebuffer(logicDevice_->GetHandle(), frameBuffer_, nullptr);
		}
	}

	~FrameBuffer() {
		Destroy();
	}

	VkFramebuffer GetNative() const { return frameBuffer_; }

private:
	std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
	VkFramebuffer frameBuffer_;
};


}