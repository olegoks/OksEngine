#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <Common.Types.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.CommandPool.hpp>

namespace Render::Vulkan {

	class Buffer {
	public:
		Buffer() = delete;
		Buffer(Buffer&& moveBuffer) : buffer_{ VK_NULL_HANDLE }, logicDevice_{ nullptr } {

			std::swap(buffer_, moveBuffer.buffer_);
			std::swap(size_, moveBuffer.size_);
			std::swap(bufferMemory_, moveBuffer.bufferMemory_);
			std::swap(memoryType_, moveBuffer.memoryType_);
			std::swap(logicDevice_, moveBuffer.logicDevice_);
			std::swap(memoryRequirements_, moveBuffer.memoryRequirements_);

		}

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> logicDevice_;
			VkDeviceSize size_;
			VkBufferUsageFlags usage_;
			VkMemoryPropertyFlags properties_;
		};

		Buffer(const CreateInfo& createInfo) :
			logicDevice_{ createInfo.logicDevice_ }, size_{ createInfo.size_ } {

			const VkBuffer buffer = CreateBuffer(createInfo.logicDevice_->GetHandle(), createInfo.size_, createInfo.usage_);
			const VkMemoryRequirements memoryRequirements = GetBufferMemoryRequirements(createInfo.logicDevice_->GetHandle(), buffer);
			const uint32_t memoryType = FindSuitableMemoryType(createInfo.physicalDevice_, memoryRequirements.memoryTypeBits, createInfo.properties_);
			const VkDeviceMemory deviceMemory = AllocateMemory(createInfo.logicDevice_->GetHandle(), memoryRequirements, memoryType);
			BindDeviceMemoryToBuffer(createInfo.logicDevice_->GetHandle(), deviceMemory, buffer);

			buffer_ = buffer;
			bufferMemory_ = deviceMemory;
			memoryType_ = memoryType;
			memoryRequirements_ = memoryRequirements;
		}

		void Fill(const void* data) noexcept {
			
			void* pointerToMappedMemory = nullptr;
			{
				[[maybe_unused]]
				const VkResult result = vkMapMemory(logicDevice_->GetHandle(), GetDeviceMemory(), 0, GetSizeInBytes(), 0, &pointerToMappedMemory);
				OS::AssertMessage(result == VK_SUCCESS, "Error while mapping buffer to device memory.");
			}
			memcpy(pointerToMappedMemory, data, (size_t)GetSizeInBytes());
			{
				vkUnmapMemory(logicDevice_->GetHandle(), GetDeviceMemory());
			}
		}

		static void DataCopy(std::shared_ptr<Buffer> bufferFrom, std::shared_ptr<Buffer> bufferTo, std::shared_ptr<LogicDevice> logicDevice, std::shared_ptr<CommandPool> commandPool) {
			
			VkCommandBufferAllocateInfo allocInfo{};
			{
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = commandPool->GetNative();
				allocInfo.commandBufferCount = 1;
			}
			
			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(logicDevice->GetHandle(), &allocInfo, &commandBuffer);
			
			VkCommandBufferBeginInfo beginInfo{};
			{
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			}

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			VkBufferCopy copyRegion{};
			{
				copyRegion.srcOffset = 0;
				copyRegion.dstOffset = 0;
				copyRegion.size = bufferFrom->GetSizeInBytes();
			}
			vkCmdCopyBuffer(commandBuffer, bufferFrom->GetNative(), bufferTo->GetNative(), 1, &copyRegion);
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			{
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;
			}
			vkQueueSubmit(logicDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(logicDevice->GetGraphicsQueue());

			vkFreeCommandBuffers(logicDevice->GetHandle(), commandPool->GetNative(), 1, &commandBuffer);
		}

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept { return size_; }

		[[nodiscard]]
		const VkBuffer& GetNative() const noexcept { return buffer_; }

		[[nodiscard]]
		const VkDeviceMemory& GetDeviceMemory() const noexcept { return bufferMemory_; }

		~Buffer() {
			if (GetNative() != VK_NULL_HANDLE) {
				DestroyBuffer(logicDevice_, buffer_);
				FreeMemory(logicDevice_, bufferMemory_);
			}
		}

	private:

		static VkBuffer CreateBuffer(VkDevice logicDevice, VkDeviceSize size, VkBufferUsageFlags usage) noexcept {
			VkBufferCreateInfo bufferInfo{};
			{
				bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
				bufferInfo.size = size;
				bufferInfo.usage = usage;
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			}
			VkBuffer buffer = VK_NULL_HANDLE;
			[[maybe_unused]]
			const VkResult result = vkCreateBuffer(logicDevice, &bufferInfo, nullptr, &buffer);
			OS::AssertMessage(result == VK_SUCCESS, "Error while creating buffer.");
			return buffer;
		}

		static void BindDeviceMemoryToBuffer(VkDevice logicDevice, VkDeviceMemory deviceMemory, VkBuffer buffer, VkDeviceSize offset = 0) noexcept {
			[[maybe_unused]]
			const VkResult result = vkBindBufferMemory(logicDevice, buffer, deviceMemory, offset);
			OS::AssertMessage(result == VK_SUCCESS, "Error while binding device memory to buffer.");
		}

		static VkDeviceMemory AllocateMemory(VkDevice logicDevice, VkMemoryRequirements memoryRequirements, uint32_t memoryType) noexcept {
			VkMemoryAllocateInfo allocInfo{};
			{
				allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocInfo.allocationSize = memoryRequirements.size;
				allocInfo.memoryTypeIndex = memoryType;
			}
			VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
			[[maybe_unused]]
			const VkResult result = vkAllocateMemory(logicDevice, &allocInfo, nullptr, &deviceMemory);
			OS::AssertMessage(result == VK_SUCCESS, "Error while allocation buffer memory.");

			return deviceMemory;
		}
		static VkMemoryRequirements GetBufferMemoryRequirements(VkDevice logicDevice, VkBuffer buffer) {
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(logicDevice, buffer, &memRequirements);
			return memRequirements;
		}

		static uint32_t FindSuitableMemoryType(std::shared_ptr<PhysicalDevice> physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
			const VkPhysicalDeviceMemoryProperties memProperties = physicalDevice->GetMemoryProperties();
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
				const VkMemoryType memoryType = memProperties.memoryTypes[i];
				if ((typeFilter & (1 << i)) && (memoryType.propertyFlags & properties) == properties) {
					return i;
				}
			}
			OS::AssertFailMessage("Suitable device memory was not found.");
			return std::numeric_limits<uint32_t>::max();
		}
		static void FreeMemory(std::shared_ptr<LogicDevice> logicDevice, VkDeviceMemory deviceMemory) {
			vkFreeMemory(logicDevice->GetHandle(), deviceMemory, nullptr);
		}
		static void DestroyBuffer(std::shared_ptr<LogicDevice> logicDevice, VkBuffer buffer) {
			OS::AssertMessage((logicDevice != nullptr) && (logicDevice->GetHandle() != VK_NULL_HANDLE), "Attempt to destroy buffer using null logic device.");
			OS::AssertMessage(buffer != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE buffer.");
			vkDestroyBuffer(logicDevice->GetHandle(), buffer, nullptr);
		}

	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		VkDeviceSize size_ = 0;
		VkBuffer buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory_ = VK_NULL_HANDLE;
		uint32_t memoryType_ = std::numeric_limits<uint32_t>::max();
		VkMemoryRequirements memoryRequirements_;
	};

}