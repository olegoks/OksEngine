#pragma once

#include <memory>

#include <Render.Vulkan.Driver.Sampler.hpp>
#include <Render.Vulkan.Driver.Image.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
#include <Render.Vulkan.Driver.VertexBuffer.hpp>
#include <Render.Vulkan.Driver.IndexBuffer.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>
#include <Render.Vulkan.Driver.CommandBuffer.hpp>
#include <Render.Vulkan.Driver.Texture.hpp>

namespace Render::Vulkan {


	struct Transform {
		alignas(16) Math::Matrix4x4f model_;
	};


	template<class VertexType, class IndexType>
	class Shape {
	public:

		struct CreateInfo {
			Math::Matrix4x4f model_ = Math::Matrix4x4f::GetIdentity();
			std::shared_ptr<VertexBuffer<VertexType>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<IndexType>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> modelDataDescriptorSet_ = nullptr;
		};

		Shape(const CreateInfo& createInfo) : 
			id_{ GetNextId() },
			vertexBuffer_{ createInfo.vertexBuffer_ },
			indexBuffer_{ createInfo.indexBuffer_ },
			transformBuffer_{ createInfo.transformBuffer_ },
			bufferDescriptorSet_{ createInfo.modelDataDescriptorSet_ } {

		}

		[[nodiscard]]
		auto GetVertexBuffer() noexcept {
			return vertexBuffer_;
		}

		[[nodiscard]]
		auto GetIndexBuffer() noexcept {
			return indexBuffer_;
		}

		[[nodiscard]]
		auto GetTransformDescriptorSet() noexcept {
			return bufferDescriptorSet_;
		}

		void Offset(const Vector3f& offset) {
			model_ = model_ * Math::Matrix4x4f::GetTranslate(offset);
			Transform transform{ model_ };
			transformBuffer_->Fill(transform);
		}

		void SetPosition(const Vector3f& position) {
			auto matrixPosition = Math::Matrix4x4f::GetTranslate(position);
			Transform transform{ matrixPosition };
			model_ = matrixPosition;
			transformBuffer_->Fill(&transform);
		}

		void SetModelMatrix(const Math::Matrix4x4f& modelMatrix) {
			//auto fixed = model_;
			//fixed[1][3] += 0.001;
			model_ = modelMatrix;//model_ * Math::Matrix4x4f::GetRotate(0.1 * 30.f, { 1.f, 1.f, 0.f }); /** Math::Matrix4x4f::GetRotate(90.f, { 1.f, 0.f, 0.f });*///modelMatrix;
			Transform transform{ model_ };
			transformBuffer_->Fill(&transform);
		}

		//void Rotate(const Vector3f& vectorAround, Math::Angle angle) {
		//	const Math::Matrix4x4f rotateMatrix = Math::Matrix4x4f::GetRotate(angle, vectorAround);
		//	model_ = model_ * rotateMatrix;
		//	Transform transform{ model_ };
		//	transformBuffer_->Fill(&transform);
		//}

		[[nodiscard]]
		Common::Index GetId() const noexcept {
			return id_;
		}
	private:
		[[nodiscard]]
		Common::Index GetNextId() const noexcept {
			static Common::Index id = 0;
			return ++id;
		}

	private:
		Math::Matrix4x4f model_;
		Common::Index id_ = 0;
		std::shared_ptr<VertexBuffer<VertexType>> vertexBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer<IndexType>> indexBuffer_ = nullptr;
		std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
		std::shared_ptr<DescriptorSet> bufferDescriptorSet_ = nullptr;
	};

	class TexturedShape : public Shape<Vertex3fnt, Index16>{
	public:

		struct CreateInfo {
			Math::Matrix4x4f model_ = Math::Matrix4x4f::GetIdentity();
			std::shared_ptr<VertexBuffer<Vertex3fnt>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<Index16>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
			std::shared_ptr<Texture> texture_ = nullptr;
		};

		TexturedShape(const CreateInfo& createInfo) :
			Shape{ Shape::CreateInfo{
				createInfo.model_,
				createInfo.vertexBuffer_,
				createInfo.indexBuffer_,
				createInfo.transformBuffer_,
				createInfo.transformDescriptorSet_
				} },
				texture_{ createInfo.texture_ } { }

		[[nodiscard]]
		std::shared_ptr<Texture> GetTexture() noexcept {
			return texture_;
		}

		std::shared_ptr<Texture> texture_ = nullptr;
	};

	class ColoredShape : public Shape<Vertex3fnc, Index16>{
	public:
		struct CreateInfo {
			Math::Matrix4x4f model_ = Math::Matrix4x4f::GetIdentity();
			std::shared_ptr<VertexBuffer<Vertex3fnc>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<Index16>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> modelDataDescriptorSet;
		};

		ColoredShape(const CreateInfo& createInfo) noexcept :
			Shape{ 
				Shape::CreateInfo{
					createInfo.model_,
					createInfo.vertexBuffer_,
					createInfo.indexBuffer_,
					createInfo.transformBuffer_,
					createInfo.modelDataDescriptorSet
				} } { }

	private:
		std::shared_ptr<VertexBuffer<Vertex3fnc>> vertexBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer<Index16>> indexBuffer_ = nullptr;
	};

}