#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Texture : public ECSComponent<Texture> {
	public:

		Texture(const std::string& name) : ECSComponent{ nullptr }, name_{ name } {}

		std::string name_ = "";
	};

	template<>
	inline void Edit<Texture>(Texture* texture) {
		ImGui::TextDisabled("Tag: \"%s\"", texture->name_.c_str());
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