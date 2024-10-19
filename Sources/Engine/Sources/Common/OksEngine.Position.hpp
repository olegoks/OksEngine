#pragma once

#include <ECS.hpp>
#include <imgui.h>
#include <Math.Vector.hpp>

#include <OksEngine.ECS.Component.hpp>
#include <Behaviour/OksEngine.Math3D.hpp>
namespace OksEngine {

	struct Position : public ECSComponent<Position> {
		glm::vec3 xyz_{ 0.f, 0.f, 0.f };
	/*	glm::quat rotate_{ };*/
	
		Position() = default; 
		Position(float x, float y, float z) :
			ECSComponent{ nullptr },
			xyz_{ x, y, z }/*,
			rotate_{  }*/ {}
		Position(const glm::vec3& translate/*, const glm::quat& rotate*/) :
			ECSComponent{ nullptr },
			xyz_{ translate }/*,
			rotate_{ rotate }*/ {}

		[[nodiscard]]
		float GetX() { return xyz_.x; }
		[[nodiscard]]
		float GetY() { return xyz_.y; }
		[[nodiscard]]
		float GetZ() { return xyz_.z; }

		[[nodiscard]]
		const glm::vec3& GetVec() const noexcept { return xyz_; }

		void Set(luabridge::LuaRef luaVectorRef) {
			const auto& luaVector = (luaVectorRef.cast<Lua::Vector>()).value();
			xyz_ = luaVector.xyz_;
		}

		//[[nodiscard]]
		//luabridge::LuaRef Get(lua_State* state) {
		//	return luabridge::LuaRef(state, Lua::Vector{ xyz_ });
		//}

		//[[nodiscard]]
		//const glm::quat& GetRotateQuat() const noexcept { return rotate_; }

		[[nodiscard]]
		const glm::mat4 GetMat() const noexcept { return glm::translate(xyz_); }

		//[[nodiscard]]
		//const glm::mat4 GetRotateMat() const noexcept { return glm::toMat4(rotate_); }

		//[[nodiscard]]
		//const glm::mat4 GetMat() const noexcept { return glm::mat4{ 1 } * GetRotateMat() * GetTranslateMat(); }


		void SetX(float x) { xyz_.x = x; }
		void SetY(float y) { xyz_.y = y; }
		void SetZ(float z) { xyz_.z = z; }
	};

	template<>
	inline void Edit<Position>(Position* position) {
		ImGui::InputFloat("X", &position->xyz_.x);
		ImGui::InputFloat("Y", &position->xyz_.y);
		ImGui::InputFloat("Z", &position->xyz_.z);
	}

	template<>
	inline void Bind<Position>(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Position>("Position")
			.addConstructor<void(*)(int x, int y, int z)>()
			.addFunction("GetX", &Position::GetX)
			.addFunction("GetY", &Position::GetY)
			.addFunction("GetZ", &Position::GetZ)
			.addFunction("SetX", &Position::SetX)
			.addFunction("SetY", &Position::SetY)
			.addFunction("SetZ", &Position::SetZ)
			//.addFunction("Get", &Position::Get)
			//.addFunction("Set", &Position::Set)
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
