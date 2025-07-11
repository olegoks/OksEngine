#pragma once
#include <Render\Driver\auto_OksEngine.DriverIndexBuffer.hpp>

namespace OksEngine
{
	void UpdateDriverIndexBuffer::Update(ECS2::Entity::Id entity0id, const Indices* indices0,
		DriverIndexBuffer* driverIndexBuffer0,
		ECS2::Entity::Id entity1id, RenderDriver* renderDriver1) {



		//auto* indices = world->GetComponent<Indices>(entityId);

		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		//auto driverIB = world->GetComponent<DriverIndexBuffer>(entityId);
		//const Common::Size currentIBSize = driver->GetIBSizeInBytes(driverIB->id_);
		//if (currentIBSize != indices->indices_.GetSizeInBytes()) {
		//	driver->ResizeIndexBuffer(driverIB->id_, indices->indices_.GetIndicesNumber());
		//	/*driver->DestroyIndexBuffer(driverIB->id_);
		//	RAL::Driver::IndexBuffer::CreateInfo1 ibci{
		//		.indicesNumber_ = indices->indices_.GetIndicesNumber(),
		//		.indexType_ = RAL::Driver::IndexType::UI16,
		//		.type_ = RAL::Driver::IndexBuffer::Type::Mutable
		//	};
		//	const RAL::Driver::IndexBuffer::Id ibid = driver->CreateIndexBuffer(ibci);
		//	driverIB->id_ = ibid;*/
		//}

		//driver->FillIndexBuffer(driverIB->id_, 0, indices->indices_.GetData(), indices->indices_.GetIndicesNumber());

	};

	void CreateDriverIndexBuffer::Update(

		ECS2::Entity::Id entity0id,
		const Indices* indices0,
		
		ECS2::Entity::Id entity1id,
		RenderDriver* renderDriver1) {


		auto driver = renderDriver1->driver_;
		RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
			.indicesNumber_ = indices0->indices_.GetIndicesNumber(),
			.indexType_ = RAL::Driver::IndexType::UI16,
			.type_ = RAL::Driver::IndexBuffer::Type::Const
		};
		RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);
		driver->FillIndexBuffer(IBId, 0, indices0->indices_.GetData(), indices0->indices_.GetIndicesNumber());
		CreateComponent<DriverIndexBuffer>(entity0id, IBId);


	};

} // namespace OksEngine