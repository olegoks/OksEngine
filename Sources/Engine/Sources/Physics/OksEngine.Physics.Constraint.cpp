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
			ECS2::Entity::Id entity0id, Physics::World* world0, ECS2::Entity::Id entity1id,
			const Physics::Constraint* constraint1,
			const Physics::FirstConstraintedRigidBodyEntityId* firstConstraintedRigidBodyEntityId1,
			const Physics::FirstConstraintedRigidBodyJointPosition3D* firstConstraintedRigidBodyJointPosition3D1,
			const Physics::FirstConstraintedRigidBodyJointRotation3D* firstConstraintedRigidBodyJointRotation3D1,
			const Physics::SecondConstraintedRigidBodyEntityId* secondConstraintedRigidBodyEntityId1,
			const Physics::SecondConstraintedRigidBodyJointPosition3D* secondConstraintedRigidBodyJointPosition3D1,
			const Physics::SecondConstraintedRigidBodyJointRotation3D* secondConstraintedRigidBodyJointRotation3D1) {

			const ECS2::ComponentsFilter constraintFilter = GetComponentsFilter(entity1id);

			const ECS2::Entity::Id firstRbEntityId = firstConstraintedRigidBodyEntityId1->id_;
			const ECS2::Entity::Id secondRbEntityId = secondConstraintedRigidBodyEntityId1->id_;

			if (!IsComponentExist<Physics::RigidBodyId>(firstRbEntityId) || !IsComponentExist<Physics::RigidBodyId>(secondRbEntityId)) {
				return;
			}

			auto* firstRbId = GetComponent<Physics::RigidBodyId>(firstRbEntityId);
			auto* secondRbId = GetComponent<Physics::RigidBodyId>(secondRbEntityId);

			if (constraintFilter.IsSet<Physics::FixedConstraint>()) {
				//Need to create fixed constraint.

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
			else if (constraintFilter.IsSet<Physics::RevoluteConstraint>()) {

				PAL::RevoluteConstraint::CI ci{
					.first_ = firstRbId->id_,
					.firstRelativeTr_ = [&]() -> glm::mat4 {

						glm::mat4 rotationMatrix = glm::mat4_cast(glm::quat{
							firstConstraintedRigidBodyJointRotation3D1->w_,
							firstConstraintedRigidBodyJointRotation3D1->x_,
							firstConstraintedRigidBodyJointRotation3D1->y_,
							firstConstraintedRigidBodyJointRotation3D1->z_
							});

						// Создаем матрицу трансляции
						glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), {
							firstConstraintedRigidBodyJointPosition3D1->x_,
							firstConstraintedRigidBodyJointPosition3D1->y_,
							firstConstraintedRigidBodyJointPosition3D1->z_
							});

						// Комбинируем: сначала вращение, затем трансляция (TR)
						return translationMatrix * rotationMatrix;

						
					}(),
					.second_ = secondRbId->id_,
					.secondRelativeTr_ = [&]() -> glm::mat4 {

						glm::mat4 rotationMatrix = glm::mat4_cast(glm::quat{
							secondConstraintedRigidBodyJointRotation3D1->w_,
							secondConstraintedRigidBodyJointRotation3D1->x_,
							secondConstraintedRigidBodyJointRotation3D1->y_,
							secondConstraintedRigidBodyJointRotation3D1->z_
							});

						// Создаем матрицу трансляции
						glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), {
							secondConstraintedRigidBodyJointPosition3D1->x_,
							secondConstraintedRigidBodyJointPosition3D1->y_,
							secondConstraintedRigidBodyJointPosition3D1->z_
							});

						// Комбинируем: сначала вращение, затем трансляция (TR)
						return translationMatrix * rotationMatrix;


					}(),
					.rotateAxis_ = [&]() -> glm::vec3 {
						
						auto* axis = GetComponent<Physics::RevoluteAxis>(entity1id);

						return { axis->x_, axis->y_, axis->z_ };
					}(),
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

				const PAL::Constraint::Id constraintId = world0->world_->CreateRevoluteConstraint(ci);

				CreateComponent<Physics::ConstraintId>(entity1id, constraintId);

			}

		};

	}

} // namespace OksEngine