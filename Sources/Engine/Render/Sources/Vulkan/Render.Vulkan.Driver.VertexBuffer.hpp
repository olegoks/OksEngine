#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

export module OksEngine.Render.Vulkan.Driver.VertexBuffer;

import OksEngine.Common.Types;

import OksEngine.Render.Vulkan.Common;
import OksEngine.Render.Vulkan.Driver.Buffer;
import OksEngine.Render.Vulkan.Driver.PhysicalDevice;
import OksEngine.Render.Vulkan.Driver.LogicDevice;

namespace Render::Vulkan {

	class VertexBuffer : public Buffer {
	public:
		VertexBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size verticesNumber) :
			Buffer{ Buffer::CreateInfo {
			physicalDevice,
			logicDevice,
			verticesNumber * sizeof(Vertex3fnñt),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
		}, verticesNumber_{ verticesNumber } { }

		void Fill(Vertex3fnñt* vertices) {
			Buffer::Fill(vertices);
		}

		Common::Size GetSize() const {
			return verticesNumber_;
		}

	private:
		const Common::Size verticesNumber_ = 0;
	};

}