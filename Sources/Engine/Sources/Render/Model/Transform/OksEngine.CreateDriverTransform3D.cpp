
#include <Render/Model/Transform/OksEngine.CreateDriverTransform3D.hpp>
#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverTransform3D::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		Position3D* position = world->GetComponent<Position3D>(entityId);
		Rotation3D* rotation = world->GetComponent<Rotation3D>(entityId);

		const glm::mat4 nodeTranslateMatrix = glm::mat4{ glm::translate(glm::vec3(position->x_, position->y_, position->z_)) };
		const glm::mat4 nodeRotationMatrix = glm::toMat4(glm::quat{ rotation->w_, rotation->x_, rotation->y_, rotation->z_ });;


		const glm::mat4 transformMatrix = glm::mat4{ 1 } * nodeRotationMatrix * nodeTranslateMatrix;

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
			.Include<Position3D>()
			.Include<Rotation3D>()
			.Exclude<DriverTransform3D>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId CreateDriverTransform3D::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverTransform3D>().GetId();
	}
}