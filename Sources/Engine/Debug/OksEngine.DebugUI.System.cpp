
#include <Debug/OksEngine.DebugUI.System.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>
#include <Common/OksEngine.MapRigidBodyToRenderGeometry.hpp>
#include <Render/OksEngine.AddImmutableRenderGeometryFromObjRequest.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>


#include <implot.h>
#include <implot_internal.h>

namespace OksEngine {

	void ImGuiSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		GetContext().GetRenderSubsystem()->RenderImGui();
	}

	void MainMenuBarSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		//auto* menuBar = world->GetComponent<MainMenuBar>(entityId);

		//	GetContext().GetRenderSubsystem()->AddImGuiCallback([]() {


		//		if (ImGui::BeginMainMenuBar()) {
		//			// Add items to the menu bar.
		//			if (ImGui::BeginMenu("Engine")) {
		//				static bool showPerformanceProfiler_ = false;
		//				ImGui::MenuItem("Performance profiler", nullptr, &menuBar->showPerformanceProfiler_);
		//				ImGui::MenuItem("ECS Inspector", NULL, false, false);
		//				ImGui::EndMenu();
		//			}
		//			// End the menu bar.
		//			ImGui::EndMainMenuBar();
		//		}
		//		if (ImGui::BeginMainMenuBar()) {
		//			// Add items to the menu bar.
		//			if (ImGui::BeginMenu("Engine")) {
		//				ImGui::MenuItem("Render", NULL, false, false);
		//				ImGui::MenuItem("Help", NULL, false, false);
		//				ImGui::EndMenu();
		//			}
		//			// End the menu bar.
		//			ImGui::EndMainMenuBar();
		//		}
		//		ImGui::ShowDemoWindow();
		//		});
	}

	void EnginePerformanceSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* enginePerformance = world->GetComponent<EnginePerformance>(entityId);

		if (enginePerformance->show_) {

			if (ImGui::BeginMainMenuBar()) {
				// Add items to the menu bar.
				if (ImGui::BeginMenu("Engine")) {
					ImGui::MenuItem(
						"Performance profiler",
						nullptr,
						&enginePerformance->show_);
					ImGui::EndMenu();
				}
				// End the menu bar.
				ImGui::EndMainMenuBar();
			}


			bool isOpen = true;
			ImGui::Begin("Engine performance", &isOpen, 0);


			static Common::UInt64 renderCalls = 0;
			++renderCalls;

			using namespace std::chrono_literals;
			std::chrono::high_resolution_clock::time_point now;
			static std::chrono::high_resolution_clock::time_point point = std::chrono::high_resolution_clock::now();;

			now = std::chrono::high_resolution_clock::now();
			auto delta = now - point;

			auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
			static Common::Size lastFps = 0;
			static std::vector<Common::Size> fps_;
			static std::vector<Common::Size> timePoints_;
			if (microseconds > 1000000) {
				Common::Size framesPerSecond = renderCalls * 1000000 / microseconds;
				ImGui::TextDisabled("Fps: %d", framesPerSecond);
				fps_.push_back(framesPerSecond);
				timePoints_.push_back(fps_.size());
				renderCalls = 0;
				point = now;
				lastFps = framesPerSecond;

			}
			else {
				ImGui::TextDisabled("Fps: %d", lastFps);
			}

			auto* framesCounter = world->GetComponent<FramesCounter>(entityId);
			ImGui::TextDisabled("Frame: %d", framesCounter->framesCount_);
			//ImGui::Begin("My Window");
			if (ImPlot::BeginPlot("My Plot")) {
				ImPlot::PlotLine("My Line Plot", timePoints_.data(), fps_.data(), static_cast<Common::UInt32>(fps_.size()));
				ImPlot::EndPlot();
			}
			//ImGui::End();
			ImGui::End();
		}


	}

	void CollectEntitiesInfo::BeforeUpdate(ECS::World* world) {
		bool isOpen = true;
		ImGui::Begin("ECS World Info", &isOpen, 0);

		ImGui::TextDisabled("Entities number: %d", world->GetEntitiesNumber());
		ImGui::TextDisabled("Entities:");
		if (ImGui::Button("Add entity")) {
			ECS::Entity::Id id = world->CreateEntity();
		}
		ImGui::Separator();
	}

	void CollectEntitiesInfo::Update(ECS::World* world, ECS::Entity::Id id) {

		const std::string idString = std::to_string(id);
		ImGui::PushID(idString.c_str());

		if (ImGui::CollapsingHeader(("Id: " + idString).c_str()), ImGuiTreeNodeFlags_SpanAllColumns) {
			if (world->IsComponentExist<Position>(id)) {
				if (ImGui::CollapsingHeader("Position")) {
					Position* position = world->GetComponent<Position>(id);
					ImGui::InputFloat("X", &position->translate_.x);
					ImGui::InputFloat("Y", &position->translate_.y);
					ImGui::InputFloat("Z", &position->translate_.z);
					ImGui::Spacing();
				}
			}


			bool isExist = world->IsComponentExist<ImmutableRenderGeometry>(id);
			if (ImGui::CollapsingHeader("ImmutableRenderGeometry", &isExist)) {
				ImmutableRenderGeometry* renderGeometry = world->GetComponent<ImmutableRenderGeometry>(id);
				ImGui::TextDisabled("Immutable render geometry: %s", renderGeometry->modelObjFileName_.c_str());
			}
			if (!isExist) {
				if (world->IsComponentExist<ImmutableRenderGeometry>(id)) {
					world->RemoveComponent<ImmutableRenderGeometry>(id);
				}
			}

			if (world->IsComponentExist<AddImmutableRenderGeometryFromObjRequest>(id)) {
				if (ImGui::CollapsingHeader("AddImmutableRenderGeometryFromObjRequest")) {
					auto* addImmutableRenderGeometryfromObj = world->GetComponent<AddImmutableRenderGeometryFromObjRequest>(id);

					ImGui::TextDisabled("Obj: %s", addImmutableRenderGeometryfromObj->obj_.c_str());
					ImGui::TextDisabled("Mtl: %s", addImmutableRenderGeometryfromObj->mtl_.c_str());
					ImGui::TextDisabled("Textures:");
					for (auto& texture : addImmutableRenderGeometryfromObj->textures_) {
						ImGui::TextDisabled("	%s", texture.c_str());
					}

					if (ImGui::Button("Delete")) {
						world->RemoveComponent<AddImmutableRenderGeometryFromObjRequest>(id);
					}
					ImGui::Spacing();
				}
			}
			if (world->IsComponentExist<StaticRigidBodyCustomMeshShape>(id)) {
				if (ImGui::CollapsingHeader("StaticRigidBodyCustomMeshShape")) {
					StaticRigidBodyCustomMeshShape* staticRigidBodyCustomMeshShape = world->GetComponent<StaticRigidBodyCustomMeshShape>(id);
					ImGui::TextDisabled("StaticRigidBodyCustomMeshShape: %s", staticRigidBodyCustomMeshShape->geomName_);
					if (ImGui::Button("Delete")) { world->RemoveComponent<StaticRigidBodyCustomMeshShape>(id); }
					ImGui::Spacing();
				}
			}
			if (world->IsComponentExist<MainMenuBar>(id)) {
				if (ImGui::CollapsingHeader("MainMenuBar")) {
					ImGui::Spacing();
				}
			}
			if (world->IsComponentExist<ECSInspector>(id)) {
				if (ImGui::CollapsingHeader("ECSInspector")) {
					ImGui::Spacing();
				}
			}
			if (world->IsComponentExist<ImGuiContext>(id)) {
				if (ImGui::CollapsingHeader("ImGuiContext")) {
					ImGui::Spacing();
				}
			}
			if (world->IsComponentExist<MapRigidBodyToRenderGeometry>(id)) {
				if (ImGui::CollapsingHeader("MapRigidBodyToRenderGeometry")) {
					MapRigidBodyToRenderGeometry* mapRigidBodyToRenderGeometry = world->GetComponent<MapRigidBodyToRenderGeometry>(id);
					ImGui::TextDisabled("MapRigidBodyToRenderGeometry");
					if (ImGui::Button("Delete")) { world->RemoveComponent<MapRigidBodyToRenderGeometry>(id); }
					ImGui::Spacing();
				}
			}

			auto& state = GetCreateState(id);
			ImGui::SeparatorText("Add component");
			const char* items[] = {
				"Position",
				"AddImmutableRenderGeometryFromObjRequest",
				"ImmutableRenderGeometry",
				"StaticRigidBodyCustomMeshShape",
				"MapRigidBodyToRenderGeometry" };
			ImGui::Combo("", &state.currentAddComponentIndex_, items, 4);

			// To get unique button behaviour to each entity.
			bool addComponent = false;
			if (ImGui::Button("Add component")) {
				addComponent = true;
			}
			const std::string currentComponent = items[state.currentAddComponentIndex_];
			if (currentComponent == "Position") {
				static float x = 0.0f;
				static float y = 0.0f;
				static float z = 0.0f;
				if (ImGui::CollapsingHeader("Create info")) {
					ImGui::InputFloat("X", &x);
					ImGui::InputFloat("Y", &y);
					ImGui::InputFloat("Z", &z);
					ImGui::Spacing();
				}
				if (addComponent) {
					if (!world->IsComponentExist<Position>(id)) {
						world->CreateComponent<Position>(id, x, y, z);
					}
				}
			}
			if (currentComponent == "AddImmutableRenderGeometryFromObjRequest") {
				static char obj[100] = { "" };
				static char mtl[100] = { "" };
				static Common::Size texturesNumber = 1;
				static std::vector<std::unique_ptr<char[]>> textures;
				if (ImGui::CollapsingHeader("Create info")) {
					ImGui::InputText("Obj", obj, sizeof(obj));
					ImGui::InputText("Mtl", mtl, sizeof(mtl));
					{
						
						if(texturesNumber <= textures.size()) {
							textures.resize(texturesNumber);
						} else {
							for (Common::Size i = 0; i < texturesNumber - textures.size(); i++) {
								auto newTexture = std::make_unique<char[]>(100);
								std::memset(newTexture.get(), 0, 100);
								textures.push_back(std::move(newTexture));
							}
						}
						ImGui::TextDisabled("Textures:");
						for (Common::Size i = 0; i < texturesNumber; i++) {
							ImGui::PushID(std::to_string(i).c_str());
							ImGui::InputText("", (char*)textures[i].get(), 100); ImGui::SameLine();
							if (ImGui::SmallButton("-")) { textures.erase(textures.begin() + i); --texturesNumber; }
							ImGui::PopID();
						}
						if (ImGui::SmallButton("+")) { ++texturesNumber; }
					}
					ImGui::Spacing();
				}
				if (addComponent) {
					if (!world->IsComponentExist<AddImmutableRenderGeometryFromObjRequest>(id)) {
						std::vector<std::string> texturesStrs;
						for(auto& textureInput : textures) {
							texturesStrs.push_back(textureInput.get());
						}
						world->CreateComponent<AddImmutableRenderGeometryFromObjRequest>(id, obj, mtl, texturesStrs);
					}
				}
			}
		}
		ImGui::Separator();

		ImGui::PopID();
		ImGui::ShowDemoWindow();
	}

	void CollectEntitiesInfo::AfterUpdate(ECS::World* world)
	{
		ImGui::End();
	}

	ECSInspectorSystem::ECSInspectorSystem(Context& context) noexcept :
		ECSSystem{ context } { }

	void ECSInspectorSystem::Update(ECS::World* world, ECS::Entity::Id entityId) {
		auto* ecsInspector = world->GetComponent<ECSInspector>(entityId);
		if (ecsInspector != nullptr) {

			if (ImGui::BeginMainMenuBar()) {
				// Add items to the menu bar.
				if (ImGui::BeginMenu("Engine")) {
					ImGui::MenuItem("ECS Inspector", nullptr, &ecsInspector->show_);
					ImGui::EndMenu();
				}
				// End the menu bar.
				ImGui::EndMainMenuBar();
			}
		}
	}

	Common::TypeId ECSInspectorSystem::GetTypeId() const noexcept {
		return Common::TypeInfo<ECSInspectorSystem>().GetId();
	}
}