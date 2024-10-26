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
#include <Render.Vulkan.Driver.DeviceMemory.hpp>

namespace Render::Vulkan {

	class Buffer : public Abstraction<VkBuffer>{
	public:
		Buffer() = delete;

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> LD_;
			Common::Size size_;
			VkBufferUsageFlags usage_;
			VkMemoryPropertyFlags properties_;
		};

		Buffer(const CreateInfo& createInfo);

		void Fill(Common::Size offset, const void* data, Common::Size sizeInBytes) noexcept;

		static void DataCopy(
			const std::shared_ptr<Buffer> bufferFrom, 
			std::shared_ptr<Buffer> bufferTo,
			Common::Size offsetFrom,
			Common::Size offsetTo,
			Common::Size bytesNumber,
			std::shared_ptr<LogicDevice> ld,
			std::shared_ptr<CommandPool> commandPool);

		static void DataCopy(
			const Buffer& bufferFrom,
			Buffer& bufferTo, 
			Common::Size offsetFrom,
			Common::Size offsetTo,
			Common::Size bytesNumber,
			std::shared_ptr<LogicDevice> ld, 
			std::shared_ptr<CommandPool> commandPool);

		void CopyDataTo(
			std::shared_ptr<Buffer> bufferTo,
			std::shared_ptr<CommandPool> commandPool);

		void CopyFullDataTo(
			Buffer& bufferTo,
			std::shared_ptr<CommandPool> commandPool);

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept;

		void BindMemory(std::shared_ptr<DeviceMemory> deviceMemory);

		[[nodiscard]]
		VkMemoryRequirements GetMemoryRequirements() const noexcept {
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(
				createInfo_.LD_->GetHandle(),
				GetHandle(),
				&memRequirements);
			return memRequirements;
		}

		void Allocate();
		void Deallocate();
		bool IsAllocated() const noexcept;

		~Buffer();

	private:

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


	private:
		CreateInfo createInfo_;
		std::shared_ptr<DeviceMemory> memory_ = nullptr;
	};

		
}