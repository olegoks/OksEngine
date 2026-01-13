#pragma once 


namespace PAL {

	class Constraint {
	public:
		using Id = Common::Id;

		enum class Type {
			Fixed,
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
			PAL::RigidBody::Id second_;
			bool isBreakable_ = false;
			float breakForce_ = 1.0;
			float invMassScaleFirst_ = 1.0;
			float invMassScaleSecond_ = 1.0;
		};

		using CI = CreateInfo;

		FixedConstraint() : Constraint{ Type::Fixed } {

		}

	};

}