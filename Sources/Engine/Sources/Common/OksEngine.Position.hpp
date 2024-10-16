#pragma once

#include <ECS.hpp>
#include <imgui.h>
#include <Math.Vector.hpp>

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Position : public ECSComponent<Position> {
		glm::vec3 translate_{ 0.f, 0.f, 0.f };
	/*	glm::quat rotate_{ };*/
	
		Position() = default; 
		Position(float x, float y, float z) :
			ECSComponent{ nullptr },
			translate_{ x, y, z }/*,
			rotate_{  }*/ {}
		Position(const glm::vec3& translate/*, const glm::quat& rotate*/) :
			ECSComponent{ nullptr },
			translate_{ translate }/*,
			rotate_{ rotate }*/ {}

		[[nodiscard]]
		float GetX() { return translate_.x; }
		[[nodiscard]]
		float GetY() { return translate_.y; }
		[[nodiscard]]
		float GetZ() { return translate_.z; }

		[[nodiscard]]
		const glm::vec3& GetVec() const noexcept { return translate_; }

		//[[nodiscard]]
		//const glm::quat& GetRotateQuat() const noexcept { return rotate_; }

		[[nodiscard]]
		const glm::mat4 GetMat() const noexcept { return glm::translate(translate_); }

		//[[nodiscard]]
		//const glm::mat4 GetRotateMat() const noexcept { return glm::toMat4(rotate_); }

		//[[nodiscard]]
		//const glm::mat4 GetMat() const noexcept { return glm::mat4{ 1 } * GetRotateMat() * GetTranslateMat(); }


		void SetX(float x) { translate_.x = x; }
		void SetY(float y) { translate_.y = y; }
		void SetZ(float z) { translate_.z = z; }
	};

	template<>
	inline void Edit<Position>(Position* position) {
		ImGui::InputFloat("X", &position->translate_.x);
		ImGui::InputFloat("Y", &position->translate_.y);
		ImGui::InputFloat("Z", &position->translate_.z);
	}

	template<>
	inline void Bind<Position>(Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Position>("Position")
			.addConstructor<void(*)(int x, int y, int z)>()
			.addFunction("GetX", &Position::GetX)
			.addFunction("GetY", &Position::GetY)
			.addFunction("GetZ", &Position::GetZ)
			.addFunction("SetX", &Position::SetX)
			.addFunction("SetY", &Position::SetY)
			.addFunction("SetZ", &Position::SetZ)
			.endClass();
	}


	template<>
	inline void Add<Position>(ECS::World* world, ECS::Entity::Id id) {
		static float x = 0.0f;
		static float y = 0.0f;
		static float z = 0.0f;
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputFloat("X", &x);
			ImGui::InputFloat("Y", &y);
			ImGui::InputFloat("Z", &z);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Position>(id)) {
				world->CreateComponent<Position>(id, x, y, z);
			}
		}
	}


}
