#pragma once

#include <memory>
#include <vulkan\vulkan.hpp>

#include <Common.Types.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>

namespace Render::Vulkan {

	class StagingBuffer : public Buffer {
	public:
		StagingBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size bufferSizeInBytes) :
			Buffer{ Buffer::CreateInfo{
				physicalDevice, logicDevice, bufferSizeInBytes,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, // TODO: for vertex index buffers DST not need
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } } {

		}
	};
}