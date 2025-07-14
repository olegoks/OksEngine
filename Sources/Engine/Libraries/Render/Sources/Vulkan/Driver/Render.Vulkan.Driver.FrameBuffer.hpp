#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>

namespace Render::Vulkan {



	class FrameBuffer : public Abstraction<VkFramebuffer>{
	public:
		FrameBuffer() = delete;

		struct CreateInfo {
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::vector<std::shared_ptr<ImageView>> attachments_;
			std::shared_ptr<RenderPass2> renderPass_ = nullptr;
			VkExtent2D extent_{ 0, 0 };
		};

	FrameBuffer(FrameBuffer&& moveFrameBuffer) :
		createInfo_{  } {
		std::swap(createInfo_, moveFrameBuffer.createInfo_);
		
	}

	FrameBuffer(const CreateInfo& createInfo) :
		createInfo_{ createInfo } {

		std::vector<VkImageView> attachments;

		for (const std::shared_ptr<ImageView>& attachment: createInfo.attachments_) {
			attachments.push_back(attachment->GetHandle());
		}

		VkFramebufferCreateInfo framebufferInfo{};
		{
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = createInfo.renderPass_->GetHandle();
			framebufferInfo.attachmentCount = static_cast<Common::UInt32>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = createInfo.extent_.width;
			framebufferInfo.height = createInfo.extent_.height;
			framebufferInfo.layers = 1;
			//framebufferInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
		}
		VkFramebuffer frameBuffer = VK_NULL_HANDLE;
		VkCall(vkCreateFramebuffer(createInfo.LD_->GetHandle(), &framebufferInfo, nullptr, &frameBuffer),
			"Framebuffer create info.");
		SetHandle(frameBuffer);
	}


	FrameBuffer& operator=(const FrameBuffer& copyFrameBuffer) const = delete;
	FrameBuffer& operator=(FrameBuffer&& moveFrameBuffer) {
		if (this == &moveFrameBuffer) {
			return *this;
		}

		std::swap(createInfo_, moveFrameBuffer.createInfo_);

		return *this;
	}
	void Destroy() { }

	~FrameBuffer() {
		if (!IsNullHandle()) {
			vkDestroyFramebuffer(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
		}
	}

	//VkFramebuffer GetNative() const { return frameBuffer_; }

private:
	CreateInfo createInfo_;
	//std::shared_ptr<LogicDevice> LD_ = nullptr;
	//VkFramebuffer frameBuffer_;
};


}