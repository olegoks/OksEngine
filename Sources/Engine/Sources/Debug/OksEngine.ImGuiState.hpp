#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <RAL.Texture.hpp>

namespace OksEngine {


	struct ImGuiState : public ECSComponent<ImGuiState> {
	public:
		ImGuiState() : ECSComponent{ nullptr } {}

		RAL::Texture::Id fontsTextureId_ = RAL::Texture::Id::Invalid();
		Common::Size fps_ = 30;
		std::vector<Common::Id> driverShapesId_;
	};

	template<>
	inline void Edit<ImGuiState>(ImGuiState* context) {
		ImGui::InputScalar("Fps: ", ImGuiDataType_U64, &context->fps_);
	}


}

