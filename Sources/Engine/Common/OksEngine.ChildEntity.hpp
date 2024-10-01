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


}