
#include <Behaviour/OksEngine.Math3D.hpp>

namespace OksEngine::Lua {

	luabridge::LuaRef Math3D::RotateVector(luabridge::LuaRef toRotateVector, luabridge::LuaRef aroundVector, float angle) const {

		glm::vec3 toRotateVectorGlm{
			toRotateVector["x"].cast<float>().value(),
			toRotateVector["y"].cast<float>().value(),
			toRotateVector["z"].cast<float>().value() };
		glm::vec3 aroundVectorGlm{
			aroundVector["x"].cast<float>().value(),
			aroundVector["y"].cast<float>().value(),
			aroundVector["z"].cast<float>().value() };

		const glm::vec3 rotatedVector = glm::rotate(toRotateVectorGlm, angle, aroundVectorGlm);

		Vector luaVector{ rotatedVector.x, rotatedVector.y, rotatedVector.z };
		luabridge::LuaRef luaVectorRef{ toRotateVector.state(), luaVector };
		return luaVectorRef;
	}
	
	[[nodiscard]]
	luabridge::LuaRef Math3D::CrossProduct(luabridge::LuaRef firstVector, luabridge::LuaRef secondVector) const {

		glm::vec3 vector1{
			firstVector["x"].cast<float>().value(),
			firstVector["y"].cast<float>().value(),
			firstVector["z"].cast<float>().value() };
		glm::vec3 vector2{
			secondVector["x"].cast<float>().value(),
			secondVector["y"].cast<float>().value(),
			secondVector["z"].cast<float>().value() };
		const glm::vec3 perpendicular = glm::normalize(glm::cross(vector1, vector2));

		return luabridge::LuaRef(firstVector.state(), Vector{ perpendicular.x, perpendicular.y, perpendicular.z });

	}


}

