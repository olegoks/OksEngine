#pragma once
#include <Common\auto_OksEngine.Rotation3D.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace OksEngine
{
	void EditWorldRotation3D(std::shared_ptr<ECS2::World> ecsWorld, WorldRotation3D* worldRotation3D) {

		ImGui::PushID(WorldRotation3D::GetTypeId());
		ImGui::InputScalar("w", ImGuiDataType_Float, &worldRotation3D->w_);
		ImGui::InputScalar("x", ImGuiDataType_Float, &worldRotation3D->x_);
		ImGui::InputScalar("y", ImGuiDataType_Float, &worldRotation3D->y_);
		ImGui::InputScalar("z", ImGuiDataType_Float, &worldRotation3D->z_);

		ImGui::PopID();

	};

	WorldRotation3D ParseWorldRotation3D(luabridge::LuaRef& worldRotation3DRef) {

		WorldRotation3D worldRotation3D;

		if (
			!worldRotation3DRef["w"].isNil() &&
			!worldRotation3DRef["x"].isNil() &&
			!worldRotation3DRef["y"].isNil() &&
			!worldRotation3DRef["z"].isNil()) {
			worldRotation3D.w_ = worldRotation3DRef["w"].cast<float>().value();
			worldRotation3D.x_ = worldRotation3DRef["x"].cast<float>().value();
			worldRotation3D.y_ = worldRotation3DRef["y"].cast<float>().value();
			worldRotation3D.z_ = worldRotation3DRef["z"].cast<float>().value();
		}
		else if(!worldRotation3DRef["aroundX"].isNil() &&
				!worldRotation3DRef["aroundY"].isNil() &&
				!worldRotation3DRef["aroundZ"].isNil()) {


			float xRad = glm::radians(worldRotation3DRef["aroundX"].cast<float>().value());
			float yRad = glm::radians(worldRotation3DRef["aroundY"].cast<float>().value());
			float zRad = glm::radians(worldRotation3DRef["aroundZ"].cast<float>().value());

			// Создаем отдельные кватернионы для каждой оси
			glm::quat qx = glm::angleAxis(xRad, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::quat qy = glm::angleAxis(yRad, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat qz = glm::angleAxis(zRad, glm::vec3(0.0f, 0.0f, 1.0f));

			glm::quat result = qz * qy * qx;

			worldRotation3D.w_ = result.w;
			worldRotation3D.x_ = result.x;
			worldRotation3D.y_ = result.y;
			worldRotation3D.z_ = result.z;

		}

		return worldRotation3D;

	};

} // namespace OksEngine