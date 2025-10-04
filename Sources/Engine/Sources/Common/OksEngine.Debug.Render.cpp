#pragma once
#include <Common\auto_OksEngine.Debug.Render.hpp>

#include <ft2build.h>  // Основной заголовочный файл FreeType
#include FT_FREETYPE_H // Заголовок с основными структурами и функциями

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace OksEngine
{

	void CreateDebugTextVertexBuffer::Update(
		ECS2::Entity::Id entity0id,
		const DebugTextRenderer* debugTextRenderer0,

		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {
		/*CreateComponent<DebugTextVertexBuffer>(entity0id, );*/
	};


	void UpdateDebugTextVertexBuffer::Update(
		ECS2::Entity::Id entity0id,
		const DebugTextRenderer* debugTextRenderer0,
		const SymbolsMetadata* symbolsMetadata0,
		DebugTextVertexBuffer* debugTextVertexBuffer0,

		ECS2::Entity::Id entity1id, 
		const DebugText2D* debugText2D1) {

		auto& vertices = debugTextVertexBuffer0->vertices_;

		float x = debugText2D1->x_;
		float y = debugText2D1->y_;
		const float scale = 1.0;
		// Для каждого символа в тексте
		for (char c : debugText2D1->text_) {
			const SymbolMetadata& ch = (symbolsMetadata0->symbolsMetadata_.find(c))->second;

			// Вычисляем позицию и размеры символа
			float cx = x + ch.bearing_.x;
			float cy = y - (/*ch.size_.y - */ch.bearing_.y) /*- ch.advanceY_*/;
			float w = ch.size_.x * scale;
			float h = ch.size_.y * scale;
			//glm::u32vec2{ 2560, 1440 }
			//TODO: use current frame buffer size.
			const float screenHalfX = 1280;
			const float screenHalfY = 720;
			vertices.Add({ { (cx - screenHalfX) / screenHalfX, ((cy + h) - screenHalfY) / screenHalfY}, { ch.uvMin_.x, ch.uvMax_.y }, Geom::Color4b::Zero()});
			vertices.Add({ { (cx - screenHalfX) / screenHalfX, (cy - screenHalfY) / screenHalfY}, { ch.uvMin_.x, ch.uvMin_.y }, Geom::Color4b::Zero() });
			vertices.Add({ { ((cx + w) - screenHalfX) / screenHalfX, (cy - screenHalfY) / screenHalfY}, { ch.uvMax_.x, ch.uvMin_.y }, Geom::Color4b::Zero() });

			vertices.Add({ { ((cx + w) - screenHalfX) / screenHalfX, (cy - screenHalfY) / screenHalfY}, { ch.uvMax_.x, ch.uvMin_.y }, Geom::Color4b::Zero() });
			vertices.Add({ { ((cx + w) - screenHalfX) / screenHalfX, ((cy + h) - screenHalfY) / screenHalfY }, { ch.uvMax_.x, ch.uvMax_.y }, Geom::Color4b::Zero() });
			vertices.Add({ { (cx - screenHalfX) / screenHalfX, ((cy + h) - screenHalfY) / screenHalfY }, { ch.uvMin_.x, ch.uvMax_.y }, Geom::Color4b::Zero() });

			// Сдвигаем позицию для следующего символа
			x += (ch.advanceX_) * scale;
		}

	};

	void CreateTextDriverVertexBuffer::Update(
		ECS2::Entity::Id entity0id, 
		const DebugTextRenderer* debugTextRenderer0,
		
		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {


		auto driver = renderDriver1->driver_;

		const Common::UInt64 capacity = Common::Limits<Common::UInt16>::Max();

		RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
			.verticesNumber_ = capacity,
			.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
			.type_ = RAL::Driver::VertexBuffer::Type::Const
		};
		RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
		CreateComponent<DebugTextDriverVertexBuffer>(entity0id, VBId, capacity, 0);
	};


	void UpdateDebugTextDriverVertexBuffer::Update(
		ECS2::Entity::Id entity0id, const DebugTextRenderer* debugTextRenderer0,
		DebugTextVertexBuffer* debugTextVertexBuffer0,
		DebugTextDriverVertexBuffer* debugTextDriverVertexBuffer0, ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1, ECS2::Entity::Id entity2id, const DebugText2D* debugText2D2) {

		if (debugTextVertexBuffer0->vertices_.GetVerticesNumber() == 0) {
			return;
		}

		auto driver = renderDriver1->driver_;

		//Resize buffers if need.
		if (debugTextVertexBuffer0->vertices_.GetSizeInBytes() > driver->GetVBSizeInBytes(debugTextDriverVertexBuffer0->id_)) {
			driver->ResizeVertexBuffer(
				debugTextDriverVertexBuffer0->id_,
				debugTextVertexBuffer0->vertices_.GetSizeInBytes());
			debugTextDriverVertexBuffer0->capacity_ = debugTextVertexBuffer0->vertices_.GetSizeInBytes();
		}

		//Send vertices and indices data to GPU only if they changed.
		bool isDataChanged = false;

		//Fill data.
		driver->FillVertexBuffer(
			debugTextDriverVertexBuffer0->id_,
			0,
			debugTextVertexBuffer0->vertices_.GetData(),
			debugTextVertexBuffer0->vertices_.GetVerticesNumber());
		debugTextDriverVertexBuffer0->size_ = debugTextVertexBuffer0->vertices_.GetSizeInBytes();

		//TODO: move to separate system.
		debugTextVertexBuffer0->vertices_.Clear();

	};

	void CreateDebugTextRenderer::Update() {

		const ECS2::Entity::Id debugTextRendererEntityId = CreateEntity();
		CreateComponent<DebugTextRenderer>(debugTextRendererEntityId);
		CreateComponent<DebugTextVertexBuffer>(debugTextRendererEntityId);
	};

	void CreateDebugTextAtlas::Update(
		ECS2::Entity::Id entity0id, ResourceSystem* resourceSystem0,
		ECS2::Entity::Id entity1id, const DebugTextRenderer* debugTextRenderer1) {

		Resources::ResourceData fontResourceData = resourceSystem0->system_->GetResourceSynch(Subsystem::Type::Engine, "Root/timesnewromanpsmt.ttf");

		FT_Library ft;
		const FT_Error error = FT_Init_FreeType(&ft);

#pragma region Assert
		ASSERT_FMSG(!error, "Error while initting FT_Library.");
#pragma endregion

		FT_Face face;
		if (FT_New_Memory_Face(ft, fontResourceData.GetData<Common::UByte>(), fontResourceData.GetSize(), 0, &face)) {
			throw std::runtime_error("Failed to load font");
		}

		// Установка размера шрифта
		FT_Set_Pixel_Sizes(face, 0, 18);

		constexpr int atlasWidth = 1024; // Ширина атласа
		constexpr int atlasHeight = 1024; // Высота атласа
		std::vector<Geom::Color4b> atlasData(atlasWidth * atlasHeight, Geom::Color4b{ 0, 0, 0, 0 }); // Чёрный фон

		int x = 0, y = 0;
		int maxRowHeight = 0;

		//symbols metadata
		std::map<char, SymbolMetadata> symbolsMetadata;



		for (char c = 32; c < 127; c++) { // ASCII-символы
			const FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);

#pragma region Assert
			ASSERT_FMSG(!error, "");
#pragma endregion
			//Symbol bitmap.
			FT_Bitmap& bitmap = face->glyph->bitmap;

			// Проверяем, помещается ли символ в текущую строку
			if (x + bitmap.width > atlasWidth) {
				x = 0;
				y += maxRowHeight;
				maxRowHeight = 0;
			}

			//TODO: make texture with only one byte for pixel
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
			if (c == '.') {
				Common::BreakPointLine();
			}

			SymbolMetadata metadata;
			{
				metadata.uvMin_ = glm::vec2(x, y) / glm::vec2(atlasWidth, atlasHeight);
				metadata.uvMax_ = glm::vec2(x + bitmap.width, y + bitmap.rows) / glm::vec2(atlasWidth, atlasHeight);
				//metadata.size_ = glm::ivec2(face->glyph->metrics.width >> 6, face->glyph->metrics.height >> 6);
				metadata.size_ = glm::ivec2(bitmap.width, bitmap.rows);
				//metadata.bearing_ = glm::ivec2(face->glyph->metrics.horiBearingX >> 6, face->glyph->metrics.horiBearingY >> 6);
				metadata.bearing_ = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
				metadata.advanceX_ = static_cast<Common::UInt32>(face->glyph->advance.x >> 6);//->metrics.horiAdvance >> 6);
				metadata.advanceY_ =  static_cast<Common::UInt32>(face->glyph->metrics.vertAdvance >> 6);
			}
			
			symbolsMetadata[c] = metadata;


			//// Сохраняем метаданные символа (позицию в атласе, смещение и т. д.)
			//characters[c] = {
			//	.textureCoords = {x, y, bitmap.width, bitmap.rows},
			//	.bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top},
			//	.advance = face->glyph->advance.x >> 6
			//};

			x += bitmap.width;
			if (bitmap.rows > maxRowHeight) {
				maxRowHeight = bitmap.rows;
			}
#undef max
			//maxRowHeight = std::max(maxRowHeight, bitmap.rows);
		}

		//debug
		int channels = 4; // RGBA
		int quality = 90; // Качество JPG (1-100)
		int result = stbi_write_jpg("FontsAtlas.png", atlasWidth, atlasHeight, channels, atlasData.data(), quality);

		CreateComponent<SymbolsMetadata>(entity1id, std::move(symbolsMetadata));
		CreateComponent<TextureInfo>(entity1id, "FontAtlasTexture");
		CreateComponent<Texture>(entity1id, atlasWidth, atlasHeight, atlasData);

	}


	void CreateDebugTextAttachmentSet::Update(
		ECS2::Entity::Id entity0id, 
		const RenderDriver* renderDriver0,
		const RenderAttachment* renderAttachment0,
		
		ECS2::Entity::Id entity1id,
		const DebugTextRenderer* debugTextRenderer1,
		const DebugTextRenderPass* debugTextRenderPass1) {

		RAL::Driver::RP::AttachmentSet::CI attachmentSetCI{
			.rpId_ = debugTextRenderPass1->rpId_,
			.textures_ = { renderAttachment0->textureId_ }, // Render target attachment texture id 
			.size_ = glm::u32vec2{ 2560, 1440 }
		};

		RAL::Driver::RP::AttachmentSet::Id rpAttachmentsSetId = renderDriver0->driver_->CreateAttachmentSet(attachmentSetCI);

		CreateComponent<DebugTextAttachmentSet>(entity1id, rpAttachmentsSetId);

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
				.loadOperation_ = RAL::Driver::RP::AttachmentUsage::LoadOperation::Load,
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


		CreateComponent<DebugTextRenderPass>(entity1id,
			renderPassId);

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

		RAL::Driver::Shader::Binding::Layout samplerBinding{
			.binding_ = 0,
			.type_ = RAL::Driver::Shader::Binding::Type::Sampler,
			.stage_ = RAL::Driver::Shader::Stage::FragmentShader
		};

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
		
		ECS2::Entity::Id entity1id,
		const DebugTextRenderer* debugTextRenderer1, 
		const DebugTextRenderPass* debugTextRenderPass1,
		const DebugTextAttachmentSet* debugTextAttachmentSet1,
		const DebugTextPipeline* debugTextPipeline1) {

		auto driver = renderDriver0->driver_;

		driver->BeginRenderPass(
			debugTextRenderPass1->rpId_, 
			debugTextAttachmentSet1->attachmentSetId_,
			{},
			{ 0, 0 }, { 2560, 1440 });
		driver->BeginSubpass();

	}

	void DrawDebugText2D::Update(
		ECS2::Entity::Id entity0id, RenderDriver* renderDriver0, const RenderPass* renderPass0,
		const Pipeline* pipeline0, ECS2::Entity::Id entity1id, const DebugTextRenderer* debugTextRenderer1,
		const DebugTextRenderPass* debugTextRenderPass1, const DebugTextPipeline* debugTextPipeline1,
		const SymbolsMetadata* symbolsMetadata1,
		const DebugTextDriverVertexBuffer* debugTextDriverVertexBuffer1,
		const TextureResource* textureResource1, ECS2::Entity::Id entity2id, const DebugText2D* debugText2D2) {

		auto driver = renderDriver0->driver_;


		driver->SetViewport(0, 0, 2560, 1440);
		driver->SetScissor(0, 0, 2560, 1440);

		// Активируем пайплайн для рендеринга текста
		driver->BindPipeline(debugTextPipeline1->id_);
		driver->Bind(debugTextPipeline1->id_, 0, { textureResource1->id_ });
		driver->BindVertexBuffer(debugTextDriverVertexBuffer1->id_, 0);
		//static Geom::VertexCloud<RAL::Vertex2ftc> vertices2ftc;


		//// Для каждого символа в тексте
		//for (auto c : name2->value_) {
		//	const SymbolMetadata& ch = symbolsMetadata1->symbolsMetadata_[c];

		//	// Вычисляем позицию и размеры символа
		//	float xpos = x + ch.bearing_.x * 1.0;
		//	float ypos = y - (ch.size_.y - ch.bearing_.y) * 1.0;

		//	float w = ch.size_.x * 1.0;
		//	float h = ch.size_.y * 1.0;

		//	// Вершины квадрата для символа
		//	std::array<Vertex, 4> vertices = {
		//		Vertex{{xpos,     ypos + h}, {ch.texCoordMin.x, ch.texCoordMax.y}},
		//		Vertex{{xpos,     ypos},     {ch.texCoordMin.x, ch.texCoordMin.y}},
		//		Vertex{{xpos + w, ypos},     {ch.texCoordMax.x, ch.texCoordMin.y}},
		//		Vertex{{xpos + w, ypos + h}, {ch.texCoordMax.x, ch.texCoordMax.y}}
		//	};



		//	// Обновляем вершинный буфер
		//	updateVertexBuffer(vertices.data(), vertices.size() * sizeof(Vertex));

		//	// Рисуем квадрат (2 треугольника)
		driver->Draw(debugTextDriverVertexBuffer1->size_ / sizeof(RAL::Vertex2ftc));

		//	// Сдвигаем позицию для следующего символа
		//	x += (ch.advance >> 6) * scale;
		//}
		//driver->BindVertexBuffer()
		//driver->Draw(vertices2ftc.GetVerticesNumber());

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