#pragma once 

#include <memory>
#include <Math.Matrix.hpp>
#include <Geometry.Shapes.hpp>

namespace PAL {

	class RigidBody {
	public:

		struct CreateInfo {
			glm::mat4 transform_;
			float staticFriction_ = 0.5f;
			float dynamicFriction_ = 0.5f;
			float restitution_ = 0.5f;
			float halfExtentX_ = 1.f;
			float halfExtentY_ = 1.f;
			float halfExtentZ_ = 1.f;
			//std::shared_ptr<Geom::Box> shape_ = nullptr;
		};

		RigidBody(const CreateInfo& createInfo) {

		}

		[[nodiscard]]
		virtual const glm::mat4 GetTransform() = 0;

	private:
		CreateInfo createInfo_;
	};

}