
#include <Debug/OksEngine.DebugUI.System.hpp>
#include <Render/OksEngine.Render.Subsystem.hpp>

#include <OksEngine.Components.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>

#include <implot.h>
#include <implot_internal.h>

namespace OksEngine {



	void ImGuiSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		auto* state = world->GetComponent<ImGuiState>(entityId);
		if (state->fontsTextureId_.IsInvalid()) {
			ImGuiIO& io = ImGui::GetIO();
			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			std::vector<RAL::Color4b> pixelsRGBA;
			pixelsRGBA.resize(width * height);
			std::memcpy(pixelsRGBA.data(), pixels, width * height * sizeof(RAL::Color4b));

			auto driver = GetContext().GetRenderSubsystem()->GetDriver();
			RAL::Texture::CreateInfo textureCreateInfo{
				.name_ = "ImGui texture",
				.pixels_ = std::move(pixelsRGBA),
				.size_ = { width, height }
			};

			state->fontsTextureId_ = driver->CreateTexture(textureCreateInfo);
		}
	}

	void ImGuiRenderSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

		ImGuiState* state = world->GetComponent<ImGuiState>(entityId);
		auto driver = GetContext().GetRenderSubsystem()->GetDriver();

		ImDrawData* draw_data = ImGui::GetDrawData();
		if (draw_data == nullptr) return;
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
		int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
		if (fb_width <= 0 || fb_height <= 0)
			return;

		//// Setup viewport:
		//{
		//	VkViewport viewport;
		//	viewport.x = 0;
		//	viewport.y = 0;
		//	viewport.width = (float)fb_width;
		//	viewport.height = (float)fb_height;
		//	viewport.minDepth = 0.0f;
		//	viewport.maxDepth = 1.0f;
		//	//vkCmdSetViewport(command_buffer, 0, 1, &viewport);
		//}

		const glm::vec2 scale{
			2.0f / draw_data->DisplaySize.x,
			2.0f / draw_data->DisplaySize.y
		};

		const glm::vec2 translate{
			-1.0f - draw_data->DisplayPos.x * scale[0],
			-1.0f - draw_data->DisplayPos.y * scale[1]
		};


		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				{
					driver->DrawIndexed(
						scale,
						translate,
						(RAL::Vertex2ftc*)cmd_list->VtxBuffer.Data + pcmd->VtxOffset,
						cmd_list->VtxBuffer.Size - pcmd->VtxOffset,
						cmd_list->IdxBuffer.Data + pcmd->IdxOffset,
						cmd_list->IdxBuffer.Size - pcmd->IdxOffset,
						state->fontsTextureId_);
				}
			}



		}
	}

	void MainMenuBarSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {

	}

	void EnginePerformanceSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
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

	//template<class ... Types>
	//void Foo() {

	//}
	//
	//template <typename... Types>
	//void callProcessForEach(ECS::World* world, ECS::Entity::Id id) {
	//	(Edit<Types>(Types* component),...);
	//}

	void CollectEntitiesInfo::Update(ECS::World* world, ECS::Entity::Id id, ECS::Entity::Id secondEntityId) {

		const std::string idString = std::to_string(id);
		ImGui::PushID(idString.c_str());

		if (ImGui::CollapsingHeader(("Id: " + idString).c_str())) {
			ImGui::Indent(20.0f);
			ECS::Entity::Filter entityFilter = world->GetEntityFilter(id);
			ImGui::TextDisabled("Components:");
			ImGui::TextDisabled("%s", entityFilter.IncludesToString().c_str());
			auto editComponent = []<class ComponentType>(ECS::World * world, ECS::Entity::Id id) {

				bool isExist = world->IsComponentExist<ComponentType>(id);
				if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist)) {
					ComponentType* component = world->GetComponent<ComponentType>(id);
					Edit<ComponentType>(component);
					ImGui::Spacing();
				}
				if (!isExist) {
					if (world->IsComponentExist<ComponentType>(id)) {
						world->RemoveComponent<ComponentType>(id);
					}
				}
			};

			ImGui::PushID("Edit");


			//callProcessForEach<Position, MapRigidBodyToRenderGeometry, ActiveMarker>(world, id);
			/*Common*/
			editComponent.template operator() < Position > (world, id);
			editComponent.template operator() < MapRigidBodyToRenderGeometry > (world, id);
			editComponent.template operator() < ActiveMarker > (world, id);
			/*Behaviour*/
			editComponent.template operator() < Behaviour > (world, id);
			editComponent.template operator() < LuaScript > (world, id);
			editComponent.template operator() < LuaContext > (world, id);
			/*Debug*/
			editComponent.template operator() < DebugInfo > (world, id);
			editComponent.template operator() < ImGuiState > (world, id);
			editComponent.template operator() < ECSInspector > (world, id);
			editComponent.template operator() < MainMenuBar > (world, id);
			editComponent.template operator() < FramesCounter > (world, id);
			//editComponent.template operator() < > (world, id);
			/*Physics*/
			//editComponent.template operator() < > (world, id);
			editComponent.template operator() < DynamicRigidBodyBox > (world, id);
			editComponent.template operator() < DynamicRigidBodyCustomMeshShape > (world, id);
			editComponent.template operator() < StaticRigidBodyCustomMeshShape > (world, id);
			/*Render*/
			editComponent.template operator() < ImmutableRenderGeometryObj > (world, id);
			editComponent.template operator() < ImmutableRenderGeometry > (world, id);
			editComponent.template operator() < GeometryFile > (world, id);
			editComponent.template operator() < Camera > (world, id);
			editComponent.template operator() < AttachedCamera > (world, id);
			editComponent.template operator() < PointLight > (world, id);
			editComponent.template operator() < SkinnedGeometry > (world, id);
			editComponent.template operator() < Mesh > (world, id);
			/*Resources*/
			editComponent.template operator() < LoadResourceRequest > (world, id);
			editComponent.template operator() < Resource > (world, id);
			/*UI*/
			editComponent.template operator() < HandleKeyboardInputMarker > (world, id);
			editComponent.template operator() < KeyboardInput > (world, id);
			editComponent.template operator() < HandleMouseInputMarker > (world, id);
			editComponent.template operator() < MouseInput > (world, id);
			editComponent.template operator() < Window > (world, id);
			ImGui::PopID();

			auto& state = GetCreateState(id);
			ImGui::SeparatorText("Add component");
			ImGui::Indent(20.0f);
			const char* items[] = {
				Behaviour::GetName(),
				Position::GetName(),
				MapRigidBodyToRenderGeometry::GetName(),
				ActiveMarker::GetName(),
				DebugInfo::GetName(),
				ImGuiState::GetName(),
				ECSInspector::GetName(),
				MainMenuBar::GetName(),
				FramesCounter::GetName(),
				DynamicRigidBodyBox::GetName(),
				DynamicRigidBodyCustomMeshShape::GetName(),
				StaticRigidBodyCustomMeshShape::GetName(),
				ImmutableRenderGeometryObj::GetName(),
				ImmutableRenderGeometry::GetName(),
				Mesh::GetName(),
				Camera::GetName(),
				AttachedCamera::GetName(),
				PointLight::GetName(),
				SkinnedGeometry::GetName(),
				LoadResourceRequest::GetName(),
				Resource::GetName(),
				HandleKeyboardInputMarker::GetName(),
				KeyboardInput::GetName(),
				Window::GetName()

			};
			ImGui::Combo("", &state.currentAddComponentIndex_, items, std::size(items));

			ImGui::PushID("Add");

			const std::string currentComponent = items[state.currentAddComponentIndex_];

			if (currentComponent == HandleKeyboardInputMarker::GetName()) {
				Add<HandleKeyboardInputMarker>(world, id);
			}
			if (currentComponent == Behaviour::GetName()) {
				Add<Behaviour>(world, id);
			}
			if (currentComponent == ActiveMarker::GetName()) {
				Add<ActiveMarker>(world, id);
			}
			if (currentComponent == Position::GetName()) {
				Add<Position>(world, id);
			}
			if (currentComponent == ImmutableRenderGeometryObj::GetName()) {
				Add<ImmutableRenderGeometryObj>(world, id);
			}
			if (currentComponent == ImmutableRenderGeometry::GetName()) {
				Add<ImmutableRenderGeometry>(world, id);
			}
			if (currentComponent == Camera::GetName()) {
				Add<Camera>(world, id);
			}
			ImGui::PopID();
			ImGui::Unindent(20.0f);
			ImGui::Unindent(20.0f);
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

	void ECSInspectorSystem::Update(ECS::World* world, ECS::Entity::Id entityId, ECS::Entity::Id secondEntityId) {
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