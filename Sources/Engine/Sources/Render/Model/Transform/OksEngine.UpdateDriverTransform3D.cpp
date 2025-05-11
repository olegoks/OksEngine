#include <Render\Model\Transform\auto_OksEngine.UpdateDriverTransform3D.hpp>

namespace OksEngine {


	void UpdateNodeTransform(std::shared_ptr<RAL::Driver> driver, ECS2::World* world, ECS2::Entity::Id nodeEntityId, const glm::mat4& parentTransform) {

		////if (world->IsComponentExist<Meshes>(nodeEntityId)) {

		////}
		//Position3D* position = world->GetComponent<Position3D>(nodeEntityId);
		//Rotation3D* rotation = world->GetComponent<Rotation3D>(nodeEntityId);

		//const glm::mat4 nodeTranslateMatrix = glm::mat4{ glm::translate(glm::vec3(position->x_, position->y_, position->z_)) };
		//const glm::mat4 nodeRotationMatrix = glm::toMat4(glm::quat{ rotation->w_, rotation->x_, rotation->y_, rotation->z_ });;

		//const glm::mat4 nodeTransform = parentTransform * nodeTranslateMatrix * nodeRotationMatrix;

		//if (world->IsComponentExist<DriverTransform3D>(nodeEntityId)) {

		//	DriverTransform3D* driverTransform = world->GetComponent<DriverTransform3D>(nodeEntityId);
		//	DriverTransform3D::Transform transform{ nodeTransform };

		//	RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
		//		.size_ = sizeof(DriverTransform3D::Transform),
		//		.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		//	};

		//	driver->FillUniformBuffer(driverTransform->UBId_, &transform);
		//}

		//if (world->IsComponentExist<ChildEntities>(nodeEntityId)) {
		//	auto* childEntities = world->GetComponent<ChildEntities>(nodeEntityId);

		//	for (ECS::Entity::Id childEntity : childEntities->entitiesIds_) {
		//		UpdateNodeTransform(driver, world, childEntity, nodeTransform);
		//	}
		//}
	}

	void UpdateDriverTransform3D::Update(
		ECS2::Entity::Id entity1Id,
		const Position3D* position3D,
		const Rotation3D* rotation3D,
		DriverTransform3D* driverTransform3D,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		//ModelEntity* modelEntity = world->GetComponent<ModelEntity>(entityId);

		const glm::mat4 translateMatrix = glm::mat4{ glm::translate(glm::vec3(position3D->x_, position3D->y_, position3D->z_)) };
		const glm::mat4 rotationMatrix = glm::toMat4(glm::quat{ rotation3D->w_, rotation3D->x_, rotation3D->y_, rotation3D->z_ });;


		glm::mat4 transform = glm::mat4{ 1 } *rotationMatrix * translateMatrix;

		//DriverTransform3D* driverTransform = world->GetComponent<DriverTransform3D>(nodeEntityId);
		//DriverTransform3D::Transform transform{ nodeTransform };

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(glm::mat4),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};

		renderDriver->driver_->FillUniformBuffer(driverTransform3D->id_, &transform);

		//ECS2::Entity::Id rootNodeEntityId = modelEntity->id_;
		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		//UpdateNodeTransform(driver, world, rootNodeEntityId, entityTransform);


	}
}