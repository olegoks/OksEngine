#pragma once

#include <memory>
#include <string>
#include <vector>
#include <any>
#include <map>

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

		enum class Format : Common::UInt64 {
			BGRA_32,
			RGBA_32,
			Undefined
		};

		enum class VertexType : Common::UInt64 {
			VF3_NF3_TF2,
			VF3_CF4,
			VF2_TF2_CF4,
			Undefined,
			Size
		};

		enum class IndexType : Common::UInt64 {
			UI16,
			UI32,
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

		enum class Stage {
			VertexShader,
			FragmentShader,
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
			struct CreateInfo1 {
				Common::Size verticesNumber_ = 0;
				VertexType vertexType_ = VertexType::Undefined;
				Type type_ = Type::Undefined;
			};
			struct CreateInfo2 {
				Common::Size verticesNumber_ = 0;
				Common::Size vertexSize_ = 0;
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
			struct CreateInfo1 {
				Common::Size indicesNumber_ = 0;
				IndexType indexType_ = IndexType::Undefined;
				Type type_ = Type::Undefined;
			};
			struct CreateInfo2 {
				Common::Size indicesNumber_ = 0;
				Common::Size indexSize_ = 0;
				Type type_ = Type::Undefined;
			};
		};


		struct ShaderBinding {
			enum class Type {
				Uniform,
				Sampler,
				InputAttachment,
				Undefined
			};
			enum class Stage {
				VertexShader,
				FragmentShader,
				Undefined
			};
			struct Data {
				Type type_ = Type::Undefined;
				Common::UInt64 binding_ = Common::Limits<Common::UInt64>::Max();
				Stage stage_ = Stage::Undefined;
				RAL::Driver::UniformBuffer::Id uniformBufferId_ = RAL::Driver::UniformBuffer::Id::Invalid();
				RAL::Texture::Id textureId_ = RAL::Texture::Id::Invalid();
			};
			struct Layout {
				std::string name_ = "";
				Common::UInt32 binding_ = 0;
				Type type_ = Type::Undefined;
				Stage stage_ = Stage::Undefined;
			};
		};

		struct DepthBuffer {
			enum class CompareOperation {
				Never,
				Less,
				Equal,
				LessOrEqual,
				Greater,
				NotEqual,
				GreaterOrEqual,
				Always,
				Undefined
			};
		};
		using DB = DepthBuffer;

		//struct Resource {

		//	using Id = Common::Id;
		//	enum class Type {
		//		Uniform,
		//		Sampler,
		//		InputAttachment,
		//		Undefined
		//	};
		//	enum class Mutability {
		//		Const,
		//		Mutable,
		//		Undefined
		//	};

		//	struct Description {
		//		Type type_ = Type::Undefined;
		//		Stage stage_ = Stage::Undefined;
		//	};
		//	using Desc = Description;

		//	Type type_ = Type::Undefined;

		//	RAL::Driver::UniformBuffer::Id ubid_ = RAL::Driver::UniformBuffer::Id::Invalid();
		//	RAL::Texture::Id textureId_;

		//	struct CreateInfo {
		//		Mutability mutability_ = Mutability::Undefined;
		//		Stage stage_ = Stage::Undefined;
		//		Type type_ = Type::Undefined;
		//		
		//		RAL::Driver::UniformBuffer::Id ubid_ = RAL::Driver::UniformBuffer::Id::Invalid();
		//		RAL::Texture::Id textureId_;

		//	};
		//	using CI = CreateInfo;

		//};
		//using Res = Resource;
		virtual std::shared_ptr<RAL::Shader> CreateShader(const RAL::Shader::CreateInfo& createInfo) const = 0;

		struct ResourceSet {
			using Id = Common::Id;

			struct Binding {
				Stage stage_ = Stage::Undefined;
				Common::UInt64 binding_ = Common::Limits<Common::UInt64>::Max();
				//Uniform buffer.
				RAL::Driver::UB::Id ubid_ = RAL::Driver::UB::Id::Invalid();
				Common::UInt64 offset_ = 0;
				Common::UInt64 size_ = Common::Limits<Common::UInt64>::Max();
				 
				//Combined sampler.
				RAL::Texture::Id textureId_ = RAL::Texture::Id::Invalid();

			};

			struct CreateInfo1 {
				Binding bindings_[4];
				Common::UInt64 bindingsNumber_ = Common::Limits<Common::UInt64>::Max();
			};
			using CI1 = CreateInfo1;

			using CreateInfo2 = Binding;
			using CI2 = Binding;
			
		};
		using RS = ResourceSet;
		using Resource = RS;

		virtual ResourceSet::Id CreateResourceSet(const ResourceSet::CI1& createInfo) = 0;
		virtual Resource::Id CreateResource(const Resource::CI2& createInfo) = 0;

		struct RenderPass {
			using Id = Common::Id;

			struct AttachmentUsage {

				//What to do with attachment in the begin of render pass.
				enum class LoadOperation {
					Load,		//Save current data. 
					Clear,		//Clear
					Ignore,		//Ignore old data(optimization)
					Undefined
				};

				//What to do with attachment in the end of render pass.
				enum class StoreOperation {
					Store,		//Save for smt(for example for display)
					DontStore,	//Dont save(if data dont need after render pass)
					Undefined
				};

				enum class State {
					DataIsUndefined,
					DataForAllOperations, //Not optimized
					DataForColorWrite,
					DataForShaderRead,
					DataForCopyingSource,
					DataForCopyingDestination,
					DataForPresentation,
					Undefined
				};

				using Id = Common::Id;
				Format format_ = Format::Undefined;
				State initialState_ = State::Undefined;						//State of attachment before render pass. Excepted by render pass.
				LoadOperation loadOperation_ = LoadOperation::Undefined;	//What to do with attachment in the begin of render pass.		
				StoreOperation storeOperation_ = StoreOperation::Undefined;	//What to do with attachment in the end of render pass.
				State finalState_ = State::Undefined;						//State of attachment after render pass.  Will be set by render pass.
				
			};

			struct AttachmentSet {
				using Id = Common::Id;
				struct CreateInfo {
					RenderPass::Id rpId_ = Common::Limits<RenderPass::Id>::Max();
					std::vector<Texture::Id> textures_;
				};
				using CI = CreateInfo;
			};

			using RS = ResourceSet;
			
			struct Subpass {
				std::vector<Common::UInt32> inputAttachments_;
				std::vector<Common::UInt32> colorAttachments_;
				Common::UInt32 resolveAttachment_ = Common::Limits<Common::UInt32>::Max();
				Common::UInt32 depthStencilAttachment_ = Common::Limits<Common::UInt32>::Max();
			};

			struct CreateInfo {
				std::string name_ = "No name";
				std::vector<AttachmentUsage> attachments_;
				std::vector<Subpass> subpasses_;
			};
			using CI = CreateInfo;
		};

		using RP = RenderPass;

		virtual std::pair<Common::UInt32, Common::UInt32> GetSwapChainTextureSize() = 0;

		virtual RP::Id CreateRenderPass(const RP::CI& rpCI) = 0;
		virtual RP::AttachmentSet::Id CreateAttachmentSet(const RP::AttachmentSet::CI& attachmentSetCI) = 0;
		//Set of images, buffers... that render pass uses.(swapchain, gbuffer, depthbuffer...)
		//virtual RenderPass::Id CreateRenderPassResourceSet(const RP::RS::CI& rsCI) = 0;


		struct Pipeline {
			using Id = Common::Id;

			struct CreateInfo {
				std::string name_ = "";
				RP::Id renderPassId_ = RP::Id::Invalid();
				std::shared_ptr<Shader> vertexShader_ = nullptr;
				std::shared_ptr<Shader> fragmentShader_ = nullptr;
				TopologyType topologyType_ = TopologyType::Undefined;
				VertexType vertexType_ = VertexType::Undefined;
				IndexType indexType_ = IndexType::Undefined;
				FrontFace frontFace_ = FrontFace::Undefined;
				CullMode cullMode_ = CullMode::Undefined;
				std::vector<ShaderBinding::Layout> shaderBindings_;
				bool enableDepthTest_ = true;
				DB::CompareOperation dbCompareOperation_ = DB::CompareOperation::Undefined;
			};
			using CI = CreateInfo;
		};

		virtual Pipeline::Id CreatePipeline(const Pipeline::CI& pipelineCI) = 0;

		using UB = UniformBuffer;

		struct CreateInfo {
			std::map<std::string, Pipeline::CI> namePipelineDescriptions_;
			RenderSurface surface_;
		};

		Driver(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } { }

		virtual void StartRender() = 0;
		virtual void Render() = 0;
		virtual void EndRender() = 0;

		//[[nodiscard]]
		//virtual Common::Id DrawMesh(
		//	const std::string& pipelineName,
		//	const void* vertices,
		//	Common::Size verticesNumber,
		//	VertexType vertexType,
		//	const void* indices,
		//	Common::Size indicesNumber,
		//	IndexType indexType,
		//	const std::vector<RAL::Driver::ShaderBinding::Data>& bindingData) = 0;

		
		virtual void SetViewport(
			Common::UInt32 x,
			Common::UInt32 y,
			Common::UInt32 width,
			Common::UInt32 height) = 0;

		virtual void SetScissor(
			Common::UInt32 x,
			Common::UInt32 y,
			Common::UInt32 width,
			Common::UInt32 height) = 0;

		virtual void BeginRenderPass(
			RP::Id renderPassId,
			RP::AttachmentSet::Id attachmentsId,
			std::pair<Common::UInt32, Common::UInt32> offset,
			std::pair<Common::UInt32, Common::UInt32> area) = 0;
		virtual void BeginSubpass() = 0;
		virtual void BindPipeline(RAL::Driver::Pipeline::Id pipelineId) = 0;
		virtual void BindVertexBuffer(
			VertexBuffer::Id VBId,
			Common::UInt64 offset) = 0;
		virtual void BindIndexBuffer(
			IndexBuffer::Id IBId,
			Common::UInt64 offset) = 0;
		virtual void Bind(RAL::Driver::Pipeline::Id pipelineId, const std::vector<Resource::Id>& resourceIds) = 0;
		virtual void DrawIndexed(Common::Size indicesNumber)= 0;
		virtual void EndSubpass() = 0;
		virtual void EndRenderPass() = 0;
		virtual void Show(RAL::Texture::Id textureId) = 0;
		//[[nodiscard]]
		//virtual Common::Id DrawMesh(
		//	const std::string& pipelineName,
		//	VertexBuffer::Id VBId,
		//	IndexBuffer::Id IBId,
		//	const std::vector<RAL::Driver::ShaderBinding::Data>& bindingData) = 0;

		//virtual void ResumeMeshDrawing(Common::Id shapeId) = 0;

		//virtual void StopMeshDrawing(Common::Id shapeId) = 0;

		//virtual void RemoveMeshFromDrawing(Common::Id shapeId) = 0;

		virtual void FrameBufferResize(glm::u32vec2 newSize) = 0;

		/* Textures */
		[[nodiscard]]
		virtual Texture::Id CreateDiffuseMap(const RAL::Texture::CreateInfo& createInfo) = 0;
		[[nodiscard]]
		virtual bool IsTextureExist(RAL::Texture::Id textureId) const noexcept = 0;

		[[nodiscard]]
		virtual UniformBuffer::Id CreateUniformBuffer(const UniformBuffer::CreateInfo& createInfo) = 0;

		virtual Common::Size GetUBSizeInBytes(UB::Id ubid) = 0;

		[[nodiscard]]
		virtual VertexBuffer::Id CreateVertexBuffer(const VertexBuffer::CreateInfo1& createInfo) = 0;
		virtual void FillVertexBuffer(VertexBuffer::Id id, Common::Size offset, const void* vertices, Common::Size verticesNumber) = 0;
		virtual void ResizeVertexBuffer(VertexBuffer::Id id, Common::Size newSize) = 0;
		[[nodiscard]]
		virtual void DestroyVertexBuffer(VertexBuffer::Id VBId) = 0;
		virtual Common::Size GetVBSizeInBytes(VertexBuffer::Id VBId) = 0;


		[[nodiscard]]
		virtual IndexBuffer::Id CreateIndexBuffer(const IndexBuffer::CreateInfo1& createInfo) = 0;
		virtual void FillIndexBuffer(IndexBuffer::Id id, Common::Size offset, const void* vertices, Common::Size verticesNumber) = 0;
		virtual void ResizeIndexBuffer(IndexBuffer::Id id, Common::Size newSize) = 0;
		[[nodiscard]]
		virtual void DestroyIndexBuffer(IndexBuffer::Id VBId) = 0;
		virtual Common::Size GetIBSizeInBytes(IndexBuffer::Id IBId) = 0;

		[[nodiscard]]
		virtual void FillUniformBuffer(UniformBuffer::Id UBId, void* data) = 0;

		virtual ~Driver() = default;

	protected:
		CreateInfo createInfo_;
	};




}