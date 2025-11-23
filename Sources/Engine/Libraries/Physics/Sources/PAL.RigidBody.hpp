#pragma once 

#include <memory>
#include <Math.Matrix.hpp>
#include <Geometry.Shapes.hpp>
#include <PAL.Shape.hpp>

namespace PAL {

	class RigidBody {
	public:
		using Id = Common::Id;

		struct CreateInfo {
			glm::mat4 transform_;
			std::shared_ptr<Shape> shape_ = nullptr;
			std::string name_ = "No name";
		};

		enum class Type {
			Dynamic,
			Static,
			Undifned
		};

		RigidBody(const CreateInfo& createInfo) : createInfo_{ createInfo } {

		}

		[[nodiscard]]
		virtual void SetTransform(const glm::mat4& transform) = 0;
		[[nodiscard]]
		virtual const glm::mat4 GetTransform() = 0;

		[[nodiscard]]
		virtual Type GetType() const noexcept = 0;

	private:
		CreateInfo createInfo_;
	};

	using RB = RigidBody;

	class DynamicRigidBody : public RigidBody {
	public:


		struct CreateInfo {
			RigidBody::CreateInfo rbCreateInfo_;
			float mass_ = 10.f;
			float linearDamping_ = 2.f;
			float angularDamping_ = 0.f;
			bool lockAngularX_ = false;
			bool lockAngularZ_ = false;
			
		};

		DynamicRigidBody(const CreateInfo& createInfo) : RigidBody{ createInfo.rbCreateInfo_ }, createInfo_ { createInfo } {

		}

		virtual void ApplyForce(const glm::vec3& direction, float force) = 0;
		virtual void SetLinearVelocity(const glm::vec3& direction, float velocity) = 0;

		virtual Type GetType() const noexcept override {
			return Type::Dynamic;
		}

	private:
		CreateInfo createInfo_;
	};

	using DRB = DynamicRigidBody;

	class StaticRigidBody : public RigidBody {
	public:

		struct CreateInfo {
			RigidBody::CreateInfo rbCreateInfo_;
		};

		StaticRigidBody(const CreateInfo& createInfo) : RigidBody{ createInfo.rbCreateInfo_ }, createInfo_{ createInfo } {

		}


		virtual Type GetType() const noexcept override {
			return Type::Static;
		}

	private:
		CreateInfo createInfo_;
	};

	using SRB = StaticRigidBody;


}