
#include <Render.Vulkan.Driver.Image.hpp>

#include <Render.Vulkan.Driver.CommandPool.hpp>
#include <Render.Vulkan.Driver.CommandBuffer.hpp>

namespace Render::Vulkan {



	//void Image::ChangeLayout(
	//	VkImageLayout oldLayout,
	//	VkImageLayout newLayout,
	//	std::shared_ptr<CommandPool> commandPool) {

	//	//CommandBuffer::CreateInfo commandBufferCreateInfo;
	//	//{
	//	//	commandBufferCreateInfo.LD_ = createInfo_.LD_;
	//	//	commandBufferCreateInfo.commandPool_ = commandPool;
	//	//	commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//	//}
	//	//auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
	//	//commandBuffer->Begin();

	//	//if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
	//	//	commandBuffer->ImageMemoryBarrier(
	//	//		GetHandle(),
	//	//		0,
	//	//		createInfo_.mipLevels_,
	//	//		oldLayout, newLayout,
	//	//		VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT,
	//	//		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
	//	//	);
	//	//}
	//	//else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
	//	//	commandBuffer->ImageMemoryBarrier(
	//	//		GetHandle(),
	//	//		0,
	//	//		//createInfo_.mipLevels_,
	//	//		oldLayout, newLayout,
	//	//		VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
	//	//		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
	//	//	);
	//	//}
	//	//else {
	//	//	OS::NotImplemented();
	//	//}

	//	//commandBuffer->End();
	//	//VkSubmitInfo submitInfo{};
	//	//{
	//	//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//	//	submitInfo.commandBufferCount = 1;
	//	//	submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
	//	//}
	//	//vkQueueSubmit(createInfo_.LD_->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	//	//vkQueueWaitIdle(createInfo_.LD_->GetGraphicsQueue());

	//}



	void Image::DataCopy(
		std::shared_ptr<Buffer> bufferFrom,
		std::shared_ptr<Image> imageTo,
		std::shared_ptr<LogicDevice> logicDevice,
		std::shared_ptr<CommandPool> commandPool) {

		CommandBuffer::CreateInfo commandBufferCreateInfo;
		{
			commandBufferCreateInfo.name_ = "Buffer to image copy command buffer";
			commandBufferCreateInfo.LD_ = logicDevice;
			commandBufferCreateInfo.commandPool_ = commandPool;
			commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		}
		auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
		commandBuffer->Begin();
		commandBuffer->Copy(bufferFrom, imageTo);
		commandBuffer->End();
		VkSubmitInfo submitInfo{};
		{
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
		}
		vkQueueSubmit(logicDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(logicDevice->GetGraphicsQueue());
	}

	Image::~Image() noexcept {
		if (!IsNullHandle()) {
			vkDestroyImage(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
			SetNullHandle();
		}
	}


	void Image::Destroy() noexcept {
		ASSERT_FMSG(
			!IsNullHandle(),
			"Attempt to destroy image twice.");
		vkDestroyImage(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
		SetNullHandle();
	}

}