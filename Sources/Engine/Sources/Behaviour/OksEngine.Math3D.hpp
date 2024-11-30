#pragma once 

#include <Math.hpp>

#include <Behaviour/OksEngine.LuaContext.hpp>

namespace OksEngine {

	namespace Lua {

		class Vector {
		public:

			Vector(float x, float y, float z) : xyz_{ x, y, z } {}

			Vector(const glm::vec3& xyz) : xyz_{ xyz } {}

			void Normalize() {
				glm::normalize(xyz_);
			}

			[[nodiscard]]
			float GetX() {
				return xyz_.x;
			}

			[[nodiscard]]
			float GetY() {
				return xyz_.y;
			}

			[[nodiscard]]
			float GetZ() {
				return xyz_.z;
			}

			glm::vec3 xyz_;

		};

	}

	template<>
	inline void Bind<Lua::Vector>(::Lua::Context& context) {

		context.GetGlobalNamespace()
			.beginClass<Lua::Vector>("Vector")
			.addConstructor<void(*)(float x, float y, float z)>()
			.addFunction("Normalize", &Lua::Vector::Normalize)
			.addFunction("GetX", &Lua::Vector::GetX)
			.addFunction("GetY", &Lua::Vector::GetY)
			.addFunction("GetZ", &Lua::Vector::GetZ)

			.endClass();
	}

	namespace Lua {

		class Math3D {
		public:

			Math3D() = default;

			luabridge::LuaRef RotateVector(
				luabridge::LuaRef toRotateVector,
				luabridge::LuaRef aroundVector,
				float angle) const;

			[[nodiscard]]
			luabridge::LuaRef CrossProduct(luabridge::LuaRef firstVector, luabridge::LuaRef secondVector) const;


		};

	}

	template<>
	inline void Bind<Lua::Math3D>(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Lua::Math3D>("Math3D")
			.addConstructor<void(*)()>()
			.addFunction("RotateVector", &Lua::Math3D::RotateVector)
			.addFunction("CrossProduct", &Lua::Math3D::CrossProduct)
			.endClass();
	}



}