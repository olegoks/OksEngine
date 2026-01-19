#pragma once 
#include <PAL.World.hpp>
#include <PxScene.h>
#include <physx/characterkinematic/PxControllerManager.h>

namespace PhysX {

	class World : public PAL::World {
	public:

		struct CreateInfo {
			PAL::World::CreateInfo palWorldCreateInfo_;
			physx::PxPhysics* physics_ = nullptr;
		};

		World(const CreateInfo& createInfo);

		virtual PAL::DynamicRigidBody::Id CreateDynamicRigidBody(const PAL::DynamicRigidBody::CreateInfo& drbCreateInfo) override;
		virtual PAL::StaticRigidBody::Id CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& srbCreateInfo) override;

		[[nodiscard]]
		physx::PxRigidActor* GetRigidBodyActor(PAL::RigidBody::Id rbId);
		

		virtual PAL::Constraint::Id CreateFixedConstraint(const PAL::FixedConstraint::CI& ci) override;
		virtual PAL::Constraint::Id CreateRevoluteConstraint(const PAL::RevoluteConstraint::CI& ci) override;

		[[nodiscard]]
		virtual PAL::CapsuleController::Id CreateCapsuleController(const PAL::CapsuleController::CreateInfo& srbCreateInfo) override;

		virtual void AddDynamicRigidBody(PAL::DynamicRigidBody::Id drbId) override;
		virtual void ApplyForce(PAL::DynamicRigidBody::Id drbId) override;
		virtual Math::Vector3f GetRigidBodyPosition(PAL::DynamicRigidBody::Id drbId) override;
		virtual glm::quat GetRigidBodyRotation(PAL::DynamicRigidBody::Id drbId) override;

		virtual void AddStaticRigidBody(PAL::StaticRigidBody::Id srbId) override;

		virtual void Simulate(float ms) override;
		PAL::CapsuleController::Id GenerateCapsuleControllerId(std::shared_ptr<physx::PxController> controller) {
			PAL::CapsuleController::Id rbId = capsuleControllerIdGenerator_.Generate();
			IdCapsuleController_[rbId] = controller;
			return rbId;
		}

		virtual void SetDebugRenderParameters(DebugRenderParameters parameter, bool value) noexcept override;

		virtual void GetDebugRenderBufferPoints() const noexcept override {

		}

		virtual Geom::VertexCloud<Geom::Vertex3fc> GetDebugRenderBufferTriangles() const noexcept override {

			const physx::PxDebugTriangle* triangles = scene_->getRenderBuffer().getTriangles();
			const Common::UInt32 trianglesNumber = scene_->getRenderBuffer().getNbTriangles();

			Geom::VertexCloud<Geom::Vertex3fc> vertices;
			vertices.Reserve(trianglesNumber * 3);
			for (Common::Index i = 0; i < trianglesNumber; i++) {
				const physx::PxDebugTriangle* triangle = triangles + i;
				vertices.Add(
					{ Geom::Vertex3f{ triangle->pos0.x, triangle->pos0.y, triangle->pos0.z  },
					Geom::Color3f{ 0.0, 1.0, 0.0 }
					//Geom::Color3f{ triangle->color0 >> 24, (triangle->color0 << 8) >> 32, (triangle->color0 << 16) >> 32 } 
					});
				vertices.Add(
					{ Geom::Vertex3f{ triangle->pos1.x, triangle->pos1.y, triangle->pos1.z  },
					Geom::Color3f{ 0.0, 1.0, 0.0 }
					//Geom::Color3f{ triangle->color1 >> 24, (triangle->color1 << 8) >> 32, (triangle->color1 << 16) >> 32} 
					});
				vertices.Add(
					{ Geom::Vertex3f{ triangle->pos2.x, triangle->pos2.y, triangle->pos2.z  },
					Geom::Color3f{ 0.0, 1.0, 0.0 }
					//Geom::Color3f{ triangle->color2 >> 24, (triangle->color2 << 8) >> 32, (triangle->color2 << 16) >> 32}
					});
			}

			return vertices;

		}
		virtual Geom::VertexCloud<Geom::Vertex3fc> GetDebugRenderBufferLines() const noexcept override {

			const physx::PxDebugLine* lines = scene_->getRenderBuffer().getLines();
			const Common::UInt32 linesNumber = scene_->getRenderBuffer().getNbLines();

			Geom::VertexCloud<Geom::Vertex3fc> vertices;

			vertices.Reserve(linesNumber * 2);
			for (Common::Index i = 0; i < linesNumber; i++) {
				const physx::PxDebugLine* line = lines + i;
				vertices.Add(
					{ Geom::Vertex3f{ line->pos0.x, line->pos0.y, line->pos0.z  },
					Geom::Color3f{ 1.0, 0.0, 0.0 }
					//Geom::Color3f{ triangle->color0 >> 24, (triangle->color0 << 8) >> 32, (triangle->color0 << 16) >> 32 } 
					});
				vertices.Add(
					{ Geom::Vertex3f{ line->pos1.x, line->pos1.y, line->pos1.z  },
					Geom::Color3f{ 1.0, 0.0, 0.0 }
					//Geom::Color3f{ triangle->color1 >> 24, (triangle->color1 << 8) >> 32, (triangle->color1 << 16) >> 32} 
					});
			}

			return vertices;
		}

	private:
		physx::PxScene* scene_ = nullptr;
		physx::PxPhysics* physics_ = nullptr;


		physx::PxControllerManager* controllerManager_ = nullptr;
		Common::IdGenerator capsuleControllerIdGenerator_;
		std::map<PAL::CapsuleController::Id, std::shared_ptr<physx::PxController>> IdCapsuleController_;
	};

}