#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Texture.hpp>


#include <Geometry.Model.hpp>

#include <Geometry.Storage.hpp>

#include <RAL.Driver.hpp>
#include <Resources/OksEngine.Resource.hpp>

namespace OksEngine {

	struct ImmutableRenderGeometry : public ECSComponent<ImmutableRenderGeometry> {
	public:
		ImmutableRenderGeometry();
		ImmutableRenderGeometry(std::string meshTag) :
			ECSComponent{ nullptr }, meshTag_{ meshTag } {}

		std::string meshTag_;
		std::string shaderTag_;
	};


	template<>
	inline void Edit<ImmutableRenderGeometry>(ImmutableRenderGeometry* immutableRenderGeometry) {
		ImGui::TextDisabled("Mesh tag %s:", immutableRenderGeometry->meshTag_.c_str());
	}

	template<>
	inline void Add<ImmutableRenderGeometry>(ECS::World* world, ECS::Entity::Id id) {
		static char geom[100] = { "dragon_lore.geom" };
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Geometry description file:", geom, sizeof(geom));
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<ImmutableRenderGeometry>(id)) {
				world->CreateComponent<ImmutableRenderGeometry>(id, geom);
			}
		}

	}


	template<>
	inline void Bind<ImmutableRenderGeometry>(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<ImmutableRenderGeometry>("ImmutableRenderGeometry")
			.addConstructor<void(*)(std::string)>()
			.endClass();
	}











}
