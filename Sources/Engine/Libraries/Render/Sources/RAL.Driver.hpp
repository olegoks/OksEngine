#pragma once

#include <memory>
#include <string>
#include <vector>
#include <any>

#include <OS.Assert.hpp>
#include <Common.hpp>
#include <Common.Types.hpp>
#include <Math.Vector.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Camera.hpp>
#include <RAL.Light.hpp>
#include <RAL.Common.hpp>
#include <RAL.Shader.hpp>
#include <Geometry.Vertex.hpp>
#include <DataStructures.Vector.hpp>
#include <GLFW\glfw3.h>
#include <RAL.Model.hpp>

namespace RAL {

	template<class VertexType, class IndexType>
	class Vertices {
	public:

		void Add(const VertexType& vertex) noexcept {
			Add(&vertex, 1);
		}

		void Add(const VertexType* vertices, Common::Size verticesNumber) noexcept {

			OS::Assert(vertices != nullptr);

			const Common::Size offset = GetVerticesNumber();
			const Common::Size newSize = GetVerticesNumber() + verticesNumber;
			vertices_.resize(newSize);
			std::memcpy(vertices_.data() + offset, vertices, verticesNumber * sizeof(VertexType));
			verticesNumber_ += verticesNumber;
		}

		[[nodiscard]]
		Common::Size GetVerticesNumber() const noexcept { return verticesNumber_; }

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept { return GetVerticesNumber() * sizeof(VertexType); }


		void Clear() noexcept {
			verticesNumber_ = 0;
		}

		[[nodiscard]]
		const VertexType* GetData() const noexcept { return vertices_.data(); }

		using iterator = std::vector<VertexType>::iterator;
		using const_iterator = const iterator;

		iterator begin() noexcept { return vertices_.begin(); }
		iterator end() noexcept { return vertices_.end(); };

		const_iterator begin() const noexcept { return const_cast<Vertices*>(this)->begin(); }
		const_iterator end() const noexcept { return const_cast<Vertices*>(this)->end(); }

	private:
		Common::Size verticesNumber_ = 0;
		std::vector<VertexType> vertices_;
	};

	template<class IndexType = Index16>
	class Indices {
	public:

		void Add(const IndexType* indices, Common::Size indicesNumber, Common::Size verticesNumber) noexcept {

			OS::Assert(indices != nullptr);

			const Common::Size offset = GetIndicesNumber();
			const Common::Size newSize = GetIndicesNumber() + indicesNumber;
			indices_.resize(newSize);
			std::memcpy(indices_.data() + offset, indices, indicesNumber * sizeof(IndexType));

			OS::AssertMessage(verticesNumber <= Common::Limits<IndexType>::Max(), 
				"Number of vertices is more than Index type can contain.");
			for (Common::Index i = offset; i < newSize; i++) {
				indices_[i] += static_cast<IndexType>(GetVerticesNumber());
			}
			verticesNumber_ += verticesNumber;
			indicesNumber_ += indicesNumber;
		}

		void Clear() noexcept {
			verticesNumber_ = 0;
			indicesNumber_ = 0;
		}

		[[nodiscard]]
		Common::Size GetVerticesNumber() const noexcept { return verticesNumber_; }

		[[nodiscard]]
		Common::Size GetIndicesNumber() const noexcept { return indicesNumber_; }

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept { return GetIndicesNumber() * sizeof(IndexType); }

		[[nodiscard]]
		const IndexType* GetData() const noexcept { return indices_.data(); }

	private:
		Common::Size verticesNumber_ = 0;
		Common::Size indicesNumber_ = 0;
		std::vector<IndexType> indices_;
	};

}

namespace RAL {
	
	enum class UISubsystem {
		WinAPI,
		GLFW
	};

	struct RenderSurface {
		std::any param1_;
		std::any param2_;
		std::any param3_;
		std::any param4_;
		glm::u32vec2 size_{ 0, 0 };
		UISubsystem uiSubsystem_;
		//If WINApi
		// param1_ == HWND
		// param2_ == HINSTANCE
		//If GLFW
		// param1_ == GLFWwindow*
		// param2_ == requiredExtensionsCound uint32
		// param3_ == requiredExtensions const char**
	};

	class Driver {
	public:

		struct Pipeline {
			std::string name_ = "No name";
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			bool enableDepthTest_ = true;
		};

		enum class VertexType : Common::UInt64 {
			VF3_NF3_TF2,
			VF2_TF2_CF4,
			Undefined,
			Size
		};

		enum class IndexType : Common::UInt64 {
			UI16,
			Undefined,
			Size
		};

		enum class TopologyType {
			LineList,
			TriangleList,
			Undefined
		};

		enum class FrontFace {
			Clockwise,
			CounterClockwise,
			Undefined
		};

		typedef enum CullMode {
			None,
			Front,
			Back,
			FrontAndBack,
			Undefined
		};

		struct ShaderBinding {
			enum class Type {
				Uniform,
				Sampler,
				Undefined
			};
			enum class Stage {
				VertexShader,
				FragmentShader,
				Undefined
			};
			Common::UInt64 binding_ = 0;
			Type type_ = Type::Undefined;
			Stage stage_ = Stage::Undefined;
		};

		struct Pipeline2 {
			std::string name_ = "";
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			TopologyType topologyType_ = TopologyType::Undefined;
			VertexType vertexType_ = VertexType::Undefined;
			IndexType indexType_ = IndexType::Undefined;
			FrontFace frontFace_ = FrontFace::Undefined;
			CullMode cullMode_ = CullMode::Undefined;
			std::vector<ShaderBinding> shaderBindings_;
			bool enableDepthTest_ = true;
		};

		struct CreateInfo {
			std::shared_ptr<Pipeline> imguiPipeline_ = nullptr;
			std::shared_ptr<Pipeline> linesPipeline_ = nullptr;
			std::shared_ptr<Pipeline> texturedPipeline_ = nullptr;
			std::shared_ptr<Pipeline> flatShadedPipeline_ = nullptr;
			std::vector<Pipeline2> pipelines2_; 
			RenderSurface surface_;
		};

		Driver(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } { }

		virtual void StartRender() = 0;
		virtual void Render() = 0;
		virtual void EndRender() = 0;


		void SetCamera(std::shared_ptr<Camera> camera) noexcept {
			camera_ = camera;
		}

		void AddLight(std::shared_ptr<Light> light) noexcept {
			light_ = light;
		}

		std::shared_ptr<Camera> GetCamera() noexcept {
			return camera_;
		}

		virtual Common::UInt64 DrawIndexed(
			glm::vec2 scale,
			glm::vec2 translate,
			const Vertex2ftc* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber,
			RAL::Texture::Id textureId) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3fnt* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber,
			RAL::Texture::Id textureId) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3fc* vertex,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indeciesNumber) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3f* vertex,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indeciesNumber,
			const Color3f& color) = 0;

		[[nodiscard]]
		virtual Common::Id AddShapeToDraw(
			const std::string& pipelineName,
			const void* transform_,
			Common::Size transformSize,
			const void* vertices,
			Common::Size verticesNumber,
			VertexType vertexType,
			const void* indices,
			Common::Size indicesNumber,
			IndexType indexType,
			RAL::Texture::Id textureId) = 0;

		virtual void ResumeShapeDrawing(Common::Id shapeId) = 0;

		virtual void StopShapeDrawing(Common::Id shapeId) = 0;

		virtual void RemoveShapeFromDrawing(Common::Id shapeId) = 0;

		/* Textures */
		[[nodiscard]]
		virtual Texture::Id CreateTexture(const RAL::Texture::CreateInfo& createInfo) = 0;
		//virtual 
		virtual void SetModelMatrix(
			Common::Index shapeIndex,
			const glm::mat4& modelMatrix) = 0;

		virtual ~Driver() = default;

	protected:
		CreateInfo createInfo_;
		std::shared_ptr<Light> light_ = nullptr;
		std::shared_ptr<Camera> camera_ = nullptr;
	};




}