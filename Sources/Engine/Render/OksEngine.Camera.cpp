
#include <Render/OksEngine.Camera.hpp>
#include <Common/OksEngine.Position.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>


namespace OksEngine {


	void Camera::DirectionRight(float angle) {
		direction_ = glm::normalize(glm::rotate(
			direction_,
			glm::radians(-angle),
			{ 0.f, 1.f, 0.f })
		);
		up_ = glm::normalize(glm::rotate(
			up_,
			glm::radians(-angle),
			{ 0.f, 1.f, 0.f })
		);
	}


	void Camera::DirectionLeft(float angle) {
		direction_ = glm::normalize(glm::rotate(
			direction_,
			glm::radians(angle),
			{ 0.f, 1.f, 0.f })
		);
		up_ = glm::normalize(glm::rotate(
			up_,
			glm::radians(angle),
			{ 0.f, 1.f, 0.f })
		);
	}

	void Camera::DirectionUp(float angle) {
		const glm::vec3 perpendicular = glm::normalize(glm::cross(direction_, up_));
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(angle), perpendicular));
		up_ = glm::normalize(glm::rotate(up_, glm::radians(angle), perpendicular));
	}

	void Camera::DirectionDown(float angle) {
		const glm::vec3 perpendicular = glm::normalize(glm::cross(up_, direction_));
		direction_ = glm::normalize(glm::rotate(direction_, glm::radians(angle), perpendicular));
		up_ = glm::normalize(glm::rotate(up_, glm::radians(angle), perpendicular));
	}


	template<>
	void Bind<Camera>(Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<Camera>("Camera")
			.addConstructor<void(*)(
				const glm::vec3& direction,
				const glm::vec3& up, bool isActive)>()
			.addFunction("GetDirectionX", &Camera::GetDirectionX)
			.addFunction("GetDirectionY", &Camera::GetDirectionY)
			.addFunction("GetDirectionZ", &Camera::GetDirectionZ)
			.addFunction("SetDirectionX", &Camera::SetDirectionX)
			.addFunction("SetDirectionY", &Camera::SetDirectionY)
			.addFunction("SetDirectionZ", &Camera::SetDirectionZ)
			.addFunction("SetDirection", &Camera::SetDirection)
			.addFunction("GetUpX", &Camera::GetUpX)
			.addFunction("GetUpY", &Camera::GetUpY)
			.addFunction("GetUpZ", &Camera::GetUpZ)
			.addFunction("SetUpX", &Camera::SetUpX)
			.addFunction("SetUpY", &Camera::SetUpY)
			.addFunction("SetUpZ", &Camera::SetUpZ)
			.addFunction("GetUpZ", &Camera::GetUpZ)
			.addFunction("SetUpX", &Camera::SetUpX)
			.addFunction("SetUpY", &Camera::SetUpY)
			.addFunction("SetUpZ", &Camera::SetUpZ)
			.addFunction("DirectionUp", &Camera::DirectionUp)
			.addFunction("DirectionDown", &Camera::DirectionDown)
			.addFunction("DirectionLeft", &Camera::DirectionLeft)
			.addFunction("DirectionRight", &Camera::DirectionRight)
			.endClass();
	}

	template<>
	void Edit<Camera>(Camera* camera) {

		ImGui::PushID("Direction");
		{
			ImGui::TextDisabled("Direction:");
			ImGui::InputFloat("X", &camera->direction_.x);
			ImGui::InputFloat("Y", &camera->direction_.y);
			ImGui::InputFloat("Z", &camera->direction_.z);
		}
		ImGui::PopID();
		ImGui::PushID("Up");
		{
			ImGui::TextDisabled("Up:");
			ImGui::InputFloat("X", &camera->up_.x);
			ImGui::InputFloat("Y", &camera->up_.y);
			ImGui::InputFloat("Z", &camera->up_.z);
		}
		ImGui::PopID();

		ImGui::Checkbox("Is active", &camera->isActive_);

	}




	template<>
	void Add<Camera>(ECS::World* world, ECS::Entity::Id id) {
		static float directionX = 0.0f;
		static float directionY = 1.0f;
		static float directionZ = 0.0f;
		static float upX = 0.0f;
		static float upY = 0.0f;
		static float upZ = 1.0f;
		static bool isActive = true;
		if (ImGui::CollapsingHeader("Create info")) {

			{
				ImGui::PushID("Direction");
				{
					ImGui::TextDisabled("Direction:");
					ImGui::InputFloat("X", &directionX);
					ImGui::InputFloat("Y", &directionY);
					ImGui::InputFloat("Z", &directionZ);
				}
				ImGui::PopID();

				ImGui::PushID("Up");
				{
					ImGui::TextDisabled("Up:");
					ImGui::InputFloat("X", &upX);
					ImGui::InputFloat("Y", &upY);
					ImGui::InputFloat("Z", &upZ);
				}
				ImGui::PopID();
				ImGui::Checkbox("Is active", &isActive);
			}


			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Camera>(id)) {
				world->CreateComponent<Camera>(id,
					//glm::vec3{ positionX, positionY, positionZ },
					glm::vec3{ directionX , directionY, directionZ },
					glm::vec3{ upX, upY, upZ },
					isActive);
			}
		}
	}



}