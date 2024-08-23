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
		ImmutableRenderGeometry(const std::string& meshTag) :
			ECSComponent{ nullptr } {}

<<<<<<< Updated upstream
		const std::string meshTag_;

		ImmutableRenderGeometry(
			std::string objFileName,
			std::string mtlFileName,
			const std::vector<std::string>& textures) :
			ECSComponent{ nullptr },
			modelObjFileName_{ objFileName },
			modelMtlFileName_{ mtlFileName },
			textures_{ textures } {}

		std::string modelObjFileName_ = "";
		std::string modelMtlFileName_ = "";
		std::vector<std::string> textures_;
=======
		std::string meshTag_;
>>>>>>> Stashed changes
	};


	template<>
	inline void Edit<ImmutableRenderGeometry>(ImmutableRenderGeometry* immutableRenderGeometry) {
		ImGui::TextDisabled("Mesh tag: \"%s\"", immutableRenderGeometry->meshTag_);
	}

	template<>
	inline void Add<ImmutableRenderGeometry>(ECS::World* world, ECS::Entity::Id id) {


	}


}
