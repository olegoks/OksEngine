
#include <Debug/OksEngine.CollectEntitiesInfo.hpp>

#include <OksEngine.Components.hpp>

namespace OksEngine {


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
			//editComponent.template operator() < DynamicRigidBodyBox > (world, id);
			editComponent.template operator() < DynamicRigidBodyCustomMeshShape > (world, id);
			editComponent.template operator() < Material > (world, id);
			editComponent.template operator() < Mass > (world, id);
			editComponent.template operator() < DynamicRigidBody > (world, id);
			//editComponent.template operator() < StaticRigidBodyCustomMeshShape > (world, id);
			/*Render*/
			editComponent.template operator() < ImmutableRenderGeometry > (world, id);
			editComponent.template operator() < GeometryFile > (world, id);
			editComponent.template operator() < Camera > (world, id);
			editComponent.template operator() < AttachedCamera > (world, id);
			//editComponent.template operator() < PointLight > (world, id);
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
				//DynamicRigidBodyBox::GetName(),
				DynamicRigidBodyCustomMeshShape::GetName(),
				//StaticRigidBodyCustomMeshShape::GetName(),
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


}