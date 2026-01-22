#pragma once
#include <Physics\auto_OksEngine.Physics.Constraint.hpp>

#define GLM_ENABLE_EXPERIMENTAL
namespace OksEngine
{

	namespace Physics
	{

		FirstConstraintedRigidBodyJointRotation3D ParseFirstConstraintedRigidBodyJointRotation3D(
			luabridge::LuaRef& firstConstraintedRigidBodyJointRotation3DRef) {



			FirstConstraintedRigidBodyJointRotation3D worldRotation3D;

			if (
				!firstConstraintedRigidBodyJointRotation3DRef["w"].isNil() &&
				!firstConstraintedRigidBodyJointRotation3DRef["x"].isNil() &&
				!firstConstraintedRigidBodyJointRotation3DRef["y"].isNil() &&
				!firstConstraintedRigidBodyJointRotation3DRef["z"].isNil()) {
				worldRotation3D.w_ = firstConstraintedRigidBodyJointRotation3DRef["w"].cast<float>().value();
				worldRotation3D.x_ = firstConstraintedRigidBodyJointRotation3DRef["x"].cast<float>().value();
				worldRotation3D.y_ = firstConstraintedRigidBodyJointRotation3DRef["y"].cast<float>().value();
				worldRotation3D.z_ = firstConstraintedRigidBodyJointRotation3DRef["z"].cast<float>().value();
			}
			else if (!firstConstraintedRigidBodyJointRotation3DRef["aroundX"].isNil() &&
				!firstConstraintedRigidBodyJointRotation3DRef["aroundY"].isNil() &&
				!firstConstraintedRigidBodyJointRotation3DRef["aroundZ"].isNil()) {


				float xRad = glm::radians(firstConstraintedRigidBodyJointRotation3DRef["aroundX"].cast<float>().value());
				float yRad = glm::radians(firstConstraintedRigidBodyJointRotation3DRef["aroundY"].cast<float>().value());
				float zRad = glm::radians(firstConstraintedRigidBodyJointRotation3DRef["aroundZ"].cast<float>().value());

				// —оздаем отдельные кватернионы дл€ каждой оси
				glm::quat qx = glm::angleAxis(xRad, glm::vec3(1.0f, 0.0f, 0.0f));
				glm::quat qy = glm::angleAxis(yRad, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat qz = glm::angleAxis(zRad, glm::vec3(0.0f, 0.0f, 1.0f));

				glm::quat result = qz * qy * qx;

				worldRotation3D.w_ = result.w;
				worldRotation3D.x_ = result.x;
				worldRotation3D.y_ = result.y;
				worldRotation3D.z_ = result.z;

			}

			return worldRotation3D;

		}

		void EditSecondConstraintedRigidBodyJointRotation3D(
			std::shared_ptr<ECS2::World> ecsWorld,
			SecondConstraintedRigidBodyJointRotation3D* secondConstraintedRigidBodyJointRotation3D) {

			ImGui::PushID(SecondConstraintedRigidBodyJointRotation3D::GetTypeId());
			ImGui::InputScalar("w", ImGuiDataType_Float, &secondConstraintedRigidBodyJointRotation3D->w_);
			ImGui::InputScalar("x", ImGuiDataType_Float, &secondConstraintedRigidBodyJointRotation3D->x_);
			ImGui::InputScalar("y", ImGuiDataType_Float, &secondConstraintedRigidBodyJointRotation3D->y_);
			ImGui::InputScalar("z", ImGuiDataType_Float, &secondConstraintedRigidBodyJointRotation3D->z_);

			ImGui::PopID();

		}

		void EditFirstConstraintedRigidBodyJointRotation3D(
			std::shared_ptr<ECS2::World> ecsWorld,
			FirstConstraintedRigidBodyJointRotation3D* firstConstraintedRigidBodyJointRotation3D) {

			ImGui::PushID(FirstConstraintedRigidBodyJointRotation3D::GetTypeId());
			ImGui::InputScalar("w", ImGuiDataType_Float, &firstConstraintedRigidBodyJointRotation3D->w_);
			ImGui::InputScalar("x", ImGuiDataType_Float, &firstConstraintedRigidBodyJointRotation3D->x_);
			ImGui::InputScalar("y", ImGuiDataType_Float, &firstConstraintedRigidBodyJointRotation3D->y_);
			ImGui::InputScalar("z", ImGuiDataType_Float, &firstConstraintedRigidBodyJointRotation3D->z_);

			ImGui::PopID();

		}

		SecondConstraintedRigidBodyJointRotation3D ParseSecondConstraintedRigidBodyJointRotation3D(
			luabridge::LuaRef& secondConstraintedRigidBodyJointRotation3DRef) {


			SecondConstraintedRigidBodyJointRotation3D worldRotation3D;

			if (
				!secondConstraintedRigidBodyJointRotation3DRef["w"].isNil() &&
				!secondConstraintedRigidBodyJointRotation3DRef["x"].isNil() &&
				!secondConstraintedRigidBodyJointRotation3DRef["y"].isNil() &&
				!secondConstraintedRigidBodyJointRotation3DRef["z"].isNil()) {
				worldRotation3D.w_ = secondConstraintedRigidBodyJointRotation3DRef["w"].cast<float>().value();
				worldRotation3D.x_ = secondConstraintedRigidBodyJointRotation3DRef["x"].cast<float>().value();
				worldRotation3D.y_ = secondConstraintedRigidBodyJointRotation3DRef["y"].cast<float>().value();
				worldRotation3D.z_ = secondConstraintedRigidBodyJointRotation3DRef["z"].cast<float>().value();
			}
			else if (!secondConstraintedRigidBodyJointRotation3DRef["aroundX"].isNil() &&
				!secondConstraintedRigidBodyJointRotation3DRef["aroundY"].isNil() &&
				!secondConstraintedRigidBodyJointRotation3DRef["aroundZ"].isNil()) {


				float xRad = glm::radians(secondConstraintedRigidBodyJointRotation3DRef["aroundX"].cast<float>().value());
				float yRad = glm::radians(secondConstraintedRigidBodyJointRotation3DRef["aroundY"].cast<float>().value());
				float zRad = glm::radians(secondConstraintedRigidBodyJointRotation3DRef["aroundZ"].cast<float>().value());

				// —оздаем отдельные кватернионы дл€ каждой оси
				glm::quat qx = glm::angleAxis(xRad, glm::vec3(1.0f, 0.0f, 0.0f));
				glm::quat qy = glm::angleAxis(yRad, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::quat qz = glm::angleAxis(zRad, glm::vec3(0.0f, 0.0f, 1.0f));

				glm::quat result = qz * qy * qx;

				worldRotation3D.w_ = result.w;
				worldRotation3D.x_ = result.x;
				worldRotation3D.y_ = result.y;
				worldRotation3D.z_ = result.z;

			}

			return worldRotation3D;

		}

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

						// —оздаем матрицу трансл€ции
						glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), {
							firstConstraintedRigidBodyJointPosition3D1->x_,
							firstConstraintedRigidBodyJointPosition3D1->y_,
							firstConstraintedRigidBodyJointPosition3D1->z_
							});

						//  омбинируем: сначала вращение, затем трансл€ци€ (TR)
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

						// —оздаем матрицу трансл€ции
						glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), {
							secondConstraintedRigidBodyJointPosition3D1->x_,
							secondConstraintedRigidBodyJointPosition3D1->y_,
							secondConstraintedRigidBodyJointPosition3D1->z_
							});

						//  омбинируем: сначала вращение, затем трансл€ци€ (TR)
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
					.disableCollision_ = constraintFilter.IsSet<Physics::DisableCollision>(),
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