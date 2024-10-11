#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Direction : public ECSComponent<Direction> {
	public:

		Direction() : ECSComponent{ nullptr } {}

		Direction(float x, float y, float z) : ECSComponent{ nullptr }, value_{ x, y, z } {}

		[[nodiscard]]
		float GetX() { return value_.x; }
		[[nodiscard]]
		float GetY() { return value_.y; }
		[[nodiscard]]
		float GetZ() { return value_.z; }


		void SetX(float x) { value_.x = x; }
		void SetY(float y) { value_.y = y; }
		void SetZ(float z) { value_.z = z; }

		void Rotate(float x, float y, float z, float angle) {
			value_ = glm::normalize(glm::rotate(
				value_,
				glm::radians(angle),
				{ x, y, z })
			);
		}


		glm::vec3 value_{ 1.0, 0.0, 0 };
	};


	template<>
	inline void Edit<Direction>(Direction* direction) {
		ImGui::InputFloat("X", &direction->value_.x);
		ImGui::InputFloat("Y", &direction->value_.y);
		ImGui::InputFloat("Z", &direction->value_.z);
	}

	template<>
	inline void Bind<Direction>(Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Direction>("Direction")
			.addConstructor<void(*)(int x, int y, int z)>()
			.addFunction("GetX", &Direction::GetX)
			.addFunction("GetY", &Direction::GetY)
			.addFunction("GetZ", &Direction::GetZ)
			.addFunction("SetX", &Direction::SetX)
			.addFunction("SetY", &Direction::SetY)
			.addFunction("SetZ", &Direction::SetZ)
			.addFunction("Rotate", &Direction::Rotate)
			.endClass();
	}

}