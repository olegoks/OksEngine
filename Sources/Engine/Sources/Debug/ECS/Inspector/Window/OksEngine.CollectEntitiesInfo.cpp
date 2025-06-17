
#include <Debug/ECS/Inspector/Window/auto_OksEngine.CollectEntitiesInfo.hpp>

#include <Common/auto_OksEngine.Position2D.hpp>

namespace OksEngine {

	void CollectEntitiesInfo::Update(ECS2::Entity::Id entityId){

	}

	//void CollectEntitiesInfo::BeforeUpdate(ECS::World* world) {
	//	bool isOpen = true;
	//	ImGui::Begin("ECS World Info", &isOpen, 0);

	//	ImGui::TextDisabled("Entities number: %d", world->GetEntitiesNumber());
	//	ImGui::TextDisabled("Entities:");
	//	if (ImGui::Button("Add entity")) {
	//		ECS::Entity::Id id = world->CreateEntity();
	//	}
	//	ImGui::Separator();
	//}

	//void CollectEntitiesInfo::Update(
	//	ECS2::Entity::Id entityId) {

	//	//if (IsComponentExist<Position3D>(entityId)) {
	//	//	//auto* position3D = GetComponent<Position3D>(entityId);
	//	//	//EditPosition3D(position3D);
	//	//}

	//	//ImGui::Begin("ECSInspector");
	//	//if (IsComponentExist<Position2D>(entityId)) {
	//	//	EditPosition2D(GetComponent<Position2D>(entityId));
	//	//}
	//	//
	//	//ImGui::End();


	//}

	//void CollectEntitiesInfo::ShowEntityInfo(ECS::World* world, ECS::Entity::Id id)
	//{
	//	const std::string idString = std::to_string(id);

	//	ImGui::PushID(idString.c_str());


	//	if (ImGui::CollapsingHeader(("Id: " + idString).c_str())) {
	//		ImGui::Indent(20.0f);
	//		ECS::Entity::Filter entityFilter = world->GetEntityFilter(id);
	//		auto editComponent = [this]<class ComponentType>(ECS::World * world, ECS::Entity::Id id) {

	//			bool isExist = world->IsComponentExist<ComponentType>(id);
	//			if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist)) {
	//				ComponentType* component = world->GetComponent<ComponentType>(id);
	//				Edit<ComponentType>(component);
	//				ImGui::Spacing();
	//			}
	//			if (!isExist) {
	//				if (world->IsComponentExist<ComponentType>(id)) {
	//					world->RemoveComponent<ComponentType>(id);
	//				}
	//			}
	//		};

	//		ImGui::PushID("Edit");


	//		//callProcessForEach<Position3D, MapRigidBodyToRenderGeometry, ActiveMarker>(world, id);
	//		/*Common*/
	//		editComponent.template operator() < Position3D > (world, id);
	//		editComponent.template operator() < Position2D > (world, id);
	//		editComponent.template operator() < Scale2D > (world, id);

	//		editComponent.template operator() < Direction3D > (world, id);
	//		editComponent.template operator() < Up > (world, id);
	//		editComponent.template operator() < Width > (world, id);
	//		editComponent.template operator() < Height > (world, id);
	//		editComponent.template operator() < Rotation3D > (world, id);
	//		editComponent.template operator() < MapRigidBodyToRenderGeometry > (world, id);
	//		editComponent.template operator() < Active > (world, id);
	//		editComponent.template operator() < Name > (world, id);
	//		editComponent.template operator() < Text > (world, id);
	//		editComponent.template operator() < BinaryData > (world, id);
	//		editComponent.template operator() < Completed > (world, id);
	//		editComponent.template operator() < Clock > (world, id);

	//		//Sound

	//		editComponent.template operator() < SoundInfo > (world, id);
	//		editComponent.template operator() < LoadSoundRequest > (world, id);
	//		editComponent.template operator() < Sound > (world, id);
	//		editComponent.template operator() < StartPlaySound > (world, id);
	//		editComponent.template operator() < SoundInProgress > (world, id);

	//		/*Behaviour*/
	//		editComponent.template operator() < Behaviour > (world, id);
	//		editComponent.template operator() < LuaScript > (world, id);
	//		editComponent.template operator() < LuaContext > (world, id);
	//		/*Debug*/
	//		editComponent.template operator() < DebugInfo > (world, id);
	//		editComponent.template operator() < ImGuiState > (world, id);
	//		editComponent.template operator() < ECSInspector > (world, id);
	//		editComponent.template operator() < MainMenuBar > (world, id);
	//		editComponent.template operator() < FramesCounter > (world, id);
	//		/*Physics*/
	//		editComponent.template operator() < DynamicRigidBodyCustomMeshShape > (world, id);
	//		editComponent.template operator() < PhysicsShape > (world, id);
	//		editComponent.template operator() < Material > (world, id);
	//		editComponent.template operator() < Mass > (world, id);
	//		editComponent.template operator() < DynamicRigidBody > (world, id);
	//		/*Render*/
	//		editComponent.template operator() < ImmutableRenderGeometry > (world, id);
	//		editComponent.template operator() < LoadMtlRequest > (world, id);
	//		editComponent.template operator() < LoadObjRequest > (world, id);
	//		editComponent.template operator() < LoadFbxRequest > (world, id);
	//		editComponent.template operator() < FbxEntity > (world, id);
	//		editComponent.template operator() < LoadTextureRequest > (world, id);
	//		editComponent.template operator() < ObjEntity > (world, id);
	//		editComponent.template operator() < MtlEntity > (world, id);

	//		editComponent.template operator() < Node > (world, id);

	//		editComponent.template operator() < Animation > (world, id);
	//		editComponent.template operator() < StartAnimation > (world, id);
	//		editComponent.template operator() < AnimationInProcess > (world, id);
	//		editComponent.template operator() < AnimationEnded> (world, id);
	//		editComponent.template operator() < RunningAnimationState > (world, id);

	//		editComponent.template operator() < DriverTransform3D > (world, id);

	//		editComponent.template operator() < MeshEntity > (world, id);
	//		if (world->IsComponentExist<MeshEntity>(id)) {
	//			auto meshEntity = world->GetComponent<MeshEntity>(id);
	//			ImGui::Indent(20.0f);
	//			ShowEntityInfo(world, meshEntity->id_);
	//			ImGui::Unindent(20.0f);
	//		}

	//		if (world->IsComponentExist<MtlEntity>(id)) {
	//			auto mtlEntity = world->GetComponent<MtlEntity>(id);
	//			ImGui::Indent(20.0f);
	//			ShowEntityInfo(world, mtlEntity->id_);
	//			ImGui::Unindent(20.0f);
	//		}
	//		editComponent.template operator() < ObjEntity > (world, id);
	//		if (world->IsComponentExist<ObjEntity>(id)) {
	//			auto objEntity = world->GetComponent<ObjEntity>(id);
	//			ImGui::Indent(20.0f);
	//			ShowEntityInfo(world, objEntity->id_);
	//			ImGui::Unindent(20.0f);
	//		}
	//		editComponent.template operator() < ResourceEntity > (world, id);
	//		if (world->IsComponentExist<ResourceEntity>(id)) {
	//			auto resourceEntity = world->GetComponent<ResourceEntity>(id);
	//			ImGui::Indent(20.0f);
	//			ShowEntityInfo(world, resourceEntity->id_);
	//			ImGui::Unindent(20.0f);
	//		}
	//		editComponent.template operator() < ModelEntity > (world, id);
	//		if (world->IsComponentExist<ModelEntity>(id)) {
	//			auto modelEntity = world->GetComponent<ModelEntity>(id);
	//			ImGui::Indent(20.0f);
	//			ShowEntityInfo(world, modelEntity->id_);
	//			ImGui::Unindent(20.0f);
	//		}
	//		editComponent.template operator() < Mesh2 > (world, id);
	//		editComponent.template operator() < ChildEntities > (world, id);
	//		if (world->IsComponentExist<ChildEntities>(id)) {
	//			auto childEntity = world->GetComponent<ChildEntities>(id);
	//			ImGui::Indent(20.0f);
	//			for (ECS::Entity::Id id : childEntity->entitiesIds_) {
	//				ShowEntityInfo(world, id);
	//			}
	//			ImGui::Unindent(20.0f);
	//			
	//		}

	//		editComponent.template operator() < Meshes > (world, id);
	//		if (world->IsComponentExist<Meshes>(id)) {
	//			auto meshes = world->GetComponent<Meshes>(id);
	//			ImGui::Indent(20.0f);
	//			for (ECS::Entity::Id id : meshes->entitiesIds_) {
	//				ShowEntityInfo(world, id);
	//			}
	//			ImGui::Unindent(20.0f);

	//		}

	//		editComponent.template operator() < Vertices3D > (world, id);
	//		editComponent.template operator() < DriverVertexBuffer > (world, id);
	//		editComponent.template operator() < Normals > (world, id);
	//		editComponent.template operator() < Colors > (world, id);
	//		editComponent.template operator() < UVs > (world, id);
	//		editComponent.template operator() < Indices > (world, id);
	//		editComponent.template operator() < DriverIndexBuffer > (world, id);
	//		editComponent.template operator() < TextureInfo > (world, id);
	//		editComponent.template operator() < Texture > (world, id);
	//		editComponent.template operator() < DriverTexture > (world, id);
	//		editComponent.template operator() < DriverMesh > (world, id);

	//		editComponent.template operator() < GeometryDescriptionFileEntity > (world, id);
	//		editComponent.template operator() < LoadGeometryDescriptionFileRequest > (world, id);
	//		editComponent.template operator() < Camera > (world, id);
	//		//editComponent.template operator() < PointLight > (world, id);
	//		editComponent.template operator() < SkinnedGeometry > (world, id);
	//		/*Resources*/
	//		editComponent.template operator() < LoadResourceRequest > (world, id);
	//		editComponent.template operator() < Resource > (world, id);
	//		editComponent.template operator() < AsyncTask > (world, id);
	//		editComponent.template operator() < WaitingForResourceLoading > (world, id);
	//		editComponent.template operator() < ResourceWasLoadedEarly > (world, id);
	//		/*UI*/
	//		editComponent.template operator() < HandleKeyboardInputMarker > (world, id);
	//		editComponent.template operator() < KeyboardInput > (world, id);
	//		editComponent.template operator() < HandleMouseInputMarker > (world, id);
	//		editComponent.template operator() < MouseInput > (world, id);
	//		editComponent.template operator() < Window > (world, id);
	//		ImGui::PopID();

	//		auto& state = GetCreateState(id);
	//		ImGui::SeparatorText("Add component");
	//		ImGui::Indent(20.0f);
	//		const char* items[] = {
	//			StartPlaySound::GetName(),
	//			SoundInfo::GetName(),
	//			Behaviour::GetName(),
	//			Position3D::GetName(),
	//			MapRigidBodyToRenderGeometry::GetName(),
	//			Active::GetName(),
	//			DebugInfo::GetName(),
	//			ImGuiState::GetName(),
	//			ECSInspector::GetName(),
	//			MainMenuBar::GetName(),
	//			FramesCounter::GetName(),
	//			//DynamicRigidBodyBox::GetName(),
	//			DynamicRigidBodyCustomMeshShape::GetName(),
	//			//StaticRigidBodyCustomMeshShape::GetName(),
	//			ImmutableRenderGeometry::GetName(),
	//			Camera::GetName(),
	//			//PointLight::GetName(),
	//			SkinnedGeometry::GetName(),
	//			LoadResourceRequest::GetName(),
	//			Resource::GetName(),
	//			HandleKeyboardInputMarker::GetName(),
	//			KeyboardInput::GetName(),
	//			Window::GetName(),
	//			StartAnimation::GetName()

	//		};
	//		ImGui::Combo("", &state.currentAddComponentIndex_, items, std::size(items));

	//		ImGui::PushID("Add");

	//		const std::string currentComponent = items[state.currentAddComponentIndex_];
	//		if (currentComponent == StartPlaySound::GetName()) {
	//			Add<StartPlaySound>(world, id);
	//		}
	//		if (currentComponent == SoundInfo::GetName()) {
	//			Add<SoundInfo>(world, id);
	//		}
	//		if (currentComponent == HandleKeyboardInputMarker::GetName()) {
	//			Add<HandleKeyboardInputMarker>(world, id);
	//		}
	//		if (currentComponent == Behaviour::GetName()) {
	//			Add<Behaviour>(world, id);
	//		}
	//		if (currentComponent == StartAnimation::GetName()) {
	//			Add<StartAnimation>(world, id);
	//		}
	//		if (currentComponent == Active::GetName()) {
	//			Add<Active>(world, id);
	//		}
	//		if (currentComponent == Position3D::GetName()) {
	//			Add<Position3D>(world, id);
	//		}
	//		if (currentComponent == ImmutableRenderGeometry::GetName()) {
	//			Add<ImmutableRenderGeometry>(world, id);
	//		}
	//		if (currentComponent == Camera::GetName()) {
	//			Add<Camera>(world, id);
	//		}
	//		ImGui::PopID();
	//		ImGui::Unindent(20.0f);
	//		ImGui::Unindent(20.0f);
	//	}
	//	ImGui::Separator();
	//	ImGui::PopID();
	//}

	//void CollectEntitiesInfo::AfterUpdate(ECS::World* world)
	//{
	//	ImGui::End();
	//}


}