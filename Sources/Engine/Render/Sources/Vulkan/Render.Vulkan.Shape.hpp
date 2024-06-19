#pragma once

#include <memory>

#include <Render.Vulkan.Driver.VertexBuffer.hpp>
#include <Render.Vulkan.Driver.IndexBuffer.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>

namespace Render::Vulkan {

	class Shape {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<RAL::Shape> shape_ = nullptr;
		};

		Shape(const CreateInfo& createInfo) {

			if (createInfo.shape_->GetVertexType() == RAL::VertexType::V3f_N3f_C3f) {

				DS::Vector<RAL::Vertex3fnc> vertices;
				createInfo.shape_->ForEachVertex<RAL::Vertex3fnc>([&vertices](const RAL::Vertex3fnc& vertex) {
						vertices.PushBack(vertex);
					});

				auto vertexStagingBuffer = std::make_shared<StagingBuffer>(
					createInfo.physicalDevice_,
					createInfo.logicDevice_,
					vertices.GetSize() * sizeof(RAL::Vertex3fnc));

				vertexStagingBuffer->Fill(vertices.GetData());

				auto vertex3fncBuffer = std::make_shared<VertexBuffer<Vertex3fnc>>(
					createInfo.physicalDevice_,
					createInfo.logicDevice_,
					vertices.GetSize());

				DataCopy(vertexStagingBuffer, vertex3fncBuffer, logicDevice_, commandPool_);
			}

			
			

			

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(RAL::Index16));
			indexStagingBuffer->Fill(indices);
			auto indexBuffer = std::make_shared<IndexBuffer<RAL::Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);

			vertexBuffers_.PushBack(vertex3fncBuffer);
			indexBuffers_.PushBack(indexBuffer);
		}

	private:
		std::shared_ptr<VertexBuffer<RAL::Vertex3fnc>> vertexBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer<RAL::Index16>> indexBuffer_ = nullptr;
	};

}