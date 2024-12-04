
#include <Render/Driver/OksEngine.UpdateDriverIndexBuffer.hpp>

#include <Render/OksEngine.Render.Components.hpp>
#include <Common/OksEngine.Common.Components.hpp>

#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void UpdateDriverIndexBuffer::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* indices = world->GetComponent<Indices>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		auto driverIB = world->GetComponent<DriverIndexBuffer>(entityId);
		const Common::Size currentIBSize = driver->GetIBSizeInBytes(driverIB->id_);
		if (currentIBSize != indices->indices_.GetSizeInBytes()) {
			driver->ResizeIndexBuffer(driverIB->id_, indices->indices_.GetIndicesNumber());
			/*driver->DestroyIndexBuffer(driverIB->id_);
			RAL::Driver::IndexBuffer::CreateInfo1 ibci{
				.indicesNumber_ = indices->indices_.GetIndicesNumber(),
				.indexType_ = RAL::Driver::IndexType::UI16,
				.type_ = RAL::Driver::IndexBuffer::Type::Mutable
			};
			const RAL::Driver::IndexBuffer::Id ibid = driver->CreateIndexBuffer(ibci);
			driverIB->id_ = ibid;*/
		}

		driver->FillIndexBuffer(driverIB->id_, 0, indices->indices_.GetData(), indices->indices_.GetIndicesNumber());
	}
	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateDriverIndexBuffer::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = {
			ECS::Entity::Filter{}
			.Include<Indices>()
			.Include<MutableMesh>()
			.Include<DriverIndexBuffer>(),
			ECS::Entity::Filter{}.ExcludeAll() };
		return filter;
	}
	Common::TypeId UpdateDriverIndexBuffer::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateDriverIndexBuffer>().GetId();
	}
}