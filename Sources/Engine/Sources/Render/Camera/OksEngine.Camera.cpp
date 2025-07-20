#pragma once
#include <Render\Camera\auto_OksEngine.Camera.hpp>

#include <glm/glm.hpp>


namespace OksEngine
{


	struct ViewProjection {
		alignas(16) glm::mat4 view_;
		alignas(16) glm::mat4 proj_;
	};

	void UpdateCameraTransformUniformBuffer::Update(ECS2::Entity::Id entity0id,
		const Camera* camera0,
		const Position3D* position3D0, 
		const Direction3D* direction3D0,
		const Up3D* up3D0, 
		const ZNear* zNear0, const ZFar* zFar0,
		const Width* width0, const Height* height0,
		DriverViewProjectionUniformBuffer* uniformBuffer0,


		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {


		const glm::mat4 view = glm::lookAt(
			glm::vec3(position3D0->x_, position3D0->y_, position3D0->z_),
			glm::vec3(position3D0->x_, position3D0->y_, position3D0->z_) + glm::vec3(direction3D0->x_, direction3D0->y_, direction3D0->z_),
			glm::vec3(up3D0->x_, up3D0->y_, up3D0->z_)
		);

		glm::mat4 proj = glm::perspective(
			glm::radians(45.0f),
			width0->value_ / (float)height0->value_,
			zNear0->value_, zFar0->value_);

		proj[1][1] *= -1;

		ViewProjection viewProj{
			.view_ = view,
			.proj_ = proj
		};

		renderDriver1->driver_->FillUniformBuffer(uniformBuffer0->id_, &viewProj);

	};

	void CreateCameraTransformDriverUniformBuffer::Update(
		ECS2::Entity::Id entity0id, 
		const Camera* camera0,

		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {


		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(ViewProjection),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id UBId = renderDriver1->driver_->CreateUniformBuffer(UBCreateInfo);

		CreateComponent<DriverViewProjectionUniformBuffer>(entity0id, UBId);


	};

	void CreateCameraTransformResource::Update(
		ECS2::Entity::Id entity0id,
		RenderDriver* renderDriver0,
		
		ECS2::Entity::Id entity1id,
		const Camera* camera1, 
		const DriverViewProjectionUniformBuffer* driverViewProjectionUniformBuffer1) {

		RAL::Driver::ResourceSet::Binding transformUBBinding
		{
			.stage_ = RAL::Driver::Shader::Stage::VertexShader,
			.binding_ = 0,
			.ubid_ = driverViewProjectionUniformBuffer1->id_,
			.offset_ = 0,
			.size_ = sizeof(ViewProjection)
		};

		RAL::Driver::Resource::Id resourceId = renderDriver0->driver_->CreateResource(transformUBBinding);

		CreateComponent<CameraTransformResource>(entity1id, resourceId);

	}


} // namespace OksEngine