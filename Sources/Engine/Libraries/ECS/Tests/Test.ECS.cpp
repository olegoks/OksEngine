#include <gtest/gtest.h>

#include <ECS.hpp>

// Demonstrate some basic assertions.
TEST(TestECS, CreatingWorld) {
	
	using namespace ECS;

	World world;

}

struct Position : public ECS::IComponent<Position> {
	int x = 0;
	int y = 0;
	int z = 0;
};

TEST(TestECS, CreatingEntity) {

	using namespace ECS;

	World world;
	Entity::Id entityId = world.CreateEntity();
	OS::AssertMessage(entityId != Entity::Id::invalid_,
		"Created entity with invalid id.");

	world.CreateComponent<Position>(entityId);

	class TestSystem : public System {
	public:

		virtual void Update(World* world, Entity::Id entityId) override {
			Position* position = world->GetComponent<Position>(entityId);
			position->x++;
		}

		virtual Common::TypeId GetTypeId() const noexcept override {
			return Common::TypeInfo<TestSystem>().GetId();
		}
		virtual ECS::Entity::Filter GetFilter() const noexcept override {
			return ECS::Entity::Filter{};
		}
		virtual void BeforeUpdate(ECS::World* world) override{}
		virtual void AfterUpdate(ECS::World* world) override{}
		virtual void StartUpdate() override { }
		virtual void EndUpdate() override { }

	};

	world.RegisterSystem<TestSystem>();
	world.Process();
	Position* position = world.GetComponent<Position>(entityId);
	OS::AssertMessage(position->x == 1,
		"Incorrect value of a component field. Maybe system was not started or not one time.");

}
