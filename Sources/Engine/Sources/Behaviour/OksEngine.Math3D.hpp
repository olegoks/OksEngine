#pragma once 

#include <Math.hpp>

namespace OksEngine {

	class LuaVector {
	public:

		LuaVector(float x, float y, float z) : vector_{ x, y, z } {}

		LuaVector(const glm::vec3& vector) : vector_{ vector } {}

		void Normalize() {
			glm::normalize(vector_);
		}

		float GetX() {
			return vector_.x;
		}

		float GetY() {
			return vector_.y;
		}

		float GetZ() {
			return vector_.z;
		}

		glm::vec3 vector_;

	};

	class Math3D {
	public:

		luabridge::LuaRef RotateVector(
			lua_State* state,
			luabridge::LuaRef toRotateVector,
			luabridge::LuaRef aroundVector,
			float angle
		) {

			auto toRotateLuaVector = *toRotateVector.cast<LuaVector>();
			auto aroundLuaVector = *aroundVector.cast<LuaVector>();

			const glm::vec3 rotatedVector = glm::rotate(toRotateLuaVector.vector_, angle, aroundLuaVector.vector_);

			luabridge::
			return luabridge::LuaRef{ LuaVector{ rotatedVector } };

		}

	};



}