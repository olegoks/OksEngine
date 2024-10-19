
#include <Render/Camera/OksEngine.UpdateCameraTransformUniformBuffer.hpp>

#include <Common/OksEngine.Position.hpp>
#include <Common/OksEngine.Direction.hpp>
#include <Render/Camera/OksEngine.Camera.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Render/Driver/OksEngine.UniformBuffer.hpp>
#include <Render/Camera/OksEngine.CameraTransformUniformBufferType.hpp>

namespace OksEngine {

	UpdateCameraTransformUniformBuffer::UpdateCameraTransformUniformBuffer(Context& context) noexcept :
		ECSSystem{ context } { }

	void UpdateCameraTransformUniformBuffer::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* camera = world->GetComponent<Camera>(entityId);
		auto* direction = world->GetComponent<Direction>(entityId);
		auto* uniformBuffer = world->GetComponent<UniformBuffer>(entityId);

		auto* position = world->GetComponent<Position>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		const glm::mat4 view = glm::lookAt(
			position->xyz_,
			position->xyz_ + direction->xyz_,
			camera->GetUp()
		);

		glm::mat4 proj = glm::perspective(
			glm::radians(45.0f),
			camera->width_ / (float)camera->height_,
			camera->zNear_, camera->zFar_);

		proj[1][1] *= -1;

		ViewProjection viewProj{
			.view_ = view,
			.proj_ = proj
		};

		driver->FillUniformBuffer(uniformBuffer->id_, &viewProj);
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateCameraTransformUniformBuffer::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Camera>().Include<Position>().Include<UniformBuffer>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId UpdateCameraTransformUniformBuffer::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateCameraTransformUniformBuffer>().GetId();
	}

}
