
#include <Render/Model/Transform/OksEngine.CreateDriverTransform3D.hpp>
#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverTransform3D::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		Position* position = world->GetComponent<Position>(entityId);
		Rotation* rotation = world->GetComponent<Rotation>(entityId);

		const glm::mat4 transformMatrix = glm::mat4{ 1 } * rotation->GetMat() * position->GetMat();

		DriverTransform3D::Transform transform{ transformMatrix };

		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(DriverTransform3D::Transform),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);
		driver->FillUniformBuffer(ubId, &transform);

		world->CreateComponent<DriverTransform3D>(entityId, ubId);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverTransform3D::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Position>()
			.Include<Rotation>()
			.Exclude<DriverTransform3D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId CreateDriverTransform3D::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverTransform3D>().GetId();
	}
}