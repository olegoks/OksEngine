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

		const std::string meshTag_;
	};


	template<>
	inline void Edit<ImmutableRenderGeometry>(ImmutableRenderGeometry* immutableRenderGeometry) {
		ImGui::TextDisabled("Mesh tag: \"%s\"", immutableRenderGeometry->meshTag_);
	}

	template<>
	inline void Add<ImmutableRenderGeometry>(ECS::World* world, ECS::Entity::Id id) {


	}


}
