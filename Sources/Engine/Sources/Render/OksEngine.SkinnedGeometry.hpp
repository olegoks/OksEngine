#pragma once

#include <OksEngine.ECS.Component.hpp>
#include <Math.Matrix.hpp>
#include <OksEngine.Context.hpp>

namespace OksEngine {


	struct SkinnedGeometry : public ECSComponent<SkinnedGeometry> {
	public:
		SkinnedGeometry() : ECSComponent{ nullptr } {

		}

		SkinnedGeometry(Context*, std::string obj, std::string mtl, const std::vector<std::string>& textures) : ECSComponent{ nullptr } {

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

		SkinnedGeometry(
			Context* context,
			const glm::mat4& modelMatrix,
			std::string objFileName,
			std::string mtlFileName,
			const std::vector<std::string>& textures) :
			ECSComponent{ context },
			modelMatrix_{ modelMatrix },
			modelObjFileName_{ objFileName },
			modelMtlFileName_{ mtlFileName },
			textures_{ textures } {}
		std::string modelObjFileName_ = "";
		std::string modelMtlFileName_ = "";
		std::vector<std::string> textures_;
	private:
		Common::Index modelId_ = Common::Limits<Common::Index>::Max();
		glm::mat4 modelMatrix_ = glm::identity<glm::mat4>();


	};


	template<>
	inline void Edit<SkinnedGeometry>(SkinnedGeometry* skinnedGeometry) {

	}

}