#pragma once 
#include <memory>
#include <PAL.World.hpp>
#include <PAL.RigidBody.hpp>

namespace PAL {

	class API {
	public:

		virtual std::shared_ptr<World> 
		CreateWorld(const PAL::World::CreateInfo& createInfo) = 0;

		virtual std::shared_ptr<RigidBody> 
		CreateRigidBody(const PAL::RigidBody::CreateInfo& createInfo) = 0;

	private:
	};

}