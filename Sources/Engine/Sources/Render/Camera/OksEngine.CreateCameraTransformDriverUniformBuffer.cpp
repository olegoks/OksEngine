
#include <Render/Camera/OksEngine.CreateCameraTransformDriverUniformBuffer.hpp>

#include <Render/Camera/OksEngine.Camera.hpp>
#include <Render/Driver/OksEngine.UniformBuffer.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Render/Camera/OksEngine.CameraTransformUniformBufferType.hpp>

namespace OksEngine {

	void CreateDriverCameraTransformDriverUniformBuffer::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		const auto* camera = world->GetComponent<Camera>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();


		RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(ViewProjection),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		RAL::Driver::UniformBuffer::Id UBId = driver->CreateUniformBuffer(UBCreateInfo);

		world->CreateComponent<UniformBuffer>(entityId, /*matrices, */UBId);

	}

	inline std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverCameraTransformDriverUniformBuffer::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Camera>().Exclude<UniformBuffer>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	inline Common::TypeId CreateDriverCameraTransformDriverUniformBuffer::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverCameraTransformDriverUniformBuffer>().GetId();
	}

	CreateDriverCameraTransformDriverUniformBuffer::CreateDriverCameraTransformDriverUniformBuffer(Context& context) noexcept :
		ECSSystem{ context } { }

}