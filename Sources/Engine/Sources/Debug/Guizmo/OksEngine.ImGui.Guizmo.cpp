#pragma once
#include <Debug\Guizmo\auto_OksEngine.ImGui.Guizmo.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <imguizmo/ImGuizmo.h>

#include <Presets/auto_OksEngine.EntityPresets.hpp>

namespace OksEngine
{
	namespace ImGUI
	{

		namespace Gizmo {

			void CreateState::Update() {

				const ECS2::Entity::Id stateEntityId = CreateEntity();

				CreateComponent<State>(stateEntityId);
				CreateComponent<CurrentOperation>(stateEntityId, Operation::Translate);
			}

			void DrawOperationSelectionWindow::Update(ECS2::Entity::Id entity0id, const OksEngine::ImGUI::Gizmo::State* state0,
				OksEngine::ImGUI::Gizmo::CurrentOperation* currentOperation0, ECS2::Entity::Id entity1id,
				const OksEngine::ImGUI::State* imGUI__State1) {

				ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove
					| ImGuiWindowFlags_NoResize
					| ImGuiWindowFlags_NoTitleBar
					| ImGuiWindowFlags_NoCollapse;

				if (ImGui::Begin("Transformation Modes", nullptr, flags))
				{
					// Горизонтальное расположение кнопок
					if (ImGui::Button("Translate"))
						currentOperation0->operation_ = Operation::Translate;

					ImGui::SameLine();

					if (ImGui::Button("Rotate"))
						currentOperation0->operation_ = Operation::Rotate;

					ImGui::SameLine();

					if (ImGui::Button("Scale"))
						currentOperation0->operation_ = Operation::Scale;

					// Дополнительно: индикация активного режима
					// Например, выделение кнопки или вывод текста
					ImGui::SameLine();
					ImGui::TextUnformatted("Mode: ");
					ImGui::SameLine();
					switch (currentOperation0->operation_)
					{
					case Operation::Translate: ImGui::Text("Translate"); break;
					case Operation::Rotate:    ImGui::Text("Rotate");    break;
					case Operation::Scale:     ImGui::Text("Scale");     break;
					}
				}
				ImGui::End();

			}

			void Process::Update(
				ECS2::Entity::Id entity0id, const OksEngine::ImGUI::State* imGUI__State0, ECS2::Entity::Id entity1id,
				const OksEngine::ImGUI::Gizmo::State* state1,
				const OksEngine::ImGUI::Gizmo::CurrentOperation* currentOperation1, ECS2::Entity::Id entity2id,
				const OksEngine::MainWindow* mainWindow2,
				const OksEngine::MainWindowFramebufferSize* mainWindowFramebufferSize2,
				const OksEngine::MainWindowWorkAreaSize* mainWindowWorkAreaSize2, ECS2::Entity::Id entity3id,
				const OksEngine::Camera* camera3, const OksEngine::Active* active3,
				const OksEngine::WorldPosition3D* worldPosition3D3, const OksEngine::Direction3D* direction3D3,
				const OksEngine::Up3D* up3D3, const OksEngine::ZNear* zNear3, const OksEngine::ZFar* zFar3,
				const OksEngine::Width* width3, const OksEngine::Height* height3, ECS2::Entity::Id entity4id,
				const OksEngine::Render::Outline::State* render__Outline__State4,
				const OksEngine::Render::Outline::EnableSelection* render__Outline__EnableSelection4,
				const OksEngine::Render::Outline::SelectedEntityIds* render__Outline__SelectedEntityIds4,
				ECS2::Entity::Id entity5id, OksEngine::RenderDriver* renderDriver5) {

				int display_w = mainWindowFramebufferSize2->width_;
				int display_h = mainWindowFramebufferSize2->height_;

				ImGuizmo::BeginFrame();
				// Прямоугольник гизмо = весь экран/окно
				ImGuizmo::SetRect(0, 0, (float)display_w, (float)display_h);

				const glm::mat4 view = glm::lookAt(
					glm::vec3(worldPosition3D3->x_, worldPosition3D3->y_, worldPosition3D3->z_),
					glm::vec3(worldPosition3D3->x_, worldPosition3D3->y_, worldPosition3D3->z_) + glm::vec3(direction3D3->x_, direction3D3->y_, direction3D3->z_),
					glm::vec3(up3D3->x_, up3D3->y_, up3D3->z_)
				);

				glm::mat4 proj = glm::perspective(
					glm::radians(45.0f),
					width3->value_ / (float)height3->value_,
					zNear3->value_, zFar3->value_);

				//proj[1][1] *= -1;

				if (render__Outline__SelectedEntityIds4->ids_.size() == 1) {

					const ECS2::Entity::Id selectedEntityId = render__Outline__SelectedEntityIds4->ids_.front();
					const ECS2::ComponentsFilter selectedEntityComponentFilter = GetComponentsFilter(selectedEntityId);




					if (currentOperation1->operation_ == Operation::Translate) {
						if (selectedEntityComponentFilter.IsSet<WorldPosition3D>()) {

							auto* worldPosition3D = GetComponent<WorldPosition3D>(selectedEntityId);

							const glm::vec3 translate{
								worldPosition3D->x_,
								worldPosition3D->y_,
								worldPosition3D->z_
							};

							glm::mat4 model = glm::translate(glm::mat4(1.0f), translate);


							ImGuizmo::Manipulate(
								glm::value_ptr(view),
								glm::value_ptr(proj),
								ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD,
								glm::value_ptr(model)
							);

							if (ImGuizmo::IsUsing()) {
								glm::vec3 scale;
								glm::quat rotation;
								glm::vec3 translation;
								glm::vec3 skew;
								glm::vec4 perspective;
								glm::decompose(model, scale, rotation, translation, skew, perspective);

								worldPosition3D->x_ = translation.x;
								worldPosition3D->y_ = translation.y;
								worldPosition3D->z_ = translation.z;

							}

						}
					}
					else if (currentOperation1->operation_ == Operation::Rotate) {

						if (selectedEntityComponentFilter.IsSet<WorldRotation3D>()) {

							auto* worldRotation3D = GetComponent<WorldRotation3D>(selectedEntityId);

							const glm::quat rotation{
								worldRotation3D->w_,
								worldRotation3D->x_,
								worldRotation3D->y_,
								worldRotation3D->z_
							};
							glm::mat4 model = glm::mat4_cast(rotation);

							if (selectedEntityComponentFilter.IsSet<WorldPosition3D>()) {

								auto* worldPosition3D = GetComponent<WorldPosition3D>(selectedEntityId);

								const glm::vec3 translate{
									worldPosition3D->x_,
									worldPosition3D->y_,
									worldPosition3D->z_
								};

								model = glm::translate(glm::mat4(1.0f), translate) * glm::mat4_cast(rotation);
							}

							ImGuizmo::Manipulate(
								glm::value_ptr(view),
								glm::value_ptr(proj),
								ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::WORLD,
								glm::value_ptr(model)
							);

							if (ImGuizmo::IsUsing()) {
								glm::vec3 scale;
								glm::quat rotation;
								glm::vec3 translation;
								glm::vec3 skew;
								glm::vec4 perspective;
								glm::decompose(model, scale, rotation, translation, skew, perspective);

								worldRotation3D->w_ = rotation.w;
								worldRotation3D->x_ = rotation.x;
								worldRotation3D->y_ = rotation.y;
								worldRotation3D->z_ = rotation.z;
							}
						}
					}
					else if (currentOperation1->operation_ == Operation::Scale) {
						if (selectedEntityComponentFilter.IsSet<WorldScale3D>()) {

							auto* worldScale3D = GetComponent<WorldScale3D>(selectedEntityId);

							const glm::vec3 scale{
								worldScale3D->x_,
								worldScale3D->y_,
								worldScale3D->z_
							};
							glm::mat4 model = glm::scale(scale);

							ImGuizmo::Manipulate(
								glm::value_ptr(view),
								glm::value_ptr(proj),
								ImGuizmo::OPERATION::SCALE, ImGuizmo::MODE::WORLD,
								glm::value_ptr(model)
							);

							if (ImGuizmo::IsUsing()) {
								glm::vec3 scale;
								glm::quat rotation;
								glm::vec3 translation;
								glm::vec3 skew;
								glm::vec4 perspective;
								glm::decompose(model, scale, rotation, translation, skew, perspective);

								worldScale3D->x_ = scale.x;
								worldScale3D->y_ = scale.y;
								worldScale3D->z_ = scale.z;
							}
						}
					}
				}
			}
		}
	}

} // namespace OksEngine