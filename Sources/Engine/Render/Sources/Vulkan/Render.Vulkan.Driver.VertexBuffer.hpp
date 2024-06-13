#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	template<class VertexType>
	class VertexBuffer : public Buffer {
	public:
		VertexBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size verticesNumber) :
			Buffer{ Buffer::CreateInfo {
			physicalDevice,
			logicDevice,
			verticesNumber * sizeof(VertexType),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
		}, verticesNumber_{ verticesNumber } { }

		void Fill(VertexType* vertices) {
			Buffer::Fill(vertices);
		}

		Common::Size GetSize() const {
			return verticesNumber_;
		}

	private:
		const Common::Size verticesNumber_ = 0;
	};

}