#pragma once 
#include <memory>
#include <PAL.World.hpp>
namespace PAL {

	class API {
	public:

		virtual std::shared_ptr<World> 
		CreateWorld(const PAL::World::CreateInfo& createInfo) = 0;

		virtual std::shared_ptr<DynamicRigidBody> 
		CreateRigidBody(const PAL::DynamicRigidBody::CreateInfo& createInfo) = 0;

		virtual std::shared_ptr<StaticRigidBody>
		CreateStaticRigidBody(const PAL::StaticRigidBody::CreateInfo& createInfo) = 0;

		virtual std::shared_ptr<Shape>
		CreateShape(const PAL::Shape::CreateInfoBox& createInfo) = 0;

		virtual std::shared_ptr<Shape>
		CreateShape(const PAL::Shape::CreateInfoCapsule& createInfo) = 0;

		virtual std::shared_ptr<Shape>
		CreateShape(const PAL::Shape::CreateInfoMesh& createInfo) = 0;

	private:
	};

}