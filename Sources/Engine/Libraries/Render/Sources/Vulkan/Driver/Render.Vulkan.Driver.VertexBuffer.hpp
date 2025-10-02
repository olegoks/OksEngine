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

	//template<class VertexType>
	//class VertexBuffer : public Buffer {
	//public:

	//	struct CreateInfo {
	//		std::shared_ptr<PhysicalDevice> physicalDevice_;
	//		std::shared_ptr<LogicDevice> LD_;
	//		Common::Size verticesNumber_ = 0;
	//	};


	//	VertexBuffer(const CreateInfo& createInfo) :
	//		Buffer{ Buffer::CreateInfo {
	//		createInfo.physicalDevice_,
	//		createInfo.LD_,
	//		createInfo.verticesNumber_ * sizeof(VertexType),
	//		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
	//	}, verticesNumber_{ createInfo.verticesNumber_ } { }

	//	VertexBuffer(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<LogicDevice> logicDevice, Common::Size verticesNumber) :
	//		Buffer{ Buffer::CreateInfo {
	//		physicalDevice,
	//		logicDevice,
	//		verticesNumber * sizeof(VertexType),
	//		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	//		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
	//	}, verticesNumber_{ verticesNumber } { }

	//	void Fill(VertexType* vertices) {
	//		Buffer::Fill(vertices);
	//	}

	//	Common::Size GetSize() const {
	//		return verticesNumber_;
	//	}

	//private:
	//	const Common::Size verticesNumber_ = 0;
	//};

	class VertexBuffer2 : public Buffer {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> LD_;
			Common::Size verticesNumber_ = 0;
			Common::Size vertexSize_ = 0;
		};


		VertexBuffer2(const CreateInfo& createInfo) :
			Buffer{ Buffer::CreateInfo {
			createInfo.physicalDevice_,
			createInfo.LD_,
			createInfo.verticesNumber_ * createInfo.vertexSize_,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT }
		}, createInfo_{ createInfo } { }

		void Fill(Common::Index firstVertex, void* vertices, Common::Size vertexCount) {
			Buffer::Write(firstVertex * createInfo_.vertexSize_, vertices, vertexCount * createInfo_.vertexSize_);
		}

		[[nodiscard]]
		Common::Size GetSize() const noexcept {
			return createInfo_.verticesNumber_;
		}

		[[nodiscard]]
		Common::Size GetVertexSize() const noexcept {
			return createInfo_.vertexSize_;
		}

		[[nodiscard]]
		Common::Size GetVerticesNumber() const noexcept {
			return createInfo_.verticesNumber_;
		}
		
	protected:
		CreateInfo createInfo_;
	};


	//template<class VertexType>
	//class AllocatedVertexBuffer : public VertexBuffer<VertexType> {
	//public:

	//	struct CreateInfo {
	//		std::shared_ptr<PhysicalDevice> physicalDevice_;
	//		std::shared_ptr<LogicDevice> LD_;
	//		std::shared_ptr<CommandPool> commandPool_;

	//		const VertexType* vertices_ = nullptr;
	//		Common::Size verticesNumber_ = 0;
	//	};

	//	AllocatedVertexBuffer(const CreateInfo& createInfo) : 
	//		VertexBuffer<VertexType>{
	//		createInfo.physicalDevice_,
	//		createInfo.LD_,
	//		createInfo.verticesNumber_ 
	//	} {

	//		auto vertexStagingBuffer = std::make_shared<StagingBuffer>(createInfo.physicalDevice_, createInfo.LD_, createInfo.verticesNumber_ * sizeof(VertexType));
	//		vertexStagingBuffer->Fill(createInfo.vertices_);
	//		Buffer::DataCopy(*vertexStagingBuffer, *this, createInfo.LD_, createInfo.commandPool_);

	//	}

	//private:
	//};


	class HostVisibleVertexBuffer : public VertexBuffer2 {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			std::shared_ptr<LogicDevice> LD_;
			std::shared_ptr<CommandPool> commandPool_;

			//const void* vertices_ = nullptr;
			Common::Size vertexSize_ = 0;
			Common::Size verticesNumber_ = 0;
		};

		HostVisibleVertexBuffer(const CreateInfo& createInfo) :
			VertexBuffer2{ VertexBuffer2::CreateInfo{
				createInfo.physicalDevice_,
				createInfo.LD_,
				createInfo.verticesNumber_,
				createInfo.vertexSize_ } } {


			stagingBuffer_ = std::make_shared<StagingBuffer>(
				createInfo_.physicalDevice_,
				createInfo_.LD_,
				createInfo_.verticesNumber_ * createInfo_.vertexSize_);
			stagingBuffer_->Allocate();

		}


		void FillData(Common::Size verticesOffset, const void* vertices, Common::Size verticesNumber, std::shared_ptr<CommandPool> commandPool) {
			stagingBuffer_->Write(
				verticesOffset,
				vertices,
				createInfo_.vertexSize_ * verticesNumber);
			Buffer::DataCopy(*stagingBuffer_, *this, 0, verticesOffset * createInfo_.vertexSize_, verticesNumber * createInfo_.vertexSize_, createInfo_.LD_, commandPool);
		}

	private:

		std::shared_ptr<StagingBuffer> stagingBuffer_ = nullptr;

	};

}