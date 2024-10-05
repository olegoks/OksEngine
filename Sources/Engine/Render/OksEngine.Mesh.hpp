#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Mesh : public ECSComponent<Mesh> {
	public:
		Mesh() : ECSComponent{ nullptr } {

		}
		Mesh(const Geom::Model2::Id& modelId) :
			ECSComponent{ nullptr }, modelId_{ modelId } {}
		Geom::Model2::Id modelId_;

	};

	template<>
	inline void Edit<Mesh>(Mesh* mesh) {
		ImGui::TextDisabled("Model id: \"%d\"", mesh->modelId_);
	}


}