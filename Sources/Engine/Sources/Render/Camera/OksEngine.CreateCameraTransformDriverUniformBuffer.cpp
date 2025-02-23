
#include <Render/Camera/auto_OksEngine.CreateCameraTransformDriverUniformBuffer.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Render/Camera/OksEngine.CameraTransformUniformBufferType.hpp>

namespace OksEngine {

	void CreateCameraTransformDriverUniformBuffer::Update(
		ECS2::Entity::Id entity1Id,
		const Camera* camera,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(ViewProjection),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id UBId = renderDriver->driver_->CreateUniformBuffer(UBCreateInfo);

		CreateComponent<DriverViewProjectionUniformBuffer>(entity1Id, UBId);

	}

}