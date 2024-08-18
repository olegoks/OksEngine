#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <RAL.Texture.hpp>

namespace OksEngine {


	struct ImGuiState : public ECSComponent<ImGuiState> {
	public:
		ImGuiState() : ECSComponent{ nullptr } {}

		RAL::Texture::Id fontsTextureId_ = RAL::Texture::Id::Invalid();
	};

	template<>
	inline void Edit<ImGuiState>(ImGuiState* context) {

	}


}

