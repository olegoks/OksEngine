#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>

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