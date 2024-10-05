#pragma once 

#include <Math.Vector.hpp>

#include <PAL.RigidBody.hpp>

namespace PAL {

	class World {
	public:
		struct CreateInfo {
			Math::Vector3f gravity_{ 0.f, -9.81, 0.f };
		};

		World(const CreateInfo& createInfo) : 
			createInfo_{ createInfo } { }

		[[nodiscard]]
		virtual PAL::DynamicRigidBody::Id CreateDynamicRigidBody(const PAL::DynamicRigidBody::CreateInfo& drbCreateInfo) = 0;
		[[nodiscard]]
		virtual PAL::StaticRigidBody::Id CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& srbCreateInfo) = 0;

		[[nodiscard]]
		std::shared_ptr<RigidBody> GetRigidBodyById(RigidBody::Id drbid);

		virtual void AddDynamicRigidBody(PAL::DynamicRigidBody::Id drbId) = 0;

		virtual void AddStaticRigidBody(PAL::StaticRigidBody::Id srbId) = 0;

		virtual void Simulate(float ms) = 0;


	protected:

		[[nodiscard]]
		RigidBody::Id GenerateId(std::shared_ptr<RigidBody> drbPtr);

	private:
		CreateInfo createInfo_;
		Common::IdGenerator rbIdGenerator_;
		std::map<PAL::RigidBody::Id, std::shared_ptr<RigidBody>> IdRb_;

	};

}