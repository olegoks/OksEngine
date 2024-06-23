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

namespace Render::Vulkan {


	class Texture {
	public:
		struct CreateInfo {
			std::shared_ptr<AllocatedTextureImage> textureImage_ = nullptr;
			std::shared_ptr<ImageView> textureImageView_ = nullptr;
			std::shared_ptr<Sampler> sampler = nullptr;
			std::shared_ptr<DescriptorSet> descriptorSet_ = nullptr;
		};

		Texture(const CreateInfo& createInfo) : 
			textureImage_{ createInfo.textureImage_ },
			textureImageView_{ createInfo.textureImageView_ },
			sampler{ createInfo.sampler },
			descriptorSet_{ createInfo.descriptorSet_} {

		}

		[[nodiscard]]
		std::shared_ptr<DescriptorSet> GetDescriptorSet() noexcept {
			return descriptorSet_;
		}

	private:
		std::shared_ptr<AllocatedTextureImage> textureImage_ = nullptr;
		std::shared_ptr<ImageView> textureImageView_ = nullptr;
		std::shared_ptr<Sampler> sampler = nullptr;
		std::shared_ptr<DescriptorSet> descriptorSet_ = nullptr;
	};

	template<class VertexType, class IndexType>
	class BaseShape {
	public:
		BaseShape(
			std::shared_ptr<VertexBuffer<VertexType>> vertexBuffer,
			std::shared_ptr<IndexBuffer<IndexType>> indexBuffer) : 
			vertexBuffer_{ vertexBuffer },
			indexBuffer_{ indexBuffer } { }

		[[nodiscard]]
		std::shared_ptr<VertexBuffer<VertexType>> GetVertexBuffer() noexcept {
			return vertexBuffer_;
		}

		[[nodiscard]]
		std::shared_ptr<IndexBuffer<IndexType>> GetIndexBuffer() noexcept {
			return indexBuffer_;
		}

	private:
		std::shared_ptr<VertexBuffer<VertexType>> vertexBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer<IndexType>> indexBuffer_ = nullptr;
	};

	class TexturedShape : public BaseShape<RAL::Vertex3fnt, RAL::Index16>{
	public:

		struct CreateInfo {
			std::shared_ptr<VertexBuffer<RAL::Vertex3fnt>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<RAL::Index16>> indexBuffer_ = nullptr;
			std::shared_ptr<Texture> texture_ = nullptr;
		};

		TexturedShape(const CreateInfo& createInfo) :
			BaseShape<RAL::Vertex3fnt, RAL::Index16>{
				createInfo.vertexBuffer_,
				createInfo.indexBuffer_ },
				texture_{ createInfo.texture_ } { }

		[[nodiscard]]
		std::shared_ptr<Texture> GetTexture() noexcept {
			return texture_;
		}

		std::shared_ptr<Texture> texture_ = nullptr;
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