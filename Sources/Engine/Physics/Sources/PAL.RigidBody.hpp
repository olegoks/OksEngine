#pragma once 

#include <memory>
#include <Math.Matrix.hpp>
#include <Geometry.Shapes.hpp>
#include <PAL.Shape.hpp>

namespace PAL {

	class RigidBody {
	public:

		struct CreateInfo {
			glm::mat4 transform_;
			float mass_ = 10.f;
			float linearDamping_ = 2.f;
			float angularDamping_ = 0.f;
			std::shared_ptr<Shape> shape_ = nullptr;
			std::string name_ = "No name";
		};

		RigidBody(const CreateInfo& createInfo) {

		}

		[[nodiscard]]
		virtual const glm::mat4 GetTransform() = 0;
		[[nodiscard]]
		virtual void SetTransform(const glm::mat4& transform) = 0;
		virtual void ApplyForce(const glm::vec3& direction, float force) = 0;
		virtual void SetVelocity(const glm::vec3& direction, float velocity) = 0;

	private:
		CreateInfo createInfo_;
	};

	class StaticRigidBody {
	public:

		struct CreateInfo {
			glm::mat4 transform_;
			std::shared_ptr<Shape> shape_ = nullptr;
			std::string name_ = "No name";
		};

		StaticRigidBody(const CreateInfo& createInfo) {

		}

		[[nodiscard]]
		virtual const glm::mat4 GetTransform() = 0;

	private:
		CreateInfo createInfo_;
	};



}