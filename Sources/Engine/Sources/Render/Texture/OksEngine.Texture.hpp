#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Driver.hpp>

namespace OksEngine {

	struct Texture : public ECSComponent<Texture> {

		Texture() :
			ECSComponent{ nullptr } { }
		Texture(Common::Size width,
			Common::Size height,
			const DS::Vector<Geom::Color4b>& pixels) :
			ECSComponent{ nullptr }, width_{ width }, height_{ height }, pixels_{ pixels } { }

		Common::Size width_ = 0;
		Common::Size height_ = 0;
		DS::Vector<Geom::Color4b> pixels_;
	};

	template<>
	inline void Edit<Texture>(Texture* texture) {
		ImGui::TextDisabled("Width: %d", texture->width_);
		ImGui::TextDisabled("Height: %d", texture->height_);

	}


}