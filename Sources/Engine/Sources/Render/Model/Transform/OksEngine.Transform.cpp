#pragma once
#include <Render\Model\Transform\auto_OksEngine.Transform.hpp>

#include <Common\auto_OksEngine.Name.hpp>

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
	struct RTS {
		alignas(16) float rotation[4];
		alignas(16) float transform[3]; // + 4 padding
		alignas(16) float scale[3];		// + 4 padding
	};
	void UpdateDriverTransform3D::Update(ECS2::Entity::Id entity0id, const WorldPosition3D* position3D0,
		const WorldRotation3D* rotation3D0, const WorldScale3D* scale3D0, DriverTransform3D* driverTransform3D0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {


			return;
		//const glm::mat4 translateMatrix = glm::mat4{ glm::translate(glm::vec3(position3D0->x_, position3D0->y_, position3D0->z_)) };
		//const glm::mat4 rotationMatrix = glm::toMat4(glm::quat{ rotation3D0->w_, rotation3D0->x_, rotation3D0->y_, rotation3D0->z_ });;
		//const glm::mat4 scaleMatrix = glm::scale(glm::vec3(scale3D0->x_, scale3D0->y_, scale3D0->z_));

		//glm::mat4 transform = translateMatrix * rotationMatrix * scaleMatrix;

		//float x = rotation3D0->x_;
		//float y = rotation3D0->y_;
		//float z = rotation3D0->z_;
		//float w = rotation3D0->w_;

		//float x2 = x * x;
		//float y2 = y * y;
		//float z2 = z * z;
		//float xy = x * y;
		//float xz = x * z;
		//float yz = y * z;
		//float wx = w * x;
		//float wy = w * y;
		//float wz = w * z;

		//auto matrix = glm::mat4(
		//	scale3D0->x_ * (1.0 - 2.0 * (y2 + z2)),
		//	scale3D0->x_ * (2.0 * (xy + wz)),
		//	scale3D0->x_ * (2.0 * (xz - wy)),
		//	0.0,

		//	scale3D0->y_ * (2.0 * (xy - wz)),
		//	scale3D0->y_ * (1.0 - 2.0 * (x2 + z2)),
		//	scale3D0->y_ * (2.0 * (yz + wx)),
		//	0.0,

		//	scale3D0->z_ * (2.0 * (xz + wy)),
		//	scale3D0->z_ * (2.0 * (yz - wx)),
		//	scale3D0->z_ * (1.0 - 2.0 * (x2 + y2)),
		//	0.0,

		//	position3D0->x_,
		//	position3D0->y_,
		//	position3D0->z_,
		//	1.0
		//);
		
		RTS rts{
			{ rotation3D0->w_, rotation3D0->x_, rotation3D0->y_, rotation3D0->z_ },
			{ position3D0->x_, position3D0->y_, position3D0->z_ },
			{ scale3D0->x_, scale3D0->y_, scale3D0->z_ }
		};

		//if(IsComponentExist<Name>(entity0id)) {
		//	if (GetComponent<Name>(entity0id)->value_ == "robot_base.025_low_53") {
		//		Common::BreakPointLine();
		//	}
		//	
		//}

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(RTS),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};

		renderDriver1->driver_->FillUniformBuffer(driverTransform3D0->id_, &rts);

	/*	RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(glm::mat4),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};

		renderDriver1->driver_->FillUniformBuffer(driverTransform3D0->id_, &matrix);*/

	};

	void CreateDriverTransform3D::Update(ECS2::Entity::Id entity0id, const WorldPosition3D* position3D0,
		const WorldRotation3D* rotation3D0, const WorldScale3D* scale3D0, ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {

		//const glm::mat4 nodeTranslateMatrix = glm::mat4{ glm::translate(glm::vec3(position3D0->x_, position3D0->y_, position3D0->z_)) };
		//const glm::mat4 nodeRotationMatrix = glm::toMat4(glm::quat{ rotation3D0->w_, rotation3D0->x_, rotation3D0->y_, rotation3D0->z_ });
		//const glm::mat4 nodeScaleMatrix = glm::scale(glm::vec3(scale3D0->x_, scale3D0->y_, scale3D0->z_));

		//glm::mat4 transformMatrix = glm::mat4{ 1 } *nodeTranslateMatrix * nodeRotationMatrix * nodeScaleMatrix;


		//float x = rotation3D0->x_;
		//float y = rotation3D0->y_;
		//float z = rotation3D0->z_;
		//float w = rotation3D0->w_;

		//float x2 = x * x;
		//float y2 = y * y;
		//float z2 = z * z;
		//float xy = x * y;
		//float xz = x * z;
		//float yz = y * z;
		//float wx = w * x;
		//float wy = w * y;
		//float wz = w * z;

		//auto matrix = glm::mat4(
		//	scale3D0->x_ * (1.0 - 2.0 * (y2 + z2)),
		//	scale3D0->x_ * (2.0 * (xy + wz)),
		//	scale3D0->x_ * (2.0 * (xz - wy)),
		//	0.0,

		//	scale3D0->y_ * (2.0 * (xy - wz)),
		//	scale3D0->y_ * (1.0 - 2.0 * (x2 + z2)),
		//	scale3D0->y_ * (2.0 * (yz + wx)),
		//	0.0,

		//	scale3D0->z_ * (2.0 * (xz + wy)),
		//	scale3D0->z_ * (2.0 * (yz - wx)),
		//	scale3D0->z_ * (1.0 - 2.0 * (x2 + y2)),
		//	0.0,

		//	position3D0->x_,
		//	position3D0->y_,
		//	position3D0->z_,
		//	1.0
		//);

		RTS rts{
			{ rotation3D0->w_, rotation3D0->x_, rotation3D0->y_, rotation3D0->z_ },
			{ position3D0->x_, position3D0->y_, position3D0->z_ },
			{ scale3D0->x_, scale3D0->y_, scale3D0->z_ }
		};

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(RTS),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id ubId = renderDriver1->driver_->CreateUniformBuffer(UBCreateInfo);
		renderDriver1->driver_->FillUniformBuffer(ubId, &rts);

		//RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
		//	.size_ = sizeof(glm::mat4),
		//	.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		//};
		//RAL::Driver::UniformBuffer::Id ubId = renderDriver1->driver_->CreateUniformBuffer(UBCreateInfo);
		//renderDriver1->driver_->FillUniformBuffer(ubId, &matrix);

		CreateComponent<DriverTransform3D>(entity0id, ubId);


	};

	void CreateTransform3DResource::Update(
		ECS2::Entity::Id entity0id,
		const DriverTransform3D* driverTransform3D0,

		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {


		auto driver = renderDriver1->driver_;

		const RAL::Driver::Resource::CI2 rci{
			.stage_ = RAL::Driver::Shader::Stage::VertexShader,
			.binding_ = 0,
			.ubid_ = driverTransform3D0->id_,
			.offset_ = 0,
			.size_ = driver->GetUBSizeInBytes(driverTransform3D0->id_)
		};

		const RAL::Driver::Resource::Id rid = driver->CreateResource(rci);

		CreateComponent<Transform3DResource>(entity0id, rid);


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