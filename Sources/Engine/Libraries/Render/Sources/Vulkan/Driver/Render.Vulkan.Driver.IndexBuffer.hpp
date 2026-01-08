#pragma once 

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	//template<class IndexType = RAL::Index16>
	//class IndexBuffer : public Buffer {
	//public:
	//	IndexBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size indecesNumber) :
	//		Buffer{ Buffer::CreateInfo{ physicalDevice, logicDevice, indecesNumber * sizeof(IndexType),
	//		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT } },
	//		indecesNumber_{ indecesNumber } {

	//	}

	//	[[nodiscard]]
	//	Common::Size GetIndecesNumber() const { return indecesNumber_; }

	//private:
	//	const Common::Size indecesNumber_ = 0;
	//};



	//template<class IndexType>
	//class AllocatedIndexBuffer : public IndexBuffer<IndexType> {
	//public:

	//	struct CreateInfo {
	//		std::shared_ptr<PhysicalDevice> physicalDevice_;
	//		std::shared_ptr<LogicDevice> LD_;
	//		std::shared_ptr<CommandPool> commandPool_;
	//		const IndexType* indices_ = nullptr;
	//		Common::Size indicesNumber_ = 0;
	//	};

	//	AllocatedIndexBuffer(const CreateInfo& createInfo) : 
	//		IndexBuffer<IndexType>{ createInfo.physicalDevice_, createInfo.LD_, createInfo.indicesNumber_ } {

	//		auto indexStagingBuffer = std::make_shared<StagingBuffer>(createInfo.physicalDevice_, createInfo.LD_, createInfo.indicesNumber_ * sizeof(IndexType));
	//		indexStagingBuffer->Fill(createInfo.indices_);
	//		Buffer::DataCopy(*indexStagingBuffer, *this, createInfo.LD_, createInfo.commandPool_);
	//	}

	//private:
	//};

	class IndexBuffer2 : public Buffer {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> PD_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			Common::Size indicesNumber_ = 0;
			Common::Size indexSize_ = 0;
		};

		IndexBuffer2(const CreateInfo& createInfo) :
			Buffer{ Buffer::CreateInfo{ createInfo.PD_, createInfo.LD_, createInfo.indicesNumber_ * createInfo.indexSize_,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT } },
			createInfo_{ createInfo } {

		}

		[[nodiscard]]
		Common::Size GetIndicesNumber() const { return createInfo_.indicesNumber_; }

		[[nodiscard]]
		Common::Size GetIndexSize() const noexcept { return createInfo_.indexSize_; }

	protected:
		CreateInfo createInfo_;
	};


	class HostVisibleIndexBuffer : public IndexBuffer2 {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> LD_;
			std::shared_ptr<CommandPool> commandPool_;
			Common::Size indicesNumber_ = 0;
			Common::Size indexSize_ = 0;
		};

		HostVisibleIndexBuffer(const CreateInfo& createInfo) :
			IndexBuffer2{ IndexBuffer2::CreateInfo{ createInfo.physicalDevice_, createInfo.LD_, createInfo.indicesNumber_, createInfo.indexSize_ } } {

			OS::Assert(createInfo.indicesNumber_ >= 3);


			stagingBuffer_ = std::make_shared<StagingBuffer>(createInfo_.PD_, createInfo_.LD_, createInfo_.indicesNumber_ * GetIndexSize());
			stagingBuffer_->Allocate();

		}

		void FillData(Common::Size offset, const void* indices, Common::Size indicesNumber, std::shared_ptr<CommandPool> commandPool) {

			stagingBuffer_->Write(0, indices, indicesNumber * GetIndexSize());

			Buffer::DataCopy(
				*stagingBuffer_, *this,
				0, offset,
				indicesNumber * GetIndexSize(),
				createInfo_.LD_,
				commandPool);
		}

	private:

		std::shared_ptr<StagingBuffer> stagingBuffer_ = nullptr;

	};


}