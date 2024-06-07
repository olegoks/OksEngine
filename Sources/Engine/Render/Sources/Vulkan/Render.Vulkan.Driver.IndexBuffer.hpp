#pragma once 

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class IndexBuffer : public Buffer {
	public:
		IndexBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size indecesNumber) :
			Buffer{ Buffer::CreateInfo{ physicalDevice, logicDevice, indecesNumber * sizeof(uint16_t),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } },
			indecesNumber_{ indecesNumber } {

		}

		[[nodiscard]]
		Common::Size GetIndecesNumber() const { return indecesNumber_; }

	private:
		const Common::Size indecesNumber_ = 0;
	};


}