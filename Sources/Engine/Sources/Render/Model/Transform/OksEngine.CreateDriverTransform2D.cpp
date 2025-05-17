#include <Render/Model/Transform\auto_OksEngine.CreateDriverTransform2D.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

namespace OksEngine {
void CreateDriverTransform2D::Update(
	ECS2::Entity::Id entity1Id, const Position2D* position2D,
	const Scale2D* scale2D, const Rotation2D* rotation2D,
	ECS2::Entity::Id entity2Id, RenderDriver* renderDriver) {

	const glm::mat3 translateMatrix = glm::translate(glm::mat3(1.f), glm::vec2(position2D->x_, position2D->y_));
	const glm::mat3 scaleMatrix = glm::scale(glm::mat3(1.0f), glm::vec2(scale2D->x_, scale2D->y_));
	const glm::mat3 rotationMatrix = glm::mat3_cast(glm::quat( rotation2D->w_, rotation2D->x_, rotation2D->y_, rotation2D->z_));


	glm::mat3 transformMatrix = glm::mat3{ 1 } * scaleMatrix * rotationMatrix * translateMatrix;

	RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
		.size_ = sizeof(glm::mat3),
		.type_ = RAL::Driver::UniformBuffer::Type::Mutable
	};
	RAL::Driver::UniformBuffer::Id ubId = renderDriver->driver_->CreateUniformBuffer(UBCreateInfo);
	renderDriver->driver_->FillUniformBuffer(ubId, &transformMatrix);

	CreateComponent<DriverTransform2D>(entity1Id, ubId);

};
}