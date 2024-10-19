#pragma once 

#include <Math.hpp>

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

			luabridge::LuaRef RotateVector(
				luabridge::LuaRef toRotateVector,
				luabridge::LuaRef aroundVector,
				float angle) const {

				const auto toRotateLuaVector = *toRotateVector.cast<Vector>();
				const auto aroundLuaVector = *aroundVector.cast<Vector>();

				const glm::vec3 rotatedVector = glm::rotate(toRotateLuaVector.xyz_, angle, aroundLuaVector.xyz_);

				Vector luaVector{ rotatedVector };
				luabridge::LuaRef luaVectorRef{ toRotateVector.state(), luaVector };
				return luaVectorRef;
			}

			[[nodiscard]]
			luabridge::LuaRef CrossProduct(luabridge::LuaRef firstVector, luabridge::LuaRef secondVector) const {

				const auto firstLuaVector = *firstVector.cast<Vector>();
				const auto secondLuaVector = *firstVector.cast<Vector>();

				const glm::vec3 perpendicular = glm::normalize(glm::cross(firstLuaVector.xyz_, secondLuaVector.xyz_));

				return luabridge::LuaRef(firstVector.state(), Vector{ perpendicular });

			}


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