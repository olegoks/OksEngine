#pragma once

#include <ECS.hpp>
#include <imgui.h>
#include <Math.Vector.hpp>

#include <OksEngine.ECS.Component.hpp>
#include <Behaviour/OksEngine.Math3D.hpp>
namespace OksEngine {

	struct Scale2D : public ECSComponent<Scale2D> {
		float x_ = 0.f;
		float y_ = 0.f;

		Scale2D() = default;
		Scale2D(float x, float y) :
			ECSComponent{ nullptr },
			x_{ x },
			y_{ y } {}

		Scale2D(const glm::vec2& scale/*, const glm::quat& rotate*/) :
			ECSComponent{ nullptr },
			x_{ scale.x },
			y_{ scale.y } /*,
			rotate_{ rotate }*/ {}

		[[nodiscard]]
		float GetX() { return x_; }
		[[nodiscard]]
		float GetY() { return y_; }

		[[nodiscard]]
		const glm::vec2 GetVec() const noexcept { return glm::vec2{ x_, y_ }; }

		void SetX(float x) { x_ = x; }
		void SetY(float y) { y_ = y; }

	};

	template<>
	inline void Edit<Scale2D>(Scale2D* position) {
		ImGui::InputFloat("X", &position->x_);
		ImGui::InputFloat("Y", &position->y_);
	}

	template<>
	inline void Bind<Scale2D>(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Scale2D>("Scale2D")
			.addConstructor<void(*)(int x, int y)>()
			.addFunction("GetX", &Scale2D::GetX)
			.addFunction("GetY", &Scale2D::GetY)
			.addFunction("SetX", &Scale2D::SetX)
			.addFunction("SetY", &Scale2D::SetY)
			.endClass();
	}


	template<>
	inline void Add<Scale2D>(ECS::World* world, ECS::Entity::Id id) {
		static float x = 0.0f;
		static float y = 0.0f;
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputFloat("X", &x);
			ImGui::InputFloat("Y", &y);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Scale2D>(id)) {
				world->CreateComponent<Scale2D>(id, x, y);
			}
		}
	}


}
