#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
		Common::Index modelId_ = Common::Limits<Common::Index>::Max();
		Math::Matrix4x4f modelMatrix_ = Math::Matrix4x4f::GetIdentity();
		std::string modelObjFileName_ = "";
		std::string modelMtlFileName_ = "";
		std::string modelTextureFileName_ = "";

		ImmutableRenderGeometry() : ECSComponent{ nullptr }{

		}

		ImmutableRenderGeometry(Context*, float x, float y, float z, std::string obj, std::string mtl, std::string texture) : ECSComponent{ nullptr } {

		}

		void Rotate(float angle, float x, float y, float z) {
			Math::Matrix4x4f rotateMatrix = Math::Matrix4x4f::GetRotate(angle, { x, y, z });
			modelMatrix_ = modelMatrix_ * rotateMatrix;
		}

		ImmutableRenderGeometry(
			Context* context,
			const Math::Matrix4x4f& modelMatrix,
			std::string objFileName,
			std::string mtlFileName,
			std::string textureFileName) :
			ECSComponent{ context },
			modelMatrix_{ modelMatrix },
			modelObjFileName_{ objFileName },
			modelMtlFileName_{ mtlFileName },
			modelTextureFileName_{ textureFileName } {}
	};


}
