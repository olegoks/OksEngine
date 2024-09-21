#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Abstraction.hpp>

namespace Render::Vulkan {

	class DeviceMemory : public Abstraction<VkDeviceMemory> {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			VkMemoryRequirements requirements_ = { 0 };
			uint32_t memoryTypeIndex_ = 0;
		};

		DeviceMemory(const DeviceMemory& copyDeviceMemory) noexcept = delete;

		DeviceMemory(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {
			VkMemoryAllocateInfo alloccationInfo{};
			{
				alloccationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				alloccationInfo.allocationSize = createInfo.requirements_.size;
				alloccationInfo.memoryTypeIndex = createInfo.memoryTypeIndex_;
			}

			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkCall(vkAllocateMemory(createInfo.LD_->GetHandle(), &alloccationInfo, nullptr, &memory),
				"Allocating GPU memory error.");
			SetHandle(memory);
		}

		[[nodiscard]]
		VkDeviceSize GetSize() const noexcept { return createInfo_.requirements_.size; }

		void Fill(const void* memory, Common::Size bytesNumber) noexcept {

			OS::AssertMessage(bytesNumber <= GetSize(), "Attempt to write to device memory more or less bytes than device memory size.");

			void* pointerToMappedMemory = nullptr;
			{
				VkCall(vkMapMemory(createInfo_.LD_->GetHandle(), GetHandle(), 0, bytesNumber, 0, &pointerToMappedMemory),
					"Error while mapping buffer to device memory.");
			}
			memcpy(pointerToMappedMemory, memory, (size_t)bytesNumber);
			{
				vkUnmapMemory(createInfo_.LD_->GetHandle(), GetHandle());
			}
		}


		~DeviceMemory() noexcept {
			OS::Assert(GetHandle() != nullptr);
			vkFreeMemory(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
			SetNullHandle();
		}

	private:
		CreateInfo createInfo_;
	};

}