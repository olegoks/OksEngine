#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <RAL.Driver.hpp>

namespace OksEngine {





	struct DriverTexture : public ECSComponent<DriverTexture> {
	public:

		DriverTexture() : ECSComponent{ nullptr } { }
		DriverTexture(RAL::Texture::Id driverTextureId) :
			ECSComponent{ nullptr },
			driverTextureId_{ driverTextureId } {}

		RAL::Texture::Id driverTextureId_;
	};

	template<>
	inline void Edit<DriverTexture>(DriverTexture* driverTexture) {
		ImGui::TextDisabled("Driver texture id: %d", driverTexture->driverTextureId_);

	}


}