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

		virtual void AddRigidBody(std::shared_ptr<PAL::DynamicRigidBody> rigidBody) = 0;
		virtual void AddStaticRigidBody(std::shared_ptr<PAL::StaticRigidBody> rigidBody) = 0;

		virtual void Simulate(float ms) = 0;

	private:
		CreateInfo createInfo_;
	};

}