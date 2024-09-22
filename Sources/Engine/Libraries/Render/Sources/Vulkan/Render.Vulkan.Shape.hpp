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

		ShapeBase() :
				id_{ GetNextId() }{}

		[[nodiscard]]
		Common::Index GetId() const noexcept {
			return id_;
		}


	private:
		[[nodiscard]]
		static inline Common::Index GetNextId() noexcept {
			static Common::Index id = 0;
			return ++id;
		}
		Common::Index id_ = 0;


	};

	template<class TransformType, class VertexType, class IndexType>
	class Shape : public ShapeBase {
	public:

		struct CreateInfo {
			TransformType model_;
			std::shared_ptr<AllocatedVertexBuffer<VertexType>> vertexBuffer_ = nullptr;
			std::shared_ptr<AllocatedIndexBuffer<IndexType>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
		};

		Shape(const CreateInfo& createInfo) :
			ShapeBase{},
			model_{ createInfo.model_ },
			transformBuffer_{ createInfo.transformBuffer_ },
			transformDescriptorSet_{ createInfo.transformDescriptorSet_ },
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
		TransformType model_;
		std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
		std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
		std::shared_ptr<AllocatedVertexBuffer<VertexType>> vertexBuffer_ = nullptr;
		std::shared_ptr<AllocatedIndexBuffer<IndexType>> indexBuffer_ = nullptr;

	};


	class Mesh {
	public:

		using Id = Common::Id;

		struct ShaderBinding {
			RAL::Driver::UniformBuffer::Id ubId_;
			std::shared_ptr<DescriptorSet> ds_ = nullptr;
			RAL::Texture::Id textureId_ = RAL::Texture::Id::Invalid();
		};

		struct CreateInfo {
			std::shared_ptr<AllocatedVertexBuffer2> vertexBuffer_ = nullptr;
			std::shared_ptr<AllocatedIndexBuffer2> indexBuffer_ = nullptr;
			std::vector<ShaderBinding> shaderBindings_;
			std::string pipelineName_ = "";
			bool draw_ = true;
		};

		Mesh(const CreateInfo& createInfo) : createInfo_{ createInfo }, draw_{ createInfo.draw_ } {

		
		}

		[[nodiscard]]
		auto GetVertexBuffer() noexcept {
			return createInfo_.vertexBuffer_;
		}

		[[nodiscard]]
		auto GetIndexBuffer() noexcept {
			return createInfo_.indexBuffer_;
		}

		const auto& GetShaderBindings() const noexcept {
			return createInfo_.shaderBindings_;
		}

		[[nodiscard]]
		bool IsDraw() const noexcept { return draw_; }
		void StopDrawing() noexcept { draw_ = false; }
		void ResumeDrawing() noexcept { draw_ = true; }

		[[nodiscard]]
		const std::string GetPipelineName() const noexcept {
			return createInfo_.pipelineName_;
		}

		~Mesh() {
		}

	private:
		CreateInfo createInfo_;
		bool draw_ = true;
	};

	class TexturedShape : public Shape<glm::mat4, Vertex3fnt, Index16>{
	public:

		struct CreateInfo {
			glm::mat4 model_;
			std::shared_ptr<AllocatedVertexBuffer<Vertex3fnt>> vertexBuffer_ = nullptr;
			std::shared_ptr<AllocatedIndexBuffer<Index16>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
			RAL::Texture::Id textureId_ = RAL::Texture::Id::Invalid();
		};

		TexturedShape(const CreateInfo& createInfo) :
			Shape{ Shape::CreateInfo{
				createInfo.model_,
				createInfo.vertexBuffer_,
				createInfo.indexBuffer_,
				createInfo.transformBuffer_,
				createInfo.transformDescriptorSet_
				} },
			textureId_{ createInfo.textureId_ } { }

		[[nodiscard]]
		auto GetTexture() noexcept {
			return textureId_;
		}

		RAL::Texture::Id textureId_ = RAL::Texture::Id::Invalid();
	};


	class UIShape : public Shape<glm::mat3, Vertex2ftc, Index16> {
	public:

		struct CreateInfo {
			glm::mat3 model_;
			std::shared_ptr<AllocatedVertexBuffer<Vertex2ftc>> vertexBuffer_ = nullptr;
			std::shared_ptr<AllocatedIndexBuffer<Index16>> indexBuffer_ = nullptr;
			std::shared_ptr<UniformBuffer> transformBuffer_ = nullptr;
			std::shared_ptr<DescriptorSet> transformDescriptorSet_ = nullptr;
			RAL::Texture::Id textureId_ = RAL::Texture::Id::Invalid();
		};

		UIShape(const CreateInfo& createInfo) :
			Shape{ Shape::CreateInfo{
				glm::mat4{ 1.f },
				createInfo.vertexBuffer_,
				createInfo.indexBuffer_,
				createInfo.transformBuffer_,
				createInfo.transformDescriptorSet_
				} },
			textureId_{ createInfo.textureId_ } { }

		[[nodiscard]]
		auto GetTexture() noexcept {
			return textureId_;
		}

		glm::vec2 scale_;
		glm::vec2 translate_;
		RAL::Texture::Id textureId_ = RAL::Texture::Id::Invalid();
	};

	class ColoredShape : public Shape<glm::mat4, Vertex3fnc, Index16>{
	public:
		struct CreateInfo {
			glm::mat4 model_;
			std::shared_ptr<AllocatedVertexBuffer<Vertex3fnc>> vertexBuffer_ = nullptr;
			std::shared_ptr<AllocatedIndexBuffer<Index16>> indexBuffer_ = nullptr;
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