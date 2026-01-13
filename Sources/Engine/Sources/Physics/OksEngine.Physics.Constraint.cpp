#pragma once
#include <Physics\auto_OksEngine.Physics.Constraint.hpp>

namespace OksEngine
{

	namespace Physics
	{

		void EditConstraintEntityIds(std::shared_ptr<ECS2::World> ecsWorld, ConstraintEntityIds* constraintEntityIds) {
			NOT_IMPLEMENTED();
		}


		void BindConstraintEntityIds(::Lua::Context& context) {
			
			//TODO: implement accessing entity ids from lua.
			context.GetGlobalNamespace()
				.beginClass<ConstraintEntityIds>("ConstraintEntityIds")
				.endClass();

		}

		ConstraintEntityIds ParseConstraintEntityIds(luabridge::LuaRef& constraintEntityIdsRef) {
			NOT_IMPLEMENTED();
			return ConstraintEntityIds{};
		}

		std::string SerializeConstraintEntityIds(const ConstraintEntityIds* constraintEntityIds) {
			NOT_IMPLEMENTED();
			return "";
		}

		void CreateConstraint::Update(
			ECS2::Entity::Id entity0id, Physics::World* world0,

			ECS2::Entity::Id entity1id,
			const Physics::Constraint* constraint1,
			const Physics::ConstraintedRigidBodyEntityIds* constraintedRigidBodyEntityIds1) {

			const ECS2::ComponentsFilter constraintFilter = GetComponentsFilter(entity1id);

			if (constraintFilter.IsSet<Physics::FixedConstraint>()) {
				//Need to create fixed constraint.

				const ECS2::Entity::Id firstRbEntityId = constraintedRigidBodyEntityIds1->first_;
				const ECS2::Entity::Id secondRbEntityId = constraintedRigidBodyEntityIds1->second_;

				if (IsComponentExist<Physics::RigidBodyId>(firstRbEntityId) && IsComponentExist<Physics::RigidBodyId>(secondRbEntityId)) {
					auto* firstRbId = GetComponent<Physics::RigidBodyId>(firstRbEntityId);
					auto* secondRbId = GetComponent<Physics::RigidBodyId>(secondRbEntityId);


					PAL::FixedConstraint::CI ci{
						.first_ = firstRbId->id_,
						.second_ = secondRbId->id_,
						.isBreakable_ = constraintFilter.IsSet<Physics::Breakable>(),
						.breakForce_ = [&]() -> float {

							if (constraintFilter.IsSet<Physics::BreakForce>()) {
								return GetComponent<Physics::BreakForce>(entity1id)->value_;
							}

							return 0;
						}(),
						.invMassScaleFirst_ = [&]() -> float {
							if (constraintFilter.IsSet<Physics::InvMassScale>()) {
								return GetComponent<Physics::InvMassScale>(entity1id)->first_;
							}
							return 1.0;
						}(),
						.invMassScaleSecond_ = [&]() -> float {
							if (constraintFilter.IsSet<Physics::InvMassScale>()) {
								return GetComponent<Physics::InvMassScale>(entity1id)->second_;
							}
							return 1.0;
						}()
					};

					const PAL::Constraint::Id constraintId = world0->world_->CreateFixedConstraint(ci);

					CreateComponent<Physics::ConstraintId>(entity1id, constraintId);

				}

			}

		};

	}

} // namespace OksEngine