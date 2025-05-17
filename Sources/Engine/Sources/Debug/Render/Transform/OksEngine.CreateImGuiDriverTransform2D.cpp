#include <Debug/Render/Transform\auto_OksEngine.CreateImGuiDriverTransform2D.hpp>

#include <Debug/Render/Transform/OksEngine.ImGuiTransform.hpp>

namespace OksEngine {
	void CreateImGuiDriverTransform2D::Update(
		ECS2::Entity::Id entity1Id,
		const ImGuiState* imGuiState,
		ECS2::Entity::Id entity2Id,
		RenderDriver* renderDriver) {

		auto driver = renderDriver->driver_;

		const RAL::Driver::UniformBuffer::CreateInfo UBCreateInfo{
			.size_ = sizeof(ImGuiTransform),
			.type_ = RAL::Driver::UniformBuffer::Type::Mutable
		};
		const RAL::Driver::UniformBuffer::Id ubId = driver->CreateUniformBuffer(UBCreateInfo);

		CreateComponent<DriverTransform2D>(entity1Id, ubId);

	};
}