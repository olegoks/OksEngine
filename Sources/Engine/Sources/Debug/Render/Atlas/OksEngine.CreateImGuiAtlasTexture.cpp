#include <Debug/Render/Atlas\auto_OksEngine.CreateImGuiAtlasTexture.hpp>

namespace OksEngine {
	void CreateImGuiAtlasTexture::Update(ECS2::Entity::Id entityId, ImGuiState* imGuiState) {


		//TODO: Can fontsTextureId be invalid ?? add assert.
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		DS::Vector<RAL::Color4b> pixelsRGBA;
		pixelsRGBA.Resize(width * height);
		std::memcpy(pixelsRGBA.GetData(), pixels, width * height * sizeof(RAL::Color4b));


		CreateComponent<ImGuiAtlasTexture>(entityId, width, height, pixelsRGBA);
		//auto driver = renderDriver->driver_;
		//RAL::Texture::CreateInfo textureCreateInfo{
		//	.name_ = "ImGui texture",
		//	.pixels_ = std::move(pixelsRGBA),
		//	.size_ = { width, height }
		//};


		//const RAL::Texture::Id atlasTextureId = driver->CreateDiffuseMap(textureCreateInfo);

		//CreateComponent<ImGuiAtlasDriverTexture>(entity1Id, atlasTextureId);

	};
}