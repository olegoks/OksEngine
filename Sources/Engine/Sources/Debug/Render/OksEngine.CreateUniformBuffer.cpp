
#include <Debug/Render/auto_OksEngine.CreateUniformBuffer.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	void CreateUniformBuffer::Update(
		ECS2::Entity::Id entity1Id,
		const ImGuiState* imGuiState,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		//if (imGuiState->fontsTextureId_.IsInvalid()) {
		//	ImGuiIO& io = ImGui::GetIO();
		//	unsigned char* pixels;
		//	int width, height;
		//	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		//	std::vector<RAL::Color4b> pixelsRGBA;
		//	pixelsRGBA.resize(width * height);
		//	std::memcpy(pixelsRGBA.data(), pixels, width * height * sizeof(RAL::Color4b));

		//	auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		//	RAL::Texture::CreateInfo textureCreateInfo{
		//		.name_ = "ImGui texture",
		//		.pixels_ = std::move(pixelsRGBA),
		//		.size_ = { width, height }
		//	};

		//	imGuiState->fontsTextureId_ = driver->CreateDiffuseMap(textureCreateInfo);
		//}
	}

}