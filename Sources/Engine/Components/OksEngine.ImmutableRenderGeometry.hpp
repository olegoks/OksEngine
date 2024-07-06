#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
	public:
		ImmutableRenderGeometry() : ECSComponent{ nullptr }{

		}

		ImmutableRenderGeometry(Context*, float x, float y, float z, std::string obj, std::string mtl, std::string texture) : ECSComponent{ nullptr } {

		}

		void Rotate(float angle, float x, float y, float z) {
			modelMatrix_ = glm::rotate(modelMatrix_, angle, { x, y, z });
		}

		void SetTransform(const glm::mat4& transform) {
			/*if(modelObjFileName_ == "GrassBlock.obj" && Math::IsEqual(transform[3][1], 0.0f)) {
				__debugbreak();
			}*/
			modelMatrix_ = transform;
		}

		const glm::mat4& GetTransform() {
			return modelMatrix_;
		}

		void SetId(Common::Index id) {
			modelId_ = id;
		}

		Common::Index GetId() {
			return modelId_;
		}

		ImmutableRenderGeometry(
			Context* context,
			const glm::mat4& modelMatrix,
			std::string objFileName,
			std::string mtlFileName,
			std::string textureFileName) :
			ECSComponent{ context },
			modelMatrix_{ modelMatrix },
			modelObjFileName_{ objFileName },
			modelMtlFileName_{ mtlFileName },
			modelTextureFileName_{ textureFileName } {}
		std::string modelObjFileName_ = "";
		std::string modelMtlFileName_ = "";
		std::string modelTextureFileName_ = "";
	private:
		Common::Index modelId_ = Common::Limits<Common::Index>::Max();
		glm::mat4 modelMatrix_ = glm::identity<glm::mat4>();


	};


}
