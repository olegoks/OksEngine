
#include <Render/Model/Transform/OksEngine.UpdateDriverTransform.hpp>
#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void UpdateDriverTransform3D::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

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
			.Include<DriverTransform3D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId UpdateDriverTransform3D::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateDriverTransform3D>().GetId();
	}
}