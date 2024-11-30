
#include <Behaviour/OksEngine.Math3D.hpp>

namespace OksEngine::Lua {

	luabridge::LuaRef Math3D::RotateVector(luabridge::LuaRef toRotateVector, luabridge::LuaRef aroundVector, float angle) const {

		glm::vec3 toRotateVectorGlm{
			toRotateVector["GetX"](toRotateVector)[0].cast<float>().value(),
			toRotateVector["GetY"](toRotateVector)[0].cast<float>().value(),
			toRotateVector["GetZ"](toRotateVector)[0].cast<float>().value() };
		glm::vec3 aroundVectorGlm{
			aroundVector["GetX"](aroundVector)[0].cast<float>().value(),
			aroundVector["GetY"](aroundVector)[0].cast<float>().value(),
			aroundVector["GetZ"](aroundVector)[0].cast<float>().value() };

		const glm::vec3 rotatedVector = glm::rotate(toRotateVectorGlm, angle, aroundVectorGlm);

		Vector luaVector{ rotatedVector };
		luabridge::LuaRef luaVectorRef{ toRotateVector.state(), luaVector };
		return luaVectorRef;
	}
	
	[[nodiscard]]
	luabridge::LuaRef Math3D::CrossProduct(luabridge::LuaRef firstVector, luabridge::LuaRef secondVector) const {

		glm::vec3 vector1{
			firstVector["GetX"](firstVector)[0].cast<float>().value(),
			firstVector["GetY"](firstVector)[0].cast<float>().value(),
			firstVector["GetZ"](firstVector)[0].cast<float>().value() };
		glm::vec3 vector2{
			secondVector["GetX"](secondVector)[0].cast<float>().value(),
			secondVector["GetY"](secondVector)[0].cast<float>().value(),
			secondVector["GetZ"](secondVector)[0].cast<float>().value() };
		const glm::vec3 perpendicular = glm::normalize(glm::cross(vector1, vector2));

		return luabridge::LuaRef(firstVector.state(), Vector{ perpendicular });

	}


}

