#include <Debug/Render/IndexBuffer\auto_OksEngine.CreateImGuiDriverIndexBuffer.hpp>

#include <Common.Types.hpp>

namespace OksEngine {
void CreateImGuiDriverIndexBuffer::Update(ECS2::Entity::Id entity1Id,
                                    const ImGuiState* imGuiState,
                                    ECS2::Entity::Id entity2Id,
                                    RenderDriver* renderDriver) {


	auto driver = renderDriver->driver_;

	const Common::UInt64 capacity = static_cast<Common::UInt64>(Common::Limits<Common::UInt16>().Max()) * 3;

	RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
		.indicesNumber_ = capacity,
		.indexType_ = RAL::Driver::IndexType::UI16,
		.type_ = RAL::Driver::IndexBuffer::Type::Const
	};
	const RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);

	CreateComponent<ImGuiDriverIndexBuffer>(entity1Id, IBId, capacity, 0);

};
}