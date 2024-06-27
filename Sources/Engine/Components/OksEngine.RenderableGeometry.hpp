#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
		Common::Index modelId_ = 0;
		Math::Matrix4x4f modelMatrix_ = Math::Matrix4x4f::GetIdentity();
		std::string modelObjFileName_ = "";
		std::string modelMtlFileName_ = "";
		std::string modelTextureFileName_ = "";

		ImmutableRenderGeometry(
			Context& context,
			const Math::Matrix4x4f& modelMatrix,
			std::string objFileName,
			std::string mtlFileName,
			std::string textureFileName) :
			ECSComponent{ context },
			modelMatrix_{ modelMatrix },
			modelObjFileName_{ objFileName },
			modelMtlFileName_{ modelMtlFileName_ },
			modelTextureFileName_{ textureFileName } {}
	};


}
