#pragma once

#include <memory>
#include <string>
#include <vector>
#include <any>

#include <OS.Assert.hpp>
#include <Common.hpp>
#include <Common.Types.hpp>
#include <RAL.Common.hpp>
#include <RAL.Shader.hpp>
#include <RAL.Texture.hpp>

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

		enum CullMode {
			None,
			Front,
			Back,
			FrontAndBack,
			Undefined
		};

		class UniformBuffer {
		public:
			using Id = Common::Id;
			enum class Type {
				Mutable,
				Const,
				Undefined
			};
			struct CreateInfo {
				Common::Size size_ = 0;
				Type type_ = Type::Undefined;
			};
		};

		class VertexBuffer {
		public:
			using Id = Common::Id;
			enum class Type {
				Mutable,
				Const,
				Undefined
			};
			struct CreateInfo {
				Common::Size verticesNumber_ = 0;
				VertexType vertexType_ = VertexType::Undefined;
				Type type_ = Type::Undefined;
			};
		};

		class IndexBuffer {
		public:
			using Id = Common::Id;
			enum class Type {
				Mutable,
				Const,
				Undefined
			};
			struct CreateInfo {
				Common::Size size_ = 0;
				IndexType indexType_ = IndexType::Undefined;
				Type type_ = Type::Undefined;
			};
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
			struct Data {
				Type type_ = Type::Undefined;
				Stage stage_ = Stage::Undefined;
				RAL::Driver::UniformBuffer::Id uniformBufferId_;
				RAL::Texture::Id textureId_;
			};
			struct Layout {
				std::string name_ = "";
				Common::UInt32 binding_ = 0;
				Type type_ = Type::Undefined;
				Stage stage_ = Stage::Undefined;
			};
		};



		struct PipelineDescription {
			using Id = Common::Id;
			std::string name_ = "";
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			TopologyType topologyType_ = TopologyType::Undefined;
			VertexType vertexType_ = VertexType::Undefined;
			IndexType indexType_ = IndexType::Undefined;
			FrontFace frontFace_ = FrontFace::Undefined;
			CullMode cullMode_ = CullMode::Undefined;
			std::vector<ShaderBinding::Layout> shaderBindings_;
			bool enableDepthTest_ = true;
		};


		using UB = UniformBuffer;

		struct CreateInfo {
			std::map<std::string, PipelineDescription> namePipelineDescriptions_;
			RenderSurface surface_;
		};

		Driver(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } { }

		virtual void StartRender() = 0;
		virtual void Render() = 0;
		virtual void EndRender() = 0;

		[[nodiscard]]
		virtual Common::Id DrawMesh(
			const std::string& pipelineName,
			const void* vertices,
			Common::Size verticesNumber,
			VertexType vertexType,
			const void* indices,
			Common::Size indicesNumber,
			IndexType indexType,
			const std::vector<RAL::Driver::ShaderBinding::Data>& bindingData) = 0;

		[[nodiscard]]
		virtual Common::Id DrawMesh(
			const std::string& pipelineName,
			VertexBuffer::Id VBId,
			IndexBuffer::Id IBId,
			const std::vector<RAL::Driver::ShaderBinding::Data>& bindingData) = 0;


		virtual void ResumeMeshDrawing(Common::Id shapeId) = 0;

		virtual void StopMeshDrawing(Common::Id shapeId) = 0;

		virtual void RemoveMeshFromDrawing(Common::Id shapeId) = 0;

		virtual void FrameBufferResize(glm::u32vec2 newSize) = 0;

		/* Textures */
		[[nodiscard]]
		virtual Texture::Id CreateDiffuseMap(const RAL::Texture::CreateInfo& createInfo) = 0;
		[[nodiscard]]
		virtual bool IsTextureExist(RAL::Texture::Id textureId) const noexcept = 0;

		[[nodiscard]]
		virtual UniformBuffer::Id CreateUniformBuffer(const UniformBuffer::CreateInfo& createInfo) = 0;

		[[nodiscard]]
		virtual VertexBuffer::Id CreateVertexBuffer(const VertexBuffer::CreateInfo& createInfo) = 0;
		virtual void FillVertexBuffer(VertexBuffer::Id id, Common::Size offset, const void* vertices, Common::Size verticesNumber) = 0;

		[[nodiscard]]
		virtual IndexBuffer::Id CreateIndexBuffer(const IndexBuffer::CreateInfo& createInfo) = 0;
		virtual void FillIndexBuffer(VertexBuffer::Id id, Common::Size offset, const void* vertices, Common::Size verticesNumber) = 0;

		[[nodiscard]]
		virtual void FillUniformBuffer(UniformBuffer::Id UBId, void* data) = 0;

		virtual ~Driver() = default;

	protected:
		CreateInfo createInfo_;
	};




}