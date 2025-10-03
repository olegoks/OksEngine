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
			VF3_NF3_TF2_BIDUB4_WUB4,//Vertex float 3, normal float 3, bone ids unsigned byte 4, weight unsigned byte 4
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

		//Uniform buffer
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
		using UB = UniformBuffer;

		[[nodiscard]]
		virtual UniformBuffer::Id CreateUniformBuffer(const UniformBuffer::CreateInfo& createInfo) = 0;
		virtual Common::Size GetUBSizeInBytes(UB::Id ubid) = 0;
		[[nodiscard]]
		virtual void FillUniformBuffer(UniformBuffer::Id UBId, void* data) = 0;
		//Uniform buffer


		// Storage buffer
		class StorageBuffer {
		public:
			using Id = Common::Id;
			struct CreateInfo {
				Common::Size size_ = 0;
			};
		};
		using SB = StorageBuffer;

		[[nodiscard]]
		virtual StorageBuffer::Id CreateStorageBuffer(const StorageBuffer::CreateInfo& createInfo) = 0;
		virtual Common::Size GetSBSizeInBytes(SB::Id ubid) = 0;
		[[nodiscard]]
		virtual void StorageBufferWrite(StorageBuffer::Id SBId,
			Common::Size offsetInBytes,
			void* data,
			Common::Size bytesNumber) = 0;
		virtual void StorageBufferRead(StorageBuffer::Id UBId, Common::Size offset, Common::Size size, void* data) = 0;
		// Storage buffer

		//Vertex buffer
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

		[[nodiscard]]
		virtual VertexBuffer::Id CreateVertexBuffer(const VertexBuffer::CreateInfo1& createInfo) = 0;
		virtual void FillVertexBuffer(VertexBuffer::Id id, Common::Size offset, const void* vertices, Common::Size verticesNumber) = 0;
		virtual void ResizeVertexBuffer(VertexBuffer::Id id, Common::Size newSize) = 0;
		[[nodiscard]]
		virtual void DestroyVertexBuffer(VertexBuffer::Id VBId) = 0;
		virtual Common::Size GetVBSizeInBytes(VertexBuffer::Id VBId) = 0;
		//Vertex buffer

		//Index buffer
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
		[[nodiscard]]
		virtual IndexBuffer::Id CreateIndexBuffer(const IndexBuffer::CreateInfo1& createInfo) = 0;
		virtual void FillIndexBuffer(IndexBuffer::Id id, Common::Size offset, const void* vertices, Common::Size verticesNumber) = 0;
		virtual void ResizeIndexBuffer(IndexBuffer::Id id, Common::Size newSize) = 0;
		[[nodiscard]]
		virtual void DestroyIndexBuffer(IndexBuffer::Id VBId) = 0;
		virtual Common::Size GetIBSizeInBytes(IndexBuffer::Id IBId) = 0;
		//Index buffer

		//Forward declaration section.
		// 
		//Texture
		using Texture_Id = Common::Id;

		enum class Texture_Format : Common::UInt64 {
			R_8,
			D_32_SFLOAT,	// Depth single float 32 bits value 			

			RGBA_32_UINT,	// Unsigned int { 0 - 255, 0 - 255, 0 - 255, 0 - 255 }
			//									R_8		G_8			B_8		A_8

			RGBA_32_UNORM,	// Unsigned normalized { 0.0 - 1.0,  0.0 - 1.0,  0.0 - 1.0,  0.0 - 1.0, }
			//											R_8			G_8			B_8			A_8

			BGRA_32_UINT,	// Unsigned int { 0 - 255, 0 - 255, 0 - 255, 0 - 255 }
			//									B_8		G_8			R_8		A_8

			BGRA_32_UNORM,	// Unsigned normalized { 0.0 - 1.0,  0.0 - 1.0,  0.0 - 1.0,  0.0 - 1.0, }
			//											B_8			G_8			R_8			A_8
			Undefined
		};

		enum class Texture_Usage : Common::UInt64 {
			TransferSource,
			TransferDestination,
			ColorAttachment,
			Sampled,
			DepthStencilAttachment,
			Undefined
		};

		enum class Texture_State {
			DataIsUndefined,
			DataForAllOperations, //Not optimized
			DataForColorWrite,
			DataForShaderRead,
			DataForCopyingSource,
			DataForCopyingDestination,
			DataForPresentation,
			DataForDepthStencilWrite,
			Undefined
		};

		enum class Texture_Access {
			ColorRead,
			ColorWrite,
			DepthStencilRead,
			DepthStencilWrite,
			UniformRead,
			InputAttachmentRead,
			ShaderRead,
			ShaderWrite,

			Undefined
		};
		//Texture
		
		//Pipeline
		
		enum class Pipeline_Stage {
			Top,
			DrawIndirect,
			VertexInput,
			VertexShader,
			TessellationControlShader,
			TessellationEvaluationShader,
			GeometryShader,
			FragmentShader,
			EarlyFragmentTests,
			LateFragmentTests,
			ColorAttachmentOutput,
			ComputeShader,
			Transfer,
			Bottom,
			Undefined
		};

		enum class Pipeline_Topology {
			LineList,
			TriangleList,
			Undefined
		};


		//Pipeline
		//Forward declaration section.

		//Shader
		class Shader {
		public:

			enum class Stage {
				VertexShader,
				FragmentShader,
				ComputeShader,
				Undefined
			};

			struct Binding {
				enum class Type {
					Uniform,
					Storage,
					Sampler,
					InputAttachment,
					Undefined
				};
				struct Data {
					Type type_ = Type::Undefined;
					Common::UInt64 binding_ = Common::Limits<Common::UInt64>::Max();
					Stage stage_ = Stage::Undefined;
					RAL::Driver::UniformBuffer::Id uniformBufferId_ = RAL::Driver::UniformBuffer::Id::Invalid();
					RAL::Driver::Texture_Id textureId_ = RAL::Driver::Texture_Id::Invalid();
				};
				struct Layout {
					std::string name_ = "";
					Common::UInt32 binding_ = 0;
					Type type_ = Type::Undefined;
					Stage stage_ = Stage::Undefined;
				};
			};

			Shader() noexcept = default;
			enum class Type {
				Vertex,
				Fragment,
				Compute,
				Undefined
			};
			struct CreateInfo {
				std::string name_;
				Type type_ = Type::Undefined;
				std::string code_;
			};

			Shader(const CreateInfo& createInfo) : createInfo_{ createInfo } {}

			Shader(const Shader& copyShader) {
				createInfo_ = copyShader.createInfo_;
			}

			[[nodiscard]]
			const std::string& GetCode() const noexcept {
				return createInfo_.code_;
			}

			[[nodiscard]]
			const std::string& GetName() const noexcept {
				return createInfo_.name_;
			}

			[[nodiscard]]
			Type GetType() const noexcept { return createInfo_.type_; }

			virtual ~Shader() noexcept = default;
		private:
			CreateInfo createInfo_;
		};

		virtual std::shared_ptr<Shader> CreateShader(const Shader::CreateInfo& createInfo) const = 0;
		//Shader


		//Texture
		class Texture {
		public:

			using Id = Texture_Id;
			using Format = Texture_Format;
			using Usage = Texture_Usage;
			using State = Texture_State;
			using Access = Texture_Access;

			static Common::Size GetElementSize(Format format) {
				switch (format) {
				case Format::RGBA_32_UNORM:    return 4;  // 4 компоненты × 8 бит = 32 бита (4 байта)
				case Format::D_32_SFLOAT:      return 4;  // 32 бита (4 байта)
				default:
					OS::NotImplemented();
					return 0;
				}
			}

			struct CreateInfo1 {
				std::string name_;
				Format format_ = Format::Undefined;
				std::vector<Common::Byte> data_;
				glm::u32vec2 size_{ 0, 0 };
				State targetState_ = State::Undefined;
				Access targetAccess_ = Access::Undefined;
				std::vector<Pipeline_Stage> targetStages_{};
				std::vector<Usage> usages_;
				Common::UInt32 mipLevels_ = 1;
			};
			using CI1 = CreateInfo1;

		};
		[[nodiscard]]
		virtual Texture::Id CreateTexture(const RAL::Driver::Texture::CreateInfo1& createInfo) = 0;

		[[nodiscard]]
		virtual bool IsTextureExist(RAL::Driver::Texture::Id textureId) const noexcept = 0;

		//Texture

		//Resource
		struct ResourceSet {
			using Id = Common::Id;

			struct Binding {
				Shader::Stage stage_ = Shader::Stage::Undefined;
				Common::UInt32 binding_ = Common::Limits<Common::UInt32>::Max();
				//Uniform buffer.
				UB::Id ubid_ = UB::Id::Invalid();
				SB::Id sbid_ = SB::Id::Invalid();

				Common::UInt64 offset_ = 0;
				Common::UInt64 size_ = Common::Limits<Common::UInt64>::Max();

				//Combined sampler.
				Texture_Id textureId_ = Texture_Id::Invalid();

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

		//Resource

		//Render pass
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

				using Id = Common::Id;
				Texture_Format format_;
				Texture_State initialState_;	//State of attachment before render pass. Excepted by render pass.
				LoadOperation loadOperation_;	//What to do with attachment in the begin of render pass.		
				StoreOperation storeOperation_;	//What to do with attachment in the end of render pass.
				Texture_State finalState_;		//State of attachment after render pass.  Will be set by render pass.

			};

			struct AttachmentSet {
				using Id = Common::Id;
				struct CreateInfo {
					RenderPass::Id rpId_ = Common::Limits<RenderPass::Id>::Max();
					std::vector<Texture_Id> textures_;
					glm::u32vec2 size_;
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

		virtual RP::Id CreateRenderPass(const RP::CI& rpCI) = 0;
		virtual RP::AttachmentSet::Id CreateAttachmentSet(const RP::AttachmentSet::CI& attachmentSetCI) = 0;
		//Render pass

		//Pipeline


		struct PushConstant {
			Shader::Stage shaderStage_ = Shader::Stage::Undefined;
			Common::UInt32 offset_ = 0;
			Common::UInt32 size_ = 0;
		};

		struct Pipeline {
			using Id = Common::Id;
			using Stage = Pipeline_Stage;
			using Topology = Pipeline_Topology;
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

			struct CreateInfo {
				std::string name_ = "";
				RP::Id renderPassId_ = RP::Id::Invalid();
				std::shared_ptr<Shader> vertexShader_ = nullptr;
				std::shared_ptr<Shader> fragmentShader_ = nullptr;
				Pipeline_Topology topologyType_ = Pipeline_Topology::Undefined;
				VertexType vertexType_ = VertexType::Undefined;
				IndexType indexType_ = IndexType::Undefined;
				FrontFace frontFace_ = FrontFace::Undefined;
				CullMode cullMode_ = CullMode::Undefined;
				std::vector<PushConstant> pushConstants_;
				std::vector<Shader::Binding::Layout> shaderBindings_;
				bool enableDepthTest_ = true;
				DB::CompareOperation dbCompareOperation_ = DB::CompareOperation::Undefined;
			};
			using CI = CreateInfo;
		};

		struct ComputePipeline {
			using Id = Common::Id;

			struct CreateInfo {
				std::string name_ = "";
				std::shared_ptr<Shader> computeShader_ = nullptr;
				std::vector<PushConstant> pushConstants_;
				std::vector<Shader::Binding::Layout> shaderBindings_;
			};
			using CI = CreateInfo;
		};

		virtual Pipeline::Id CreatePipeline(const Pipeline::CI& pipelineCI) = 0;
		virtual ComputePipeline::Id CreateComputePipeline(const ComputePipeline::CI& pipelineCI) = 0;
		//Pipeline


		struct CreateInfo {
			std::map<std::string, Pipeline::CI> namePipelineDescriptions_;
			RenderSurface surface_;
		};

		Driver(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } { }

		virtual void StartRender() = 0;
		virtual void EndRender() = 0;

		virtual void SetViewport(
			Common::UInt32 x,
			Common::UInt32 y,
			Common::UInt32 width,
			Common::UInt32 height) = 0;

		virtual void SetScissor(
			Common::Int32 x,
			Common::Int32 y,
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
		virtual void Bind(
			RAL::Driver::Pipeline::Id pipelineId,
			Common::UInt32 firstResourceIndex, // layout(set = firstResourceIndex + (number of resources), binding = 0) uniform Camera {
			const std::vector<Resource::Id>& resourceIds) = 0;

		virtual void PushConstants(
			RAL::Driver::Pipeline::Id pipelineId, RAL::Driver::Shader::Stage stage,
			Common::Size sizeInBytes, void* data) = 0;

		virtual void DrawIndexed(Common::Size indicesNumber) = 0;
		virtual void Draw(Common::Size verticesNumber) = 0;
		virtual void EndSubpass() = 0;
		virtual void EndRenderPass() = 0;
		virtual void Show(RAL::Driver::Texture::Id textureId) = 0;

		virtual void FrameBufferResize(glm::u32vec2 newSize) = 0;

		//Compute pipeline

		virtual void StartCompute() = 0;

		virtual void BindComputePipeline(RAL::Driver::ComputePipeline::Id pipelineId) = 0;
		virtual void ComputePushConstants(
			RAL::Driver::Pipeline::Id pipelineId,
			Common::Size sizeInBytes, void* data) = 0;
		virtual void ComputeBind(
			RAL::Driver::Pipeline::Id pipelineId,
			Common::UInt32 firstResourceIndex, // layout(set = firstResourceIndex + (number of resources), binding = 0) uniform Camera {
			const std::vector<Resource::Id>& resourceIds) = 0;

		virtual void Dispatch(Common::Size groupCountX, Common::Size groupCountY, Common::Size groupCountZ) = 0;

		virtual void EndCompute() = 0;

		//Compute pipeline


		virtual ~Driver() = default;

	protected:
		CreateInfo createInfo_;
	};




}