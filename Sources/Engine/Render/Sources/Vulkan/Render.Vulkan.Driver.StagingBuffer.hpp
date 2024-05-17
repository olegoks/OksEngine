#pragma once

#include <memory>
#include <vulkan\vulkan.hpp>

export module OksEngine.Render.Vulkan.Driver.StagingBuffer;

import OksEngine.Common.Types;
import OksEngine.Render.Vulkan.Driver.PhysicalDevice;
import OksEngine.Render.Vulkan.Driver.LogicDevice;
import OksEngine.Render.Vulkan.Driver.Buffer;

namespace Render::Vulkan {
	class StagingBuffer : public Buffer {
	public:
		StagingBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size bufferSizeInBytes) :
			Buffer{ Buffer::CreateInfo{
				physicalDevice, logicDevice, bufferSizeInBytes,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } } {

		}
	};
}