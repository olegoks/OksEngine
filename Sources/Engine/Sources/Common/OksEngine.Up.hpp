#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Up : public ECSComponent<Up> {
	public:

		Up() : ECSComponent{ nullptr } {}

		Up(float x, float y, float z) : ECSComponent{ nullptr }, value_{ x, y, z } {}

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


		glm::vec3 value_{ 0.0, 1.0, 0 };
	};


	template<>
	inline void Edit<Up>(Up* up) {
		ImGui::InputFloat("X", &up->value_.x);
		ImGui::InputFloat("Y", &up->value_.y);
		ImGui::InputFloat("Z", &up->value_.z);
	}

	template<>
	inline void Bind<Up>(Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Up>("Up")
			.addConstructor<void(*)(int x, int y, int z)>()
			.addFunction("GetX", &Up::GetX)
			.addFunction("GetY", &Up::GetY)
			.addFunction("GetZ", &Up::GetZ)
			.addFunction("SetX", &Up::SetX)
			.addFunction("SetY", &Up::SetY)
			.addFunction("SetZ", &Up::SetZ)
			.addFunction("Rotate", &Up::Rotate)
			.endClass();
	}

}