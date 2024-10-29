
#include <Debug/Render/OksEngine.CreateImGuiTexture.hpp>

#include <Debug/OksEngine.ImGuiState.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	void CreateImGuiTexture::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* state = world->GetComponent<ImGuiState>(entityId);
		if (state->fontsTextureId_.IsInvalid()) {
			ImGuiIO& io = ImGui::GetIO();
			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			std::vector<RAL::Color4b> pixelsRGBA;
			pixelsRGBA.resize(width * height);
			std::memcpy(pixelsRGBA.data(), pixels, width * height * sizeof(RAL::Color4b));

			auto driver = GetContext().GetRenderSubsystem()->GetDriver();
			RAL::Texture::CreateInfo textureCreateInfo{
				.name_ = "ImGui texture",
				.pixels_ = std::move(pixelsRGBA),
				.size_ = { width, height }
			};

			state->fontsTextureId_ = driver->CreateDiffuseMap(textureCreateInfo);
		}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateImGuiTexture::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<ImGuiState>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId CreateImGuiTexture::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateImGuiTexture>().GetId();
	}

}