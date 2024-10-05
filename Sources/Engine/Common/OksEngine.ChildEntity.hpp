#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class ChildEntity : public ECSComponent<ChildEntity> {
	public:

		ChildEntity(ECS::Entity::Id childEntityId);

		ECS::Entity::Id id_ = ECS::Entity::Id::invalid_;
	};


	template<>
	void Edit<ChildEntity>(ChildEntity* childEntity);

	template<>
	void Add<ChildEntity>(ECS::World* world, ECS::Entity::Id id);


	class ChildEntities : public ECSComponent<ChildEntities> {
	public:

		ChildEntities(const std::vector<ECS::Entity::Id>& entitiesIds) 
			: ECSComponent{ nullptr },
			entitiesIds_{ entitiesIds } {

		}

		std::vector<ECS::Entity::Id> entitiesIds_;
	};

	template<>
	inline void Edit<ChildEntities>(ChildEntities* childEntity) {
		ImGui::TextDisabled("Entities: ");
		for (ECS::Entity::Id id : childEntity->entitiesIds_) {
			ImGui::TextDisabled("%d", id);
		}
	}


}