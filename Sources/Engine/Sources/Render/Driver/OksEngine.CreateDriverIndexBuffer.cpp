
#include <Render/Driver/auto_OksEngine.CreateDriverIndexBuffer.hpp>


#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	void CreateDriverIndexBuffer::Update(Indices* indices) {

		//auto* indices = world->GetComponent<Indices>(entityId);

		//auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		//RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
		//	.indicesNumber_ = indices->indices_.GetIndicesNumber(),
		//	.indexType_ = RAL::Driver::IndexType::UI16,
		//	.type_ = RAL::Driver::IndexBuffer::Type::Const
		//};
		//RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);
		//driver->FillIndexBuffer(IBId, 0, indices->indices_.GetData(), indices->indices_.GetIndicesNumber());
		//world->CreateComponent<DriverIndexBuffer>(entityId, IBId);

	}
}