
#include <Common/auto_OksEngine.ChildEntity.hpp>
#include <Common/auto_OksEngine.ChildEntities.hpp>

namespace OksEngine {

	//ChildEntity::ChildEntity(ECS::Entity::Id childEntityId) : ECSComponent{ nullptr }, id_{ childEntityId } {}

	//template<>
	//void Edit<ChildEntity>(ChildEntity* childEntity) {
	//	static Common::Index childEntityId = Common::Limits<Common::Index>::Max();

	//	ImGui::InputScalar("Child entity id:", ImGuiDataType_U64, &childEntityId);
	//}

	//template<>
	//void Add<ChildEntity>(ECS::World* world, ECS::Entity::Id id) {
	//	static Common::Index childEntityId = Common::Limits<Common::Index>::Max();
	//	if (ImGui::CollapsingHeader("Create info")) {
	//		static_assert(std::is_same_v<ECS::Entity::Id::ValueType, Common::Size>, "Changed type of entity idf or type of input field.");
	//		ImGui::InputScalar("Child entity id:", ImGuiDataType_U64, &childEntityId);
	//		ImGui::Spacing();
	//	}
	//	if (ImGui::Button("Add component")) {
	//		if (!world->IsComponentExist<ChildEntity>(id)) {
	//			world->CreateComponent<ChildEntity>(id, childEntityId);
	//		}
	//	}
	//}

}