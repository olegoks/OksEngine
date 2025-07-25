#pragma once
#include <Common\auto_OksEngine.Debug.Render.hpp>

#include <ft2build.h>  // Основной заголовочный файл FreeType
#include FT_FREETYPE_H // Заголовок с основными структурами и функциями

namespace OksEngine
{
	void CreateDebugTextRenderer::Update() {
		
		const ECS2::Entity::Id debugTextRendererEntityId = CreateEntity();
		CreateComponent<DebugTextRenderer>(debugTextRendererEntityId);

	};

	void CreateDebugTextAtlas::Update(
		ECS2::Entity::Id entity0id, ResourceSystem* resourceSystem0,
		ECS2::Entity::Id entity1id, const DebugTextRenderer* debugTextRenderer1){

		Resources::ResourceData fontResourceData = resourceSystem0->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/CountrysideSsiItalic.ttf");

		FT_Library ft;
		const FT_Error error = FT_Init_FreeType(&ft);

#pragma region Assert
		OS::AssertMessage(!error, "Error while initting FT_Library.");
#pragma endregion

		FT_Face face;
		if (FT_New_Memory_Face(ft, fontResourceData.GetData<Common::UByte>(), fontResourceData.GetSize(), 0, &face)) {
			throw std::runtime_error("Failed to load font");
		}

		// Установка размера шрифта
		FT_Set_Pixel_Sizes(face, 0, 48);

		constexpr int atlasWidth = 1024; // Ширина атласа
		constexpr int atlasHeight = 1024; // Высота атласа
		std::vector<Geom::Color4b> atlasData(atlasWidth * atlasHeight, Geom::Color4b{ 0, 0, 0, 0 }); // Чёрный фон

		int x = 0, y = 0;
		int maxRowHeight = 0;

		for (char c = 32; c < 127; c++) { // ASCII-символы
			FT_Load_Char(face, c, FT_LOAD_RENDER);
			//Symbol bitmap.
			FT_Bitmap& bitmap = face->glyph->bitmap;



			//FT_Pixel_Mode_::FT_PIXEL_MODE_BGRA

			// Проверяем, помещается ли символ в текущую строку
			if (x + bitmap.width > atlasWidth) {
				x = 0;
				y += maxRowHeight;
				maxRowHeight = 0;
			}

			// Копируем пиксели символа в атлас
			for (int row = 0; row < bitmap.rows; row++) {
				for (int col = 0; col < bitmap.width; col++) {
					atlasData[(y + row) * atlasWidth + (x + col)] = Geom::Color4b{  
					static_cast<Common::Byte>(255),
					static_cast<Common::Byte>(255),
					static_cast<Common::Byte>(255),
					static_cast<Common::Byte>(bitmap.buffer[row * bitmap.pitch + col])
					};
				}
			}

			//// Сохраняем метаданные символа (позицию в атласе, смещение и т. д.)
			//characters[c] = {
			//	.textureCoords = {x, y, bitmap.width, bitmap.rows},
			//	.bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top},
			//	.advance = face->glyph->advance.x >> 6
			//};

			x += bitmap.width;
#undef max
			//maxRowHeight = std::max(maxRowHeight, bitmap.rows);
		}

		CreateComponent<TextureInfo>(entity1id, "FontAtlasTexture");
		CreateComponent<Texture>(entity1id, atlasWidth, atlasHeight, atlasData);

	}

	void CreateDebugTextRenderPass::Update(
		ECS2::Entity::Id entity0id,
		const RenderDriver* renderDriver0,
		const RenderPass* renderPass0,
		const Pipeline* pipeline0, 
		
		ECS2::Entity::Id entity1id,
		const DebugTextRenderer* debugTextRenderer1) {


		auto driver = renderDriver0->driver_;

		std::vector<RAL::Driver::RP::AttachmentUsage> attachmentsUsage;
		{
			RAL::Driver::RP::AttachmentUsage attachment{
				.format_ = RAL::Driver::Texture::Format::RGBA_32_UNORM,
				.initialState_ = RAL::Driver::Texture::State::DataForColorWrite,
				.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Ignore,
				.storeOperation_ = RAL::Driver::RP::AttachmentUsage::StoreOperation::Store,
				.finalState_ = RAL::Driver::Texture::State::DataForColorWrite
			};
			attachmentsUsage.push_back(attachment);
		}

		std::vector<RAL::Driver::RP::Subpass> subpasses;
		{
			RAL::Driver::RP::Subpass subpass{
				.colorAttachments_ = { 0 } // Index of attachment.
			};
			subpasses.push_back(subpass);
		}

		RAL::Driver::RP::CI rpCI{
			.name_ = "DebugTextRenderPass",
			.attachments_ = attachmentsUsage,
			.subpasses_ = subpasses
		};

		const RAL::Driver::RP::Id renderPassId = driver->CreateRenderPass(rpCI);

		//RAL::Driver::Texture::CreateInfo1 textureCreateInfo{
		//	.name_ = "ImGui render buffer",
		//	.format_ = RAL::Driver::Texture::Format::BGRA_32_UNORM,
		//	.size_ = { 2560, 1440 },
		//	.targetState_ = RAL::Driver::Texture::State::DataForColorWrite,
		//	.targetAccess_ = RAL::Driver::Texture::Access::ColorWrite,
		//	.targetStages_ = { RAL::Driver::Pipeline::Stage::ColorAttachmentOutput },
		//	.usages_ = { RAL::Driver::Texture::Usage::ColorAttachment, RAL::Driver::Texture::Usage::TransferSource },
		//	.mipLevels_ = 1
		//	
		//};
		//const RAL::Driver::Texture::Id textureId = driver->CreateTexture(textureCreateInfo);

		RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
			.rpId_ = renderPassId,
			.textures_ = { renderPass0->textureIds_[1] }, // Render target attachment texture id 
			.size_ = glm::u32vec2{ 2560, 1440 }
		};

		RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = driver->CreateAttachmentSet(attachmentSetCI);

		CreateComponent<DebugTextRenderPass>(entity1id,
			renderPassId,
			rpAttachmentsSetId,
			std::vector<Common::Id>{ renderPass0->textureIds_[1] });

	};

	void CreateDebugTextPipeline::Update(
		ECS2::Entity::Id entity0id, 
		const RenderDriver* renderDriver0,

		ECS2::Entity::Id entity1id,
		const ResourceSystem* resourceSystem1,

		ECS2::Entity::Id entity2id,
		const DebugTextRenderer* debugTextRenderer2,
		const DebugTextRenderPass* debugTextRenderPass2) {


		Resources::ResourceData vertexTextShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/text.vert");
		Resources::ResourceData fragmentTextShaderResource = resourceSystem1->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/text.frag");

		auto driver = renderDriver0->driver_;

		std::string imguiVertexShader{ vertexTextShaderResource.GetData<Common::Byte>(), vertexTextShaderResource.GetSize() };
		std::string imguiFragmentShader{ fragmentTextShaderResource.GetData<Common::Byte>(), fragmentTextShaderResource.GetSize() };


		RAL::Driver::Shader::CreateInfo vertexShaderCreateInfo{
			.name_ = "TextVertexShader",
			.type_ = RAL::Driver::Shader::Type::Vertex,
			.code_ = imguiVertexShader
		};
		auto vertexShader = driver->CreateShader(vertexShaderCreateInfo);

		RAL::Driver::Shader::CreateInfo fragmentShaderCreateInfo{
			.name_ = "TextFragmentShader",
			.type_ = RAL::Driver::Shader::Type::Fragment,
			.code_ = imguiFragmentShader
		};
		auto fragmentShader = driver->CreateShader(fragmentShaderCreateInfo);

		std::vector<RAL::Driver::Shader::Binding::Layout> shaderBindings;

		RAL::Driver::Shader::Binding::Layout transformBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Uniform,
			.stage_ = RAL::Driver::Shader::Stage::VertexShader
		};

		RAL::Driver::Shader::Binding::Layout samplerBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
			.stage_ = RAL::Driver::Shader::Stage::FragmentShader
		};

		shaderBindings.push_back(transformBinding);
		shaderBindings.push_back(samplerBinding);

		RAL::Driver::Pipeline::CI pipelineCI{
			.name_ = "Text Pipeline",
			.renderPassId_ = debugTextRenderPass2->rpId_,
			.vertexShader_ = vertexShader,
			.fragmentShader_ = fragmentShader,
			.topologyType_ = RAL::Driver::Pipeline::Topology::TriangleList,
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.indexType_ = RAL::Driver::IndexType::UI32,
			.frontFace_ = RAL::Driver::FrontFace::CounterClockwise,
			.cullMode_ = RAL::Driver::CullMode::None,
			.shaderBindings_ = shaderBindings,
			.enableDepthTest_ = false,
			.dbCompareOperation_ = RAL::Driver::Pipeline::DepthBuffer::CompareOperation::Always

		};

		const RAL::Driver::Pipeline::Id pipelineId = driver->CreatePipeline(pipelineCI);

		CreateComponent<DebugTextPipeline>(entity2id, pipelineId);



	};

	void BeginDebugTextRenderPass::Update(
		ECS2::Entity::Id entity0id, 
		RenderDriver* renderDriver0,
		const RenderPass* renderPass0,
		const Pipeline* pipeline0, 
		
		ECS2::Entity::Id entity1id, 
		const DebugTextRenderer* debugTextRenderer1,
		const DebugTextRenderPass* debugTextRenderPass1, 
		const DebugTextPipeline* debugTextPipeline1) {

		auto driver = renderDriver0->driver_;

		driver->BeginRenderPass(debugTextRenderPass1->rpId_, debugTextRenderPass1->attachmentsSetId_, { 0, 0 }, { 2560, 1440 });
		driver->BeginSubpass();

	}

	void DrawWorldPositionName::Update(ECS2::Entity::Id entity0id, RenderDriver* renderDriver0, const RenderPass* renderPass0,
		const Pipeline* pipeline0, ECS2::Entity::Id entity1id, const DebugTextRenderer* debugTextRenderer1,
		const DebugTextRenderPass* debugTextRenderPass1, const DebugTextPipeline* debugTextPipeline1,
		ECS2::Entity::Id entity2id, const Name* name2, const WorldPosition3D* worldPosition3D2) {
	


	};

	void EndDebugTextRenderPass::Update(
		ECS2::Entity::Id entity0id,
		RenderDriver* renderDriver0,
		const RenderPass* renderPass0,
		const Pipeline* pipeline0,
		
		ECS2::Entity::Id entity1id, 
		const DebugTextRenderer* debugTextRenderer1,
		const DebugTextRenderPass* debugTextRenderPass1, 
		const DebugTextPipeline* debugTextPipeline1) {

		auto driver = renderDriver0->driver_;

		driver->EndSubpass();
		driver->EndRenderPass();

	}





} // namespace OksEngine