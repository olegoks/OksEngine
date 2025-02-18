#pragma once 

#include <Math.hpp>

#include <OksEngine.ECS.Component.hpp>
#include <Lua.Context.hpp>

namespace OksEngine {

	namespace Lua {

		class Vector {
		public:

			Vector(float x, float y, float z) : x_{ x }, y_{ y }, z_{ z } {}

			void Normalize() {
				glm::vec3 vec{ x_, y_, z_ };
				glm::normalize(vec);
				x_ = vec.x;
				y_ = vec.y;
				z_ = vec.z;
			}

			float x_;
			float y_;
			float z_;
		};

	}

	template<>
	inline void Bind<Lua::Vector>(::Lua::Context& context) {

		context.GetGlobalNamespace()
			.beginClass<Lua::Vector>("Vector")
			.addConstructor<void(*)(float x, float y, float z)>()
			.addFunction("Normalize", &Lua::Vector::Normalize)
			.addProperty("x", &Lua::Vector::x_)
			.addProperty("y", &Lua::Vector::y_)
			.addProperty("z", &Lua::Vector::z_)
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