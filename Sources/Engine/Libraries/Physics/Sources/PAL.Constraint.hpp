#pragma once 


namespace PAL {

	class Constraint {
	public:
		using Id = Common::Id;

		enum class Type {
			Fixed,
			Revolute,
			Undefined
		};
		Constraint(Type type) : type_{ type } {
			
		}

		[[nodiscard]]
		Type GetType() const noexcept {
			return type_;
		}

	private:
		Type type_ = Type::Undefined;
	};

	class FixedConstraint : public Constraint {
	public:

		struct CreateInfo {
			PAL::RigidBody::Id first_;
			glm::mat4 firstRelativeTr_ = glm::mat4{ 1 };
			PAL::RigidBody::Id second_;
			glm::mat4 secondRelativeTr_ = glm::mat4{ 1 };
			bool isBreakable_ = false;
			float breakForce_ = 1.0;
			float invMassScaleFirst_ = 1.0;
			float invMassScaleSecond_ = 1.0;
		};

		using CI = CreateInfo;

		FixedConstraint() : Constraint{ Type::Fixed } {

		}

	};

	class RevoluteConstraint : public Constraint {
	public:
		struct CreateInfo {
			PAL::RigidBody::Id first_;
			glm::mat4 firstRelativeTr_ = glm::mat4{ 1 };
			PAL::RigidBody::Id second_;
			glm::mat4 secondRelativeTr_ = glm::mat4{ 1 };
			glm::vec3 rotateAxis_ = { 0.0, 1.0, 0.0 };
			bool isBreakable_ = false;
			float breakForce_ = 1.0;
			bool disableCollision_ = false;
			float invMassScaleFirst_ = Common::Limits<float>::Max();
			float invMassScaleSecond_ = Common::Limits<float>::Max();
		};
		using CI = CreateInfo;

		RevoluteConstraint() : Constraint{ Type::Revolute } {

		}

	};

}