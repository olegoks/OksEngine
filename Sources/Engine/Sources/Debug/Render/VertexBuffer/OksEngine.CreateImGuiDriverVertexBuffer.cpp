#include <Debug/Render/VertexBuffer\auto_OksEngine.CreateImGuiDriverVertexBuffer.hpp>

namespace OksEngine {
void CreateImGuiDriverVertexBuffer::Update(ECS2::Entity::Id entity1Id,
                                     const ImGuiState* imGuiState,
                                     ECS2::Entity::Id entity2Id,
                                     RenderDriver* renderDriver) {


	auto driver = renderDriver->driver_;

	const Common::UInt64 capacity = Common::Limits<Common::UInt16>::Max();

	RAL::Driver::VertexBuffer::CreateInfo1 VBCI{
		.verticesNumber_ = capacity,
		.vertexType_ = RAL::Driver::VertexType::VF2_TF2_CF4,
		.type_ = RAL::Driver::VertexBuffer::Type::Const
	};
	RAL::Driver::VertexBuffer::Id VBId = driver->CreateVertexBuffer(VBCI);
	CreateComponent<ImGuiDriverVertexBuffer>(entity1Id, VBId, capacity, 0);
};
}