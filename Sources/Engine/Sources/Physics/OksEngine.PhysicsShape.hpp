#pragma once 

#include <OksEngine.ECS.Component.hpp>
#include <OksEngine.ECS.System.hpp>
#include <PAL.Shape.hpp>

namespace OksEngine {

	class PhysicsShape : public ECSComponent<PhysicsShape> {
	public:

		PhysicsShape() : ECSComponent{ nullptr } {}

		PhysicsShape(std::shared_ptr<PAL::Shape> shape) :
			ECSComponent{ nullptr },
			shape_{ shape } {
			
		}

		std::shared_ptr<PAL::Shape> shape_ = nullptr;

	};

}