
#include <Render/auto_OksEngine.MapMeshTransform.hpp>

namespace OksEngine {


	void MapMeshTransform::Update(
		ECS2::Entity::Id entityId,
		const Position3D* position3D1) {

		//auto* position = world->GetComponent<Position3D>(entityId);
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


}