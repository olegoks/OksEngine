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
			std::shared_ptr<PhysicalDevice> PD_ = nullptr;
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
			VK_CALL(vkAllocateMemory(createInfo.LD_->GetHandle(), &alloccationInfo, nullptr, &memory),
				"Allocating GPU memory error.");
			SetHandle(memory);
		}

		[[nodiscard]]
		Common::Size GetSize() const noexcept { return createInfo_.requirements_.size; }

		void WriteFlush(Common::Size offsetInBytes, const void* memory, Common::Size bytesNumber) noexcept {

			ASSERT(memory != nullptr);
			ASSERT(bytesNumber > 0);
			ASSERT_FMSG(offsetInBytes + bytesNumber <= GetSize(), "Attempt to write to device memory more or less bytes than device memory size.");


			const Common::Size alignedBytesToFlush = Common::CalculateAlignedSize(bytesNumber, createInfo_.PD_->GetProperties().limits.nonCoherentAtomSize);
			
			ASSERT(offsetInBytes % createInfo_.PD_->GetProperties().limits.nonCoherentAtomSize == 0);
			ASSERT(alignedBytesToFlush <= createInfo_.requirements_.size);

			void* pointerToMappedMemory = nullptr;
			{
				VK_CALL(vkMapMemory(
					createInfo_.LD_->GetHandle(),
					GetHandle(),
					offsetInBytes,
					alignedBytesToFlush,//bytesNumber,
					0,
					&pointerToMappedMemory),
					"Error while mapping buffer to device memory.");
			}
			std::memcpy(pointerToMappedMemory, memory, (size_t)bytesNumber);


			VkMappedMemoryRange memoryRange = {
				.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
				.memory = GetHandle(),
				.offset = offsetInBytes,
				.size = alignedBytesToFlush,
			};

			VK_CALL(vkFlushMappedMemoryRanges(createInfo_.LD_->GetHandle(), 1, &memoryRange),
				"Error while Flushing buffer data.");

			{
				vkUnmapMemory(createInfo_.LD_->GetHandle(), GetHandle());
			}
		}

		void Write(Common::Size offsetInBytes, const void* memory, Common::Size bytesNumber) noexcept {

			ASSERT(memory != nullptr);
			ASSERT(bytesNumber > 0);
			ASSERT_FMSG(offsetInBytes + bytesNumber <= GetSize(), "Attempt to write to device memory more or less bytes than device memory size.");

			void* pointerToMappedMemory = nullptr;
			{
				VK_CALL(vkMapMemory(
					createInfo_.LD_->GetHandle(),
					GetHandle(), 
					offsetInBytes,
					bytesNumber,
					0, 
					&pointerToMappedMemory),
					"Error while mapping buffer to device memory.");
			}
			std::memcpy(pointerToMappedMemory, memory, (size_t)bytesNumber);
			{
				vkUnmapMemory(createInfo_.LD_->GetHandle(), GetHandle());
			}
		}

		void Read(Common::Size offsetInBytes, void* memory, Common::Size bytesNumber) {

			ASSERT(memory != nullptr);
			ASSERT(bytesNumber > 0);
			ASSERT_FMSG(offsetInBytes + bytesNumber <= GetSize(), "Attempt to read device memory more or less bytes than device memory size.");

			void* pointerToMappedMemory = nullptr;
			{
				VK_CALL(vkMapMemory(
					createInfo_.LD_->GetHandle(),
					GetHandle(),
					offsetInBytes,
					bytesNumber,
					0,
					&pointerToMappedMemory),
					"Error while mapping buffer to device memory.");
			}
			std::memcpy(memory, pointerToMappedMemory, (size_t)bytesNumber);
			{
				vkUnmapMemory(createInfo_.LD_->GetHandle(), GetHandle());
			}
		}

		~DeviceMemory() noexcept {
			OS::Assert(!IsNullHandle());
			vkFreeMemory(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
			SetNullHandle();
		}

	private:
		CreateInfo createInfo_;
	};

}