
#include <Render/OksEngine.MapMeshTransform.hpp>

#include <Common/OksEngine.Position.hpp>
#include <Render/OksEngine.DriverModel.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	void MapMeshTransform::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position>(entityId);
		auto* driverModel = world->GetComponent<DriverModel>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		struct Transform {
			alignas(16) glm::mat4 model_;
		};

		Transform transform{ position->GetMat() };

		for (DriverModel::Mesh& mesh : driverModel->driverMeshs_) {
			auto& transformBinding = mesh.shaderBindings_[1];
			driver->FillUniformBuffer(transformBinding.uniformBufferId_, &transform);
		}

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> MapMeshTransform::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}
		.Include<Position>()
		.Include<DriverModel>(), ECS::Entity::Filter{}.ExcludeAll() };

	}

}