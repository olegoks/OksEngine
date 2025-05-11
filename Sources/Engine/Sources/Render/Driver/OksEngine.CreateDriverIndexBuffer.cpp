
#include <Render/Driver/auto_OksEngine.CreateDriverIndexBuffer.hpp>


#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverIndexBuffer::Update(
		ECS2::Entity::Id entity1Id,
		const Indices* indices,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		auto driver = renderDriver->driver_;
		RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
			.indicesNumber_ = indices->indices_.GetIndicesNumber(),
			.indexType_ = RAL::Driver::IndexType::UI16,
			.type_ = RAL::Driver::IndexBuffer::Type::Const
		};
		RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);
		driver->FillIndexBuffer(IBId, 0, indices->indices_.GetData(), indices->indices_.GetIndicesNumber());
		CreateComponent<DriverIndexBuffer>(entity1Id, IBId);

	}
}