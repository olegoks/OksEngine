#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>

namespace Render::Vulkan {

	template<class VertexType>
	class VertexBuffer : public Buffer {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> logicDevice_;
			Common::Size verticesNumber_ = 0;
		};


		VertexBuffer(const CreateInfo& createInfo) :
			Buffer{ Buffer::CreateInfo {
			createInfo.physicalDevice_,
			createInfo.logicDevice_,
			createInfo.verticesNumber_ * sizeof(VertexType),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
		}, verticesNumber_{ createInfo.verticesNumber_ } { }

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

	template<class VertexType>
	class AllocatedVertexBuffer : public VertexBuffer<VertexType> {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> logicDevice_;
			std::shared_ptr<CommandPool> commandPool_;

			const VertexType* vertices_ = nullptr;
			Common::Size verticesNumber_ = 0;
		};

		AllocatedVertexBuffer(const CreateInfo& createInfo) : 
			VertexBuffer<VertexType>{
			createInfo.physicalDevice_,
			createInfo.logicDevice_,
			createInfo.verticesNumber_ 
		} {

			auto vertexStagingBuffer = std::make_shared<StagingBuffer>(createInfo.physicalDevice_, createInfo.logicDevice_, createInfo.verticesNumber_ * sizeof(VertexType));
			vertexStagingBuffer->Fill(createInfo.vertices_);
			Buffer::DataCopy(*vertexStagingBuffer, *this, createInfo.logicDevice_, createInfo.commandPool_);

		}

	private:
	};

}