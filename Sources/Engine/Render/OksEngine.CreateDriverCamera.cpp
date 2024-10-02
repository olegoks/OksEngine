
#include <Render/OksEngine.CreateDriverCamera.hpp>

#include <Render/OksEngine.Camera.hpp>
#include <Render/OksEngine.DriverCamera.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverCamera::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		const auto* camera = world->GetComponent<Camera>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(DriverCamera::Matrices),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id UBId = driver->CreateUniformBuffer(UBCreateInfo);
		world->CreateComponent<DriverCamera>(entityId, /*matrices, */UBId);

	}

	inline std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverCamera::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Camera>().Exclude<DriverCamera>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	inline Common::TypeId CreateDriverCamera::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverCamera>().GetId();
	}

	CreateDriverCamera::CreateDriverCamera(Context& context) noexcept :
		ECSSystem{ context } { }

}