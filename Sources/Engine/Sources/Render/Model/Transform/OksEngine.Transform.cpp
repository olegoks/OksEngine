#pragma once
#include <Render\Model\Transform\auto_OksEngine.Transform.hpp>

namespace OksEngine
{
	void UpdateDriverTransform2D::Update(ECS2::Entity::Id entity0id, const Position2D* position2D0,
		const Rotation2D* rotation2D0, const Scale2D* scale2D0,
		const DriverTransform2D* driverTransform2D0, ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {



		const glm::mat3 translateMatrix = glm::translate(glm::mat3(1.f), glm::vec2(position2D0->x_, position2D0->y_));
		const glm::mat3 scaleMatrix = glm::scale(glm::mat3(1.0f), glm::vec2(scale2D0->x_, scale2D0->y_));
		const glm::mat3 rotationMatrix = glm::mat3_cast(glm::quat(rotation2D0->w_, rotation2D0->x_, rotation2D0->w_, rotation2D0->z_));

		glm::mat3 transform = glm::mat3{ 1 } *scaleMatrix * rotationMatrix * translateMatrix;

		renderDriver1->driver_->FillUniformBuffer(driverTransform2D0->id_, &transform);



	};

	void UpdateDriverTransform3D::Update(ECS2::Entity::Id entity0id, const Position3D* position3D0,
		const Rotation3D* rotation3D0, DriverTransform3D* driverTransform3D0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {



		const glm::mat4 translateMatrix = glm::mat4{ glm::translate(glm::vec3(position3D0->x_, position3D0->y_, position3D0->z_)) };
		const glm::mat4 rotationMatrix = glm::toMat4(glm::quat{ rotation3D0->w_, rotation3D0->x_, rotation3D0->y_, rotation3D0->z_ });;


		glm::mat4 transform = glm::mat4{ 1 } *rotationMatrix * translateMatrix;


		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(glm::mat4),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};

		renderDriver1->driver_->FillUniformBuffer(driverTransform3D0->id_, &transform);


	};

	void CreateDriverTransform3D::Update(ECS2::Entity::Id entity0id, const Position3D* position3D0,
		const Rotation3D* rotation3D0, ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {


		const glm::mat4 nodeTranslateMatrix = glm::mat4{ glm::translate(glm::vec3(position3D0->x_, position3D0->y_, position3D0->z_)) };
		const glm::mat4 nodeRotationMatrix = glm::toMat4(glm::quat{ rotation3D0->w_, rotation3D0->x_, rotation3D0->y_, rotation3D0->z_ });;


		glm::mat4 transformMatrix = glm::mat4{ 1 } *nodeRotationMatrix * nodeTranslateMatrix;

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(glm::mat4),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id ubId = renderDriver1->driver_->CreateUniformBuffer(UBCreateInfo);
		renderDriver1->driver_->FillUniformBuffer(ubId, &transformMatrix);

		CreateComponent<DriverTransform3D>(entity0id, ubId);


	};

	void CreateTransform2DResource::Update(ECS2::Entity::Id entity0id, const DriverTransform2D* driverTransform2D0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {


		auto driver = renderDriver1->driver_;

		const RAL::Driver::Resource::CI2 rci{
			.stage_ = RAL::Driver::Shader::Stage::VertexShader,
			.binding_ = 0,
			.ubid_ = driverTransform2D0->id_,
			.offset_ = 0,
			.size_ = driver->GetUBSizeInBytes(driverTransform2D0->id_)
		};

		const RAL::Driver::Resource::Id rid = driver->CreateResource(rci);

		CreateComponent<Transform2DResource>(entity0id, rid);


	};

	void CreateDriverTransform2D::Update(ECS2::Entity::Id entity0id, const Position2D* position2D0, const Scale2D* scale2D0,
		const Rotation2D* rotation2D0, ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {


		const glm::mat3 translateMatrix = glm::translate(glm::mat3(1.f), glm::vec2(position2D0->x_, position2D0->y_));
		const glm::mat3 scaleMatrix = glm::scale(glm::mat3(1.0f), glm::vec2(scale2D0->x_, scale2D0->y_));
		const glm::mat3 rotationMatrix = glm::mat3_cast(glm::quat(rotation2D0->w_, rotation2D0->x_, rotation2D0->y_, rotation2D0->z_));


		glm::mat3 transformMatrix = glm::mat3{ 1 } *scaleMatrix * rotationMatrix * translateMatrix;

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(glm::mat3),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id ubId = renderDriver1->driver_->CreateUniformBuffer(UBCreateInfo);
		renderDriver1->driver_->FillUniformBuffer(ubId, &transformMatrix);

		CreateComponent<DriverTransform2D>(entity0id, ubId);


	};

} // namespace OksEngine