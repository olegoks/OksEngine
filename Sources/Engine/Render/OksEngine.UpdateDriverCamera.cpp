
#include <Render/OksEngine.UpdateDriverCamera.hpp>

#include <Render/OksEngine.DriverCamera.hpp>
#include <Common/OksEngine.Position.hpp>
#include <Render/OksEngine.Camera.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

namespace OksEngine {

	UpdateDriverCamera::UpdateDriverCamera(Context& context) noexcept :
		ECSSystem{ context } { }

	void UpdateDriverCamera::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
		auto* camera = world->GetComponent<Camera>(entityId);
		//if (camera->IsActive()) {
		auto* position = world->GetComponent<Position>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();
		auto* driverCamera = world->GetComponent<DriverCamera>(entityId);


		const glm::mat4 view = glm::lookAt(
			position->translate_,
			position->translate_ + camera->direction_,
			camera->GetUp()
		);

		glm::mat4 proj = glm::perspective(
			glm::radians(45.0f),
			camera->width_ / (float)camera->height_,
			camera->zNear_, camera->zFar_);

		proj[1][1] *= -1;

		DriverCamera::Matrices matrices{
			.view_ = view,
			.proj_ = proj
		};


		driver->FillUniformBuffer(driverCamera->matricesBuffer_, &matrices);
		//driver->GetCamera()->SetPosition(position->GetTranslateVec());
		//driver->GetCamera()->SetDirection(camera->GetDirection());
		//driver->GetCamera()->SetUp(camera->GetUp());
	//}
	}

	std::pair<ECS::Entity::Filter, ECS::Entity::Filter> UpdateDriverCamera::GetFilter() const noexcept {
		return { ECS::Entity::Filter{}.Include<Camera>().Include<Position>().Include<DriverCamera>(), ECS::Entity::Filter{}.ExcludeAll() };
	}

	Common::TypeId UpdateDriverCamera::GetTypeId() const noexcept {
		return Common::TypeInfo<UpdateDriverCamera>().GetId();
	}

}
