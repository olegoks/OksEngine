#pragma once

#include <memory>

#include <Render.Vulkan.Driver.VertexBuffer.hpp>
#include <Render.Vulkan.Driver.IndexBuffer.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>
#include <Render.Vulkan.Driver.CommandBuffer.hpp>

namespace Render::Vulkan {

	class TexturedShape {
	public:

		struct CreateInfo {
			std::shared_ptr<VertexBuffer<RAL::Vertex3fnt>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<RAL::Index16>> indexBuffer_ = nullptr;
			std::shared_ptr<RAL::Texture> texture_ = nullptr;
		};

		TexturedShape(const CreateInfo& createInfo) : 
			vertexBuffer_{ createInfo.vertexBuffer_ },
			indexBuffer_{ createInfo.indexBuffer_ }
			 {

		}
	private:
		std::shared_ptr<VertexBuffer<RAL::Vertex3fnt>> vertexBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer<RAL::Index16>> indexBuffer_ = nullptr;
	};

	class ColoredShape {
	public:

		struct CreateInfo {
			std::shared_ptr<VertexBuffer<RAL::Vertex3fnc>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<RAL::Index16>> indexBuffer_ = nullptr;
		};

		ColoredShape(const CreateInfo& createInfo) noexcept :
			vertexBuffer_{ createInfo.vertexBuffer_ },
			indexBuffer_{ createInfo.indexBuffer_ } {

			//if (createInfo.shape_->GetVertexType() == RAL::VertexType::V3f_N3f_C3f) {

			//	DS::Vector<RAL::Vertex3fnc> vertices;
			//	createInfo.shape_->ForEachVertex<RAL::Vertex3fnc>([&vertices](const RAL::Vertex3fnc& vertex) {
			//			vertices.PushBack(vertex);
			//		});

			//	auto vertexStagingBuffer = std::make_shared<StagingBuffer>(
			//		createInfo.physicalDevice_,
			//		createInfo.logicDevice_,
			//		vertices.GetSize() * sizeof(RAL::Vertex3fnc));

			//	vertexStagingBuffer->Fill(vertices.GetData());

			//	auto vertex3fncB uffer = std::make_shared<VertexBuffer<Vertex3fnc>>(
			//		createInfo.physicalDevice_,
			//		createInfo.logicDevice_,
			//		vertices.GetSize());

			//	const DS::Vector<RAL::Index16> indices = createInfo.shape_->GetIndices();

			//	auto indexStagingBuffer = std::make_shared<StagingBuffer>(
			//		createInfo.physicalDevice_,
			//		createInfo.logicDevice_,
			//		indices.GetSize() * sizeof(RAL::Index16));


			//	indexStagingBuffer->Fill(indices.GetData());
			//	auto indexBuffer = std::make_shared<IndexBuffer<RAL::Index16>>(
			//		createInfo.physicalDevice_,
			//		createInfo.logicDevice_,
			//		indices.GetSize());
			//	
			//	CommandBuffer::CreateInfo commandBufferCreateInfo;
			//	{
			//		commandBufferCreateInfo.logicDevice_ = createInfo.logicDevice_;
			//		commandBufferCreateInfo.commandPool_ = createInfo.commandPool_;
			//	}
			//	
			//	auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
			//	commandBuffer->Begin();
			//	commandBuffer->Copy(vertexStagingBuffer, vertex3fncBuffer);
			//	commandBuffer->Copy(indexStagingBuffer, indexBuffer);
			//	commandBuffer->End();

			//}

			
			

			

			//auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(RAL::Index16));
			//indexStagingBuffer->Fill(indices);
			//auto indexBuffer = std::make_shared<IndexBuffer<RAL::Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			//DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);

			//vertexBuffers_.PushBack(vertex3fncBuffer);
			//indexBuffers_.PushBack(indexBuffer);
		}

		auto GetVertexBuffer() {
			return vertexBuffer_;
		}

		auto GetIndexBuffer() {
			return indexBuffer_;
		}

	private:
		std::shared_ptr<VertexBuffer<RAL::Vertex3fnc>> vertexBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer<RAL::Index16>> indexBuffer_ = nullptr;
	};

}