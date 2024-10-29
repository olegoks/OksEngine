#pragma once

#include <ECS.hpp>
#include <imgui.h>
#include <Math.Vector.hpp>

#include <OksEngine.ECS.Component.hpp>
#include <Behaviour/OksEngine.Math3D.hpp>
namespace OksEngine {

	struct Position2D : public ECSComponent<Position2D> {
		float x_ = 0.f;
		float y_ = 0.f;

		Position2D() = default;
		Position2D(float x, float y) :
			ECSComponent{ nullptr },
			x_{ x },
			y_{ y } {}

		Position2D(const glm::vec2& translate/*, const glm::quat& rotate*/) :
			ECSComponent{ nullptr },
			x_{ translate.x },
			y_{ translate.y } /*,
			rotate_{ rotate }*/ {}

		[[nodiscard]]
		float GetX() { return x_; }
		[[nodiscard]]
		float GetY() { return y_; }

		[[nodiscard]]
		const glm::vec2 GetVec() const noexcept { return glm::vec2{ x_, y_ }; }

		void SetX(float x) { x_ = x; }
		void SetY(float y) {y_ = y; }

	};

	template<>
	inline void Edit<Position2D>(Position2D* position) {
		ImGui::InputFloat("X", &position->x_);
		ImGui::InputFloat("Y", &position->y_);
	}

	template<>
	inline void Bind<Position2D>(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Position2D>("Position2D")
			.addConstructor<void(*)(int x, int y)>()
			.addFunction("GetX", &Position2D::GetX)
			.addFunction("GetY", &Position2D::GetY)
			.addFunction("SetX", &Position2D::SetX)
			.addFunction("SetY", &Position2D::SetY)
			.endClass();
	}


	template<>
	inline void Add<Position2D>(ECS::World* world, ECS::Entity::Id id) {
		static float x = 0.0f;
		static float y = 0.0f;
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputFloat("X", &x);
			ImGui::InputFloat("Y", &y);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Position2D>(id)) {
				world->CreateComponent<Position2D>(id, x, y);
			}
		}
	}


}
