
#include <Render/Model/OksEngine.CreateDriverIndexBuffer.hpp>

#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverIndexBuffer::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* indices = world->GetComponent<Indices>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
			.indicesNumber_ = indices->indices_.GetIndicesNumber(),
			.indexType_ = RAL::Driver::IndexType::UI16,
			.type_ = RAL::Driver::IndexBuffer::Type::Const
		};
		RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);
		driver->FillIndexBuffer(IBId, 0, indices->indices_.GetData(), indices->indices_.GetIndicesNumber());
		world->CreateComponent<DriverIndexBuffer>(entityId, IBId);

	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> CreateDriverIndexBuffer::GetFilter() const noexcept {
		return {
			ECS::Entity::Filter{}
			.Include<Indices>()
			.Exclude<DriverIndexBuffer>(),
			ECS::Entity::Filter{}.ExcludeAll() };
	}
	Common::TypeId CreateDriverIndexBuffer::GetTypeId() const noexcept {
		return Common::TypeInfo<CreateDriverIndexBuffer>().GetId();
	}
}