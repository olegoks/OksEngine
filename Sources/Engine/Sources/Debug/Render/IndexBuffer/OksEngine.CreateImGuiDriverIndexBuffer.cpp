#include <Debug/Render/IndexBuffer\auto_OksEngine.CreateImGuiDriverIndexBuffer.hpp>

#include <Common.Types.hpp>
#include <Common.StringLiterals.hpp>

namespace OksEngine {
void CreateImGuiDriverIndexBuffer::Update(ECS2::Entity::Id entity1Id,
                                    const ImGuiState* imGuiState,
                                    ECS2::Entity::Id entity2Id,
                                    RenderDriver* renderDriver) {


	auto driver = renderDriver->driver_;

	using namespace Common;

	constexpr Common::UInt64 capacity = 30_MB;

	RAL::Driver::IndexBuffer::CreateInfo1 IBCI{
		.indicesNumber_ = capacity,
		.indexType_ = RAL::Driver::IndexType::UI32,
		.type_ = RAL::Driver::IndexBuffer::Type::Const
	};
	const RAL::Driver::IndexBuffer::Id IBId = driver->CreateIndexBuffer(IBCI);

	CreateComponent<ImGuiDriverIndexBuffer>(entity1Id, IBId, capacity, 0);

};
}