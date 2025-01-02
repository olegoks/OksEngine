
#include <Render/OksEngine.MapMeshTransform.hpp>

#include <Common/auto_OksEngine.Position3D.hpp>
#include <Render/Model/OksEngine.ModelEntity.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {


	void MapMeshTransform::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* position = world->GetComponent<Position3D>(entityId);
		/*auto* driverModel = world->GetComponent<DriverModel>(entityId);

		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		struct Transform {
			alignas(16) glm::mat4 model_;
		};

		Transform transform{ position->GetMat() };

		for (DriverModel::Mesh& mesh : driverModel->driverMeshs_) {
			auto& transformBinding = mesh.shaderBindings_[1];
			driver->FillUniformBuffer(transformBinding.uniformBufferId_, &transform);
		}*/

	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> MapMeshTransform::GetFilter() const noexcept {
		static std::pair<ECS::Entity::Filter, ECS::Entity::Filter> filter = 
		{ ECS::Entity::Filter{}
			.Include<Position3D>()
			.Include<ModelEntity>(),
			ECS::Entity::Filter{}.ExcludeAll() };

		return filter;

	}

}