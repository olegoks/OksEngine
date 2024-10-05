#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Model : public ECSComponent<Model> {

		Model() :
			ECSComponent{ nullptr } { }

		Model(const std::string& name/*, const std::vector<ECS::Entity::Id>& meshs*/) :
			ECSComponent{ nullptr },
			name_{ name }/*,
			meshs_{ meshs } */{ }

		std::string name_;
	};

	template<>
	inline void Edit<Model>(Model* model) {
		ImGui::TextDisabled("Name: %s", model->name_.c_str());
		//ImGui::TextDisabled("Meshs:");
		//ImGui::Indent(20.0f);
		//for (auto& meshEntityId : model->meshs_) {
		//	ImGui::TextDisabled("%d", meshEntityId);
		//}
		//ImGui::Unindent(20.0f);

	}


}