
#pragma once 

#include <OksEngine.Subsystem.hpp>
#include <PAL.API.hpp>
#include <PE.PhysicsEngine.hpp>
#include <Geometry.Model.hpp>
#include <PAL.RigidBody.hpp>

namespace OksEngine {

	class PhysicsSubsystem : public Subsystem {
	public:


		struct CreateInfo {
			Context& context_;
		};

		PhysicsSubsystem(const CreateInfo& createInfo);


		std::shared_ptr < Geom::Model<Geom::Vertex3f>> GetGeom(const std::string& geomName);
		Common::Index CreateRigidBody(const PAL::RigidBody::CreateInfo& createInfo);
		Common::Index CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& createInfo);

		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoBox& createInfo);
		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo);
		std::shared_ptr<PAL::Shape> CreateShape(const PAL::Shape::CreateInfoMesh& createInfo);
		void AddRigidBodyToWorld(Common::Index rbIndex);
		void AddStaticRigidBodyToWorld(Common::Index rbIndex);
		[[nodiscard]]
		glm::mat4 GetRigidBodyTransform(Common::Index rbIndex) {
			return rigidBodies_[rbIndex]->GetTransform();
		}

		[[nodiscard]]
		void  SetRigidBodyTransform(Common::Index rbIndex, const glm::mat4& transform) {
			return rigidBodies_[rbIndex]->SetTransform(transform);
		}

		void ApplyForce(Common::Index rbIndex, const glm::vec3& direction, float force);
		void SetVelocity(Common::Index rbIndex, const glm::vec3& direction, float velocity);
		virtual void Update() noexcept override;


	private:
		std::vector<std::shared_ptr<PAL::RigidBody>> rigidBodies_;
		std::vector<std::shared_ptr<PAL::StaticRigidBody>> staticRigidBodies_;
		std::shared_ptr<PAL::World> world_ = nullptr;
		std::shared_ptr<PE::PhysicsEngine> physicsEngine_ = nullptr;
	};

}