#pragma once 

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	template<class IndexType = RAL::Index16>
	class IndexBuffer : public Buffer {
	public:
		IndexBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size indecesNumber) :
			Buffer{ Buffer::CreateInfo{ physicalDevice, logicDevice, indecesNumber * sizeof(IndexType),
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } },
			indecesNumber_{ indecesNumber } {

		}

		[[nodiscard]]
		Common::Size GetIndecesNumber() const { return indecesNumber_; }

	private:
		const Common::Size indecesNumber_ = 0;
	};



	template<class IndexType>
	class AllocatedIndexBuffer : public IndexBuffer<IndexType> {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> logicDevice_;
			std::shared_ptr<CommandPool> commandPool_;
			const IndexType* indices_ = nullptr;
			Common::Size indicesNumber_ = 0;
		};

		AllocatedIndexBuffer(const CreateInfo& createInfo) : 
			IndexBuffer<IndexType>{ createInfo.physicalDevice_, createInfo.logicDevice_, createInfo.indicesNumber_ } {

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(createInfo.physicalDevice_, createInfo.logicDevice_, createInfo.indicesNumber_ * sizeof(IndexType));
			indexStagingBuffer->Fill(createInfo.indices_);
			Buffer::DataCopy(*indexStagingBuffer, *this, createInfo.logicDevice_, createInfo.commandPool_);
		}

	private:
	};

	class IndexBuffer2 : public Buffer {
	public:
		IndexBuffer2(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size indecesNumber, Common::Size indexSize) :
			Buffer{ Buffer::CreateInfo{ physicalDevice, logicDevice, indecesNumber * indexSize,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } },
			indecesNumber_{ indecesNumber } {

		}

		[[nodiscard]]
		Common::Size GetIndecesNumber() const { return indecesNumber_; }

	private:
		const Common::Size indecesNumber_ = 0;
	};


	class AllocatedIndexBuffer2 : public IndexBuffer2 {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> logicDevice_;
			std::shared_ptr<CommandPool> commandPool_;
			const void* indices_ = nullptr;
			Common::Size indicesNumber_ = 0;
			Common::Size indexSize_ = 0;
		};

		AllocatedIndexBuffer2(const CreateInfo& createInfo) :
			IndexBuffer2{ createInfo.physicalDevice_, createInfo.logicDevice_, createInfo.indicesNumber_, createInfo.indexSize_ } {

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(createInfo.physicalDevice_, createInfo.logicDevice_, createInfo.indicesNumber_ * createInfo.indexSize_);
			indexStagingBuffer->Fill(createInfo.indices_);
			Buffer::DataCopy(*indexStagingBuffer, *this, createInfo.logicDevice_, createInfo.commandPool_);
		}

	private:
	};


}