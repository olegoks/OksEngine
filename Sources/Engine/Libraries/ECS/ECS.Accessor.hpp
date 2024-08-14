#pragma once 

#include <ECS.Entity.hpp>

namespace ECS {

	class World;

	//class Accessor {
	//public:
	//	class Entity {
	//	public:

	//		Entity(World* world, ECS::Entity::Id id) : world_{ world }, id_{ id } {}

	//		template<class ComponentType>
	//		[[nodiscard]]
	//		ComponentType* GetComponent() noexcept {
	//			return world_->GetComponent<ComponentType>(id_);
	//		}

	//	private:
	//		World* world_ = nullptr;
	//		ECS::Entity::Id id_ = ECS::Entity::Id::invalid_;
	//	};
	//};

}