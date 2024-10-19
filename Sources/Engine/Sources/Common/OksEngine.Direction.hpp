#pragma once

#include <OksEngine.ECS.Component.hpp>
//#include <Behaviour/OksEngine.Math3D.hpp>
namespace OksEngine {

	class Direction : public ECSComponent<Direction> {
	public:

		Direction() : ECSComponent{ nullptr } {}

		Direction(float x, float y, float z) : ECSComponent{ nullptr }, xyz_{ x, y, z } {}

		[[nodiscard]]
		float GetX() { return xyz_.x; }
		[[nodiscard]]
		float GetY() { return xyz_.y; }
		[[nodiscard]]
		float GetZ() { return xyz_.z; }


		void SetX(float x) { xyz_.x = x; }
		void SetY(float y) { xyz_.y = y; }
		void SetZ(float z) { xyz_.z = z; }

		void Set(luabridge::LuaRef luaVectorRef) {
			const auto& luaVector = (luaVectorRef.cast<Lua::Vector>()).value();
			xyz_ = luaVector.xyz_;
		}
		glm::vec3 xyz_{ 1.0, 0.0, 0 };
	};


	template<>
	inline void Edit<Direction>(Direction* direction) {
		ImGui::InputFloat("X", &direction->xyz_.x);
		ImGui::InputFloat("Y", &direction->xyz_.y);
		ImGui::InputFloat("Z", &direction->xyz_.z);
	}

	template<>
	inline void Bind<Direction>(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Direction>("Direction")
			.addConstructor<void(*)(int x, int y, int z)>()
			.addFunction("GetX", &Direction::GetX)
			.addFunction("GetY", &Direction::GetY)
			.addFunction("GetZ", &Direction::GetZ)
			.addFunction("SetX", &Direction::SetX)
			.addFunction("SetY", &Direction::SetY)
			.addFunction("SetZ", &Direction::SetZ)
			.addFunction("Set", &Direction::Set)
			.endClass();
	}

}