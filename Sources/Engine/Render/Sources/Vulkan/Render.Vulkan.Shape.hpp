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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp> 
#include <glm/gtx/quaternion.hpp>

namespace Render::Vulkan {


	struct Transform {
		alignas(16) glm::mat4 model_;
	};

	class ShapeBase {
	public:

		struct CreateInfo {
			glm::mat4 model_;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
		};

		ShapeBase(const CreateInfo& createInfo) :
				id_{ GetNextId() },
			model_{ createInfo.model_ },
			transformBuffer_{ createInfo.transformBuffer_ },
			transformDescriptorSet_{ createInfo.transformDescriptorSet_ }{}

		[[nodiscard]]
		Common::Index GetId() const noexcept {
			return id_;
		}
		[[nodiscard]]
		auto GetTransformDescriptorSet() noexcept {
			return transformDescriptorSet_;
		}

		void SetModelMatrix(const glm::mat4& modelMatrix) {
			model_ = modelMatrix;
			Transform transform{ model_ };
			transformBuffer_->Fill(&transform);
		}

	private:
		[[nodiscard]]
		static inline Common::Index GetNextId() noexcept {
			static Common::Index id = 0;
			return ++id;
		}
		Common::Index id_ = 0;
		glm::mat4 model_;
		std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
		std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;

	};

	template<class TransformType, class VertexType, class IndexType>
	class Shape : public ShapeBase {
	public:

		struct CreateInfo {
			TransformType model_;
			std::shared_ptr<VertexBuffer<VertexType>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<IndexType>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
		};

		Shape(const CreateInfo& createInfo) :
			ShapeBase{
				{
					createInfo.model_,
					createInfo.transformBuffer_,
					createInfo.transformDescriptorSet_
				} },
			vertexBuffer_{ createInfo.vertexBuffer_ },
			indexBuffer_{ createInfo.indexBuffer_ }{

		}

		[[nodiscard]]
		auto GetVertexBuffer() noexcept {
			return vertexBuffer_;
		}

		[[nodiscard]]
		auto GetIndexBuffer() noexcept {
			return indexBuffer_;
		}

	private:
		std::shared_ptr<VertexBuffer<VertexType>> vertexBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer<IndexType>> indexBuffer_ = nullptr;

	};

	class TexturedShape : public Shape<glm::mat4, Vertex3fnt, Index16>{
	public:

		struct CreateInfo {
			glm::mat4 model_;
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


	class UIShape : public Shape<glm::mat3, Vertex2ftc, Index16> {
	public:

		struct CreateInfo {
			glm::mat3 model_;
			std::shared_ptr<VertexBuffer<Vertex2ftc>> vertexBuffer_ = nullptr;
			std::shared_ptr<IndexBuffer<Index16>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
			std::shared_ptr<Texture> texture_ = nullptr;
		};

		UIShape(const CreateInfo& createInfo) :
			Shape{ Shape::CreateInfo{
				glm::mat4{ 1.f },
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

		glm::vec2 scale_;
		glm::vec2 translate_;
		std::shared_ptr<Texture> texture_ = nullptr;
	};

	class ColoredShape : public Shape<glm::mat4, Vertex3fnc, Index16>{
	public:
		struct CreateInfo {
			glm::mat4 model_;
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