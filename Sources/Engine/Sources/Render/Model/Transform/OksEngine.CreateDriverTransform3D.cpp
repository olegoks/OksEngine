
#include <Render/Model/Transform/auto_OksEngine.CreateDriverTransform3D.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverTransform3D::Update(
		ECS2::Entity::Id entity1Id, const Position3D* position3D, const Rotation3D* rotation3D,
		ECS2::Entity::Id entity2Id, RenderDriver* renderDriver) {

		const glm::mat4 nodeTranslateMatrix = glm::mat4{ glm::translate(glm::vec3(position3D->x_, position3D->y_, position3D->z_)) };
		const glm::mat4 nodeRotationMatrix = glm::toMat4(glm::quat{ rotation3D->w_, rotation3D->x_, rotation3D->y_, rotation3D->z_ });;


		glm::mat4 transformMatrix = glm::mat4{ 1 } * nodeRotationMatrix * nodeTranslateMatrix;

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(glm::mat4),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id ubId = renderDriver->driver_->CreateUniformBuffer(UBCreateInfo);
		renderDriver->driver_->FillUniformBuffer(ubId, &transformMatrix);

 		CreateComponent<DriverTransform3D>(entity1Id, ubId);

	}

}