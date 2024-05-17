#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

export module OksEngine.Render.Vulkan.Driver.UniformBuffer;

import OksEngine.Common.Types;

import OksEngine.Render.Vulkan.Common;
import OksEngine.Render.Vulkan.Driver.Buffer;

namespace Render::Vulkan {
	class UniformBuffer : public Buffer {
	public:
		UniformBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size sizeInBytes) :
			Buffer{ Buffer::CreateInfo{ physicalDevice,
			logicDevice,
			sizeInBytes,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } } {

		}
	};
}