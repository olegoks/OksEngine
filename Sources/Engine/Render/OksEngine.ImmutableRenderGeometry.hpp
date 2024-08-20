#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Texture.hpp>

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
	public:
		ImmutableRenderGeometry() : ECSComponent{ nullptr }{

		}

		ImmutableRenderGeometry(
			std::string objFileName,
			std::string mtlFileName,
			const std::vector<std::string>& textures) :
			ECSComponent{ nullptr },
			modelObjFileName_{ objFileName },
			modelMtlFileName_{ mtlFileName },
			textures_{ textures } {}

		//void Rotate(float angle, float x, float y, float z) {
		//	modelMatrix_ = glm::rotate(modelMatrix_, angle, { x, y, z });
		//}

		//void SetTransform(const glm::mat4& transform) {
		//	/*if(modelObjFileName_ == "GrassBlock.obj" && Math::IsEqual(transform[3][1], 0.0f)) {
		//		__debugbreak();
		//	}*/
		//	modelMatrix_ = transform;
		//}

		//const glm::mat4& GetTransform() {
		//	return modelMatrix_;
		//}

		void SetId(Common::Index id) {
			modelId_ = id;
		}

		Common::Index GetId() {
			return modelId_;
		}

		std::string modelObjFileName_ = "";
		std::string modelMtlFileName_ = "";
		std::vector<std::string> textures_;
	private:
		Common::Index modelId_ = Common::Limits<Common::Index>::Max();
	};


	template<>
	inline void Edit<ImmutableRenderGeometry>(ImmutableRenderGeometry* immutableRenderGeometry) {


	}

}
