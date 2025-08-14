#pragma once
#include <Common\auto_OksEngine.Debug.Position3D.hpp>

namespace OksEngine
{
	void UpdatePosition3DDebugText2D::Update(
		ECS2::Entity::Id entity0id,
		const Camera* camera0,
		const ZFar* zFar0,
		const ZNear* zNear0,
		const Direction3D* direction3D0,
		const WorldPosition3D* worldPosition3D0,
		const WorldRotation3D* worldRotation3D0,
		const Width* width0,
		const Height* height0,
		const Up3D* up3D0,

		ECS2::Entity::Id entity1id,
		const WorldPosition3D* worldPosition3D1, 
		DebugText2D* debugText2D1) {

		//debugText2D0->text_ = 
		const glm::mat4 view = glm::lookAt(
			glm::vec3(worldPosition3D0->x_, worldPosition3D0->y_, worldPosition3D0->z_),
			glm::vec3(worldPosition3D0->x_, worldPosition3D0->y_, worldPosition3D0->z_) + glm::vec3(direction3D0->x_, direction3D0->y_, direction3D0->z_),
			glm::vec3(up3D0->x_, up3D0->y_, up3D0->z_)
		);

		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			width0->value_ / (float)height0->value_,
			zNear0->value_, zFar0->value_);

		//projection[1][1] *= -1;

		const glm::vec4 clipPos = projection * view * 
			glm::vec4(
				worldPosition3D1->x_,
				worldPosition3D1->y_,
				worldPosition3D1->z_,
				1.0f
			);

		glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

		float screenX = (ndc.x + 1.0f) * 0.5f * width0->value_;
		float screenY = (1.0f - ndc.y) * 0.5f * height0->value_; // Инвертируем Y

		const std::string positionText = std::format("({:.2f}, {:.2f}, {:.2f})", worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_);
		debugText2D1->text_ = positionText;
		debugText2D1->x_ = screenX;
		debugText2D1->y_ = screenY;
	};

	void CreatePosition3DDebugText2D::Update(
		ECS2::Entity::Id entity0id, 
		const WorldPosition3D* position3D0) {
	
		CreateComponent<DebugText2D>(entity0id, "DebugText2D", 0, 0);
	};

} // namespace OksEngine