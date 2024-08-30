#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Texture : public ECSComponent<Texture> {
	public:
		std::string tag_ = "";
		Geom::Texture::Id id_;
	};

	template<>
	inline void Edit<Texture>(Texture* texture) {
		ImGui::TextDisabled("Tag: \"%s\"", texture->tag_.c_str());
		ImGui::TextDisabled("Id: %d", texture->id_);
	}

	template<>
	inline void Add<Texture>(ECS::World* world, ECS::Entity::Id id) {
		//static char geom[100] = { "dragon_lore.geom" };
		//if (ImGui::CollapsingHeader("Create info")) {
		//	ImGui::InputText("Geometry description file:", geom, sizeof(geom));
		//}
		//if (ImGui::Button("Add component")) {
		//	if (!world->IsComponentExist<ImmutableRenderGeometry>(id)) {
		//		world->CreateComponent<ImmutableRenderGeometry>(id, geom);
		//	}
		//}

	}


}