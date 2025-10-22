#pragma once
#include <Common\auto_OksEngine.Debug.Position3D.hpp>

namespace OksEngine
{
	void UpdatePosition3DDebugText2D::Update(
		ECS2::Entity::Id entity0id,
		const Debug::WorldPosition3D* debug__WorldPosition3D0,

		ECS2::Entity::Id entity1id, 
		const Camera* camera1, 
		const ZFar* zFar1, 
		const ZNear* zNear1,
		const Direction3D* direction3D1, 
		const WorldPosition3D* worldPosition3D1,
		const WorldRotation3D* worldRotation3D1, 
		const Width* width1, 
		const Height* height1,
		const Up3D* up3D1,

		ECS2::Entity::Id entity2id, 
		const WorldPosition3D* worldPosition3D2, 
		DebugText2D* debugText2D2) {

		//debugText2D0->text_ = 
		const glm::mat4 view = glm::lookAt(
			glm::vec3(worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_),
			glm::vec3(worldPosition3D1->x_, worldPosition3D1->y_, worldPosition3D1->z_) + glm::vec3(direction3D1->x_, direction3D1->y_, direction3D1->z_),
			glm::vec3(up3D1->x_, up3D1->y_, up3D1->z_)
		);

		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			width1->value_ / (float)height1->value_,
			zNear1->value_, zFar1->value_);

		//projection[1][1] *= -1;

		const glm::vec4 clipPos = projection * view * 
			glm::vec4(
				worldPosition3D2->x_,
				worldPosition3D2->y_,
				worldPosition3D2->z_,
				1.0f
			);

		glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;

		float screenX = (ndc.x + 1.0f) * 0.5f * width1->value_;
		float screenY = (1.0f - ndc.y) * 0.5f * height1->value_; // ����������� Y

		const std::string positionText = std::format("({:.2f}, {:.2f}, {:.2f})", worldPosition3D2->x_, worldPosition3D2->y_, worldPosition3D2->z_);
		debugText2D2->text_ = positionText;
		debugText2D2->x_ = screenX;
		debugText2D2->y_ = screenY;

	};

	void CreatePosition3DDebugText2D::Update(
		ECS2::Entity::Id entity0id, const Debug::WorldPosition3D* debug__WorldPosition3D0,
		ECS2::Entity::Id entity1id, const WorldPosition3D* worldPosition3D1) {

		CreateComponent<DebugText2D>(entity0id, "DebugText2D", 0, 0);
	};

} // namespace OksEngine