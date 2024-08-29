#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class ChildEntity : public ECSComponent<ChildEntity> {
	public:

		ChildEntity(ECS::Entity::Id childEntityId) : ECSComponent{ nullptr }, id_ { childEntityId } {}

		ECS::Entity::Id id_ = ECS::Entity::Id::invalid_;
	};


	template<>
	inline void Edit<ChildEntity>(ChildEntity* childEntity) {
		static Common::Index childEntityId = Common::Limits<Common::Index>::Max();

		ImGui::InputScalar("Child entity id:", ImGuiDataType_U64, &childEntityId);
	}

	template<>
	inline void Add<ChildEntity>(ECS::World* world, ECS::Entity::Id id) {
		static Common::Index childEntityId = Common::Limits<Common::Index>::Max();
		if (ImGui::CollapsingHeader("Create info")) {
			static_assert(std::is_same_v<ECS::Entity::Id::ValueType, Common::Size>, "Changed type of entity idf or type of input field.");
			ImGui::InputScalar("Child entity id:", ImGuiDataType_U64, &childEntityId);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<ChildEntity>(id)) {
				world->CreateComponent<ChildEntity>(id, childEntityId);
			}
		}
	}


}