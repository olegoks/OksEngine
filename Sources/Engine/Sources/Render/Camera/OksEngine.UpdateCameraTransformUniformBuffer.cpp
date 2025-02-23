
#include <Render/Camera/auto_OksEngine.UpdateCameraTransformUniformBuffer.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

#include <Render/Camera/OksEngine.CameraTransformUniformBufferType.hpp>

namespace OksEngine {


	void UpdateCameraTransformUniformBuffer::Update(
		ECS2::Entity::Id entity1Id,
		const Camera* camera,
		const Position3D* position3D,
		const Direction3D* direction3D,
		const Up3D* up3D,
		const ZNear* zNear,
		const ZFar* zFar,
		const Width* width,
		const Height* height,
		const UniformBuffer* uniformBuffer,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		const glm::mat4 view = glm::lookAt(
			glm::vec3(position3D->x_, position3D->y_, position3D->z_),
			glm::vec3(position3D->x_, position3D->y_, position3D->z_) + glm::vec3(direction3D->x_, direction3D->y_, direction3D->z_),
			glm::vec3(up3D->x_, up3D->y_, up3D->z_)
		);

		glm::mat4 proj = glm::perspective(
			glm::radians(45.0f),
			width->value_ / (float)height->value_,
			zNear->value_, zFar->value_);

		proj[1][1] *= -1;

		ViewProjection viewProj{
			.view_ = view,
			.proj_ = proj
		};

		renderDriver->driver_->FillUniformBuffer(uniformBuffer->id_, &viewProj);
	}

}
