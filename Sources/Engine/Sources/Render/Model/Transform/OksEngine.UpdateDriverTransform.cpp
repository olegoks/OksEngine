
#include <Render/Model/Transform/OksEngine.UpdateDriverTransform.hpp>
#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void UpdateDriverTransform3D::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {


		return;
		Position* position = world->GetComponent<Position>(entityId);
		Rotation* rotation = world->GetComponent<Rotation>(entityId);

		DriverTransform3D* driverTransform = world->GetComponent<DriverTransform3D>(entityId);

		const glm::mat4 transformMatrix = glm::mat4{ 1 } *rotation->GetMat() * position->GetMat();

		DriverTransform3D::Transform transform{ transformMatrix };

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(DriverTransform3D::Transform),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		driver->FillUniformBuffer(driverTransform->UBId_, &transform);
	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateDriverTransform3D::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Position>()
			.Include<Rotation>()
			.Exclude<LocalPosition3D>()
			.Include<DriverTransform3D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId UpdateDriverTransform3D::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateDriverTransform3D>().GetId();
	}



	void UpdateDriverTransform3DWithLocalTransform::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		return;
		Position* position = world->GetComponent<Position>(entityId);
		Rotation* rotation = world->GetComponent<Rotation>(entityId);
		LocalPosition3D* localPosition = world->GetComponent<LocalPosition3D>(entityId);

		DriverTransform3D* driverTransform = world->GetComponent<DriverTransform3D>(entityId);

		const glm::mat4 transformMatrix = glm::mat4{ 1 } *  position->GetMat() * localPosition->GetMat() * rotation->GetMat();

		DriverTransform3D::Transform transform{ transformMatrix };

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(DriverTransform3D::Transform),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		driver->FillUniformBuffer(driverTransform->UBId_, &transform);
	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateDriverTransform3DWithLocalTransform::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Position>()
			.Include<Rotation>()
			.Include<LocalPosition3D>()
			.Include<DriverTransform3D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId UpdateDriverTransform3DWithLocalTransform::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateDriverTransform3DWithLocalTransform>().GetId();
	}


	void UpdateNodeTransform(std::shared_ptr<RAL::Driver> driver, ECS::World* world, ECS::Entity::Id nodeEntityId, const glm::mat4& parentTransform) {

		//if (world->IsComponentExist<Meshes>(nodeEntityId)) {

		//}
		Position* position = world->GetComponent<Position>(nodeEntityId);
		Rotation* rotation = world->GetComponent<Rotation>(nodeEntityId);

		glm::mat4 nodeTransform = parentTransform * position->GetMat() * rotation->GetMat();

		if (world->IsComponentExist<DriverTransform3D>(nodeEntityId)) {

			DriverTransform3D* driverTransform = world->GetComponent<DriverTransform3D>(nodeEntityId);
			DriverTransform3D::Transform transform{ nodeTransform };

			RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
				.size_ = sizeof(DriverTransform3D::Transform),
				.type_ = RAL::Driver::UniformBuffer::Type::Mutable
			};

			driver->FillUniformBuffer(driverTransform->UBId_, &transform);
		}

		if (world->IsComponentExist<ChildEntities>(nodeEntityId)) {
			auto* childEntities = world->GetComponent<ChildEntities>(nodeEntityId);

			for (ECS::Entity::Id childEntity : childEntities->entitiesIds_) {
				UpdateNodeTransform(driver, world, childEntity, nodeTransform);
			}
		}
	}

	void UpdateModelDriverTransform::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		//return;
		Position* position = world->GetComponent<Position>(entityId);
		Rotation* rotation = world->GetComponent<Rotation>(entityId);
		
		ModelEntity* modelEntity = world->GetComponent<ModelEntity>(entityId);

		const glm::mat4 entityTransform = glm::mat4{ 1 } * position->GetMat() * rotation->GetMat();

		ECS::Entity::Id rootNodeEntityId = modelEntity->id_;
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		UpdateNodeTransform(driver, world, rootNodeEntityId, entityTransform);


	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateModelDriverTransform::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Position>()
			.Include<Rotation>()
			.Include<ModelEntity>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId UpdateModelDriverTransform::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateModelDriverTransform>().GetId();
	}


}