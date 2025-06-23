
#include <Debug/ECS/Inspector/Window/auto_OksEngine.ShowEntitiesInfo.hpp>

#include <Common/auto_OksEngine.Position2D.hpp>

#include <auto_OksEngine.ECS.hpp>
#include <auto_OksEngine.EditEntity.hpp>

//namespace ImGui {
//
//	class ScopedId {
//	public:
//		ScopedId(const std::string& stringId) :
//			stringId_{ stringId } {
//			ImGui::PushID(stringId_.c_str());
//		}
//		~ScopedId() {
//			ImGui::PopID();
//		}
//		std::string stringId_;
//	};
//	class ScopedIndent {
//	public:
//		ScopedIndent(float indent) :
//			indent_{ indent } {
//			ImGui::Indent(indent_);
//		}
//		~ScopedIndent() {
//			ImGui::Unindent(indent_);
//		}
//		float indent_;
//	};
//
//}
namespace OksEngine {

	//template <typename T, typename FieldType>
	//concept HasSpecificField =
	//	requires(T t) {
	//		{ t.FieldName } -> std::convertible_to<FieldType>;
	//};

	//void EditEntity(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id entityId);

	//template<class ...Components>
	//void Foo(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id entityId, auto&& func) {

	//	auto editComponent = [&]<class ComponentType>() {
	//		if (!HasSpecificField<ComponentType, ECS2::Entity::Id>) {
	//			EditEntity(world, entityId);
	//		};
	//		func.template operator() < ComponentType > (world, entityId);
	//		};

	//	(..., editComponent.template operator()<Components>());
	//}

	//void EditEntity(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id entityId) {
	//	const std::string idString = std::to_string(entityId);

	//	ImGui::ScopedId entityImGuiId(idString);
	//	if (ImGui::CollapsingHeader(("Id: " + idString).c_str())) {

	//		ImGui::ScopedIndent indent(20.f);

	//		auto editComponent = []<class ComponentType>(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) {

	//			bool isExist = world->IsComponentExist<ComponentType>(id);
	//			if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist)) {
	//				ComponentType* component = world->GetComponent<ComponentType>(id);
	//				Edit<ComponentType>(component);
	//				ImGui::Spacing();
	//			}

	//		};
	//		{
	//			ImGui::ScopedId componentsImGuiId("Edit");

	//			editComponent.template operator() < Position3D > (world, entityId);
	//			editComponent.template operator() < Rotation2D > (world, entityId);
	//			editComponent.template operator() < ChildEntity > (world, entityId);
	//			if (world->IsComponentExist<ChildEntity>(entityId)) {
	//				auto childEntity = world->GetComponent<ChildEntity>(entityId);
	//				ImGui::Indent(20.0f);
	//				EditEntity(world, childEntity->childEntityId_);
	//				ImGui::Unindent(20.0f);
	//			}
	//			///...

	//			ImGui::SeparatorText("Add component");
	//			ImGui::ScopedIndent addComponentIndent(20.0f);

	//			const char* items[] = {
	//				ChildEntity::GetName(),
	//				StartPlaySound::GetName(),
	//				SoundInfo::GetName(),
	//				Behaviour::GetName(),
	//				Position3D::GetName(),
	//				MapRigidBodyToRenderGeometry::GetName(),
	//				Active::GetName(),
	//				DebugInfo::GetName(),
	//				ImGuiState::GetName(),
	//				MainMenuBar::GetName(),
	//				FramesCounter::GetName(),
	//				//DynamicRigidBodyBox::GetName(),
	//				DynamicRigidBodyCustomMeshShape::GetName(),
	//				//StaticRigidBodyCustomMeshShape::GetName(),
	//				ImmutableRenderGeometry::GetName(),
	//				Camera::GetName(),
	//				//PointLight::GetName(),
	//				//SkinnedGeometry::GetName(),
	//				LoadResourceRequest::GetName(),
	//				Resource::GetName(),
	//				//HandleKeyboardInputMarker::GetName(),
	//				KeyboardInput::GetName(),
	//				Window::GetName(),
	//				StartAnimation::GetName()
	//				//...
	//			};

	//			static int addComponentIndex = 0;
	//			ImGui::Combo("", &addComponentIndex, items, std::size(items));
	//			ImGui::ScopedId addComponentId("Add");

	//			const std::string currentComponent = items[addComponentIndex];
	//			if (currentComponent == ChildEntity::GetName()) {
	//				AddChildEntity(world.get(), entityId);
	//			}
	//		}
	//		//if (world_->IsComponentExist<MeshEntity>(entityId)) {
	//		//	auto meshEntity = world_->GetComponent<MeshEntity>(entityId);
	//		//	ImGui::Indent(20.0f);
	//		//	ShowEntityInfo(world_, meshEntity->entityId_);
	//		//	ImGui::Unindent(20.0f);
	//		//}

	//		//if (world_->IsComponentExist<MtlEntity>(entityId)) {
	//		//	auto mtlEntity = world_->GetComponent<MtlEntity>(entityId);
	//		//	ImGui::Indent(20.0f);
	//		//	ShowEntityInfo(world_, mtlEntity->entityId_);
	//		//	ImGui::Unindent(20.0f);
	//		//}
	//		//editComponent.template operator() < ObjEntity > (world_, entityId);
	//		//if (world_->IsComponentExist<ObjEntity>(entityId)) {
	//		//	auto objEntity = world_->GetComponent<ObjEntity>(entityId);
	//		//	ImGui::Indent(20.0f);
	//		//	ShowEntityInfo(world_, objEntity->entityId_);
	//		//	ImGui::Unindent(20.0f);
	//		//}
	//		//editComponent.template operator() < ResourceEntity > (world_, entityId);
	//		//if (world_->IsComponentExist<ResourceEntity>(entityId)) {
	//		//	auto resourceEntity = world_->GetComponent<ResourceEntity>(entityId);
	//		//	ImGui::Indent(20.0f);
	//		//	ShowEntityInfo(world_, resourceEntity->entityId_);
	//		//	ImGui::Unindent(20.0f);
	//		//}
	//		//editComponent.template operator() < ModelEntity > (world_, entityId);
	//		//if (world_->IsComponentExist<ModelEntity>(entityId)) {
	//		//	auto modelEntity = world_->GetComponent<ModelEntity>(entityId);
	//		//	ImGui::Indent(20.0f);
	//		//	ShowEntityInfo(world_, modelEntity->entityId_);
	//		//	ImGui::Unindent(20.0f);
	//		//}
	//		//editComponent.template operator() < Mesh2 > (world_, entityId);
	//		//editComponent.template operator() < ChildEntities > (world_, entityId);
	//		//if (world_->IsComponentExist<ChildEntities>(entityId)) {
	//		//	auto childEntity = world_->GetComponent<ChildEntities>(entityId);
	//		//	ImGui::Indent(20.0f);
	//		//	for (ECS::Entity::entityId entityId : childEntity->entitiesentityIds_) {
	//		//		ShowEntityInfo(world_, entityId);
	//		//	}
	//		//	ImGui::Unindent(20.0f);

	//		//}

	//		//editComponent.template operator() < Meshes > (world_, entityId);
	//		//if (world_->IsComponentExist<Meshes>(entityId)) {
	//		//	auto meshes = world_->GetComponent<Meshes>(entityId);
	//		//	ImGui::ScopedIndent indent(20.0f);
	//		//	for (ECS::Entity::entityId id : meshes->entitiesIds_) {
	//		//		ShowEntityInfo(world_, id);
	//		//	}

	//		//}

	//	}
	//	ImGui::Separator();
	//}

	void ShowEntitiesInfo::Update(
		ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0,
		ECS2::Entity::Id entity1id) {

		EditEntity(world_, entity1id);
		//const std::string idString = std::to_string(entity1id);

		//ImGui::PushID(idString.c_str());


		//if (ImGui::CollapsingHeader(("Id: " + idString).c_str())) {
		//	ImGui::Indent(20.0f);
		//	//ECS::Entity::Filter entityFilter = world_->GetEntityFilter(id);
		//	auto editComponent = [this]<class ComponentType>(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) {

		//		bool isExist = world_->IsComponentExist<ComponentType>(id);
		//		if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist)) {
		//			ComponentType* component = GetComponent<ComponentType>(id);
		//			Edit<ComponentType>(component);
		//			ImGui::Spacing();
		//		}
		//		if (!isExist) {
		//			if (IsComponentExist<ComponentType>(id)) {
		//				RemoveComponent<ComponentType>(id);
		//			}
		//		}
		//	};

		//	ImGui::PushID("Edit");


		//	//callProcessForEach<Position3D, MapRigidBodyToRenderGeometry, ActiveMarker>(world_, id);
		//	/*Common*/
		//	editComponent.template operator() < Position3D > (world_, entity1id);
		//	editComponent.template operator() < Position2D > (world_, entity1id);
		//	editComponent.template operator() < Scale2D > (world_, entity1id);

		//	editComponent.template operator() < Direction3D > (world_, entity1id);
		//	editComponent.template operator() < Width > (world_, entity1id);
		//	editComponent.template operator() < Height > (world_, entity1id);
		//	editComponent.template operator() < Rotation3D > (world_, entity1id);
		//	editComponent.template operator() < MapRigidBodyToRenderGeometry > (world_, entity1id);
		//	editComponent.template operator() < Active > (world_, entity1id);
		//	editComponent.template operator() < Name > (world_, entity1id);
		//	editComponent.template operator() < Text > (world_, entity1id);
		//	editComponent.template operator() < BinaryData > (world_, entity1id);
		//	editComponent.template operator() < Completed > (world_, entity1id);
		//	editComponent.template operator() < Clock > (world_, entity1id);

		//	//Sound

		//	editComponent.template operator() < SoundInfo > (world_, entity1id);
		//	editComponent.template operator() < LoadSoundRequest > (world_, entity1id);
		//	editComponent.template operator() < Sound > (world_, entity1id);
		//	editComponent.template operator() < StartPlaySound > (world_, entity1id);
		//	editComponent.template operator() < SoundInProgress > (world_, entity1id);

		//	/*Behaviour*/
		//	editComponent.template operator() < Behaviour > (world_, entity1id);
		//	editComponent.template operator() < LuaScript > (world_, entity1id);
		//	editComponent.template operator() < LuaContext > (world_, entity1id);
		//	/*Debug*/
		//	editComponent.template operator() < DebugInfo > (world_, entity1id);
		//	editComponent.template operator() < ImGuiState > (world_, entity1id);
		//	editComponent.template operator() < MainMenuBar > (world_, entity1id);
		//	editComponent.template operator() < FramesCounter > (world_, entity1id);
		//	/*Physics*/
		//	//editComponent.template operator() < DynamicRigentity1idBodyCustomMeshShape > (world_, entity1id);
		//	editComponent.template operator() < PhysicsShape > (world_, entity1id);
		//	editComponent.template operator() < Material > (world_, entity1id);
		//	editComponent.template operator() < Mass > (world_, entity1id);
		//	//editComponent.template operator() < DynamicRigentity1idBody > (world_, entity1id);
		//	/*Render*/
		//	editComponent.template operator() < ImmutableRenderGeometry > (world_, entity1id);
		//	editComponent.template operator() < LoadMtlRequest > (world_, entity1id);
		//	editComponent.template operator() < LoadObjRequest > (world_, entity1id);
		//	editComponent.template operator() < LoadFbxRequest > (world_, entity1id);
		//	editComponent.template operator() < FbxEntity > (world_, entity1id);
		//	editComponent.template operator() < LoadTextureRequest > (world_, entity1id);
		//	editComponent.template operator() < ObjEntity > (world_, entity1id);
		//	editComponent.template operator() < MtlEntity > (world_, entity1id);

		//	editComponent.template operator() < Node > (world_, entity1id);

		//	editComponent.template operator() < Animation > (world_, entity1id);
		//	editComponent.template operator() < StartAnimation > (world_, entity1id);
		//	editComponent.template operator() < AnimationInProcess > (world_, entity1id);
		//	editComponent.template operator() < AnimationEnded > (world_, entity1id);
		//	editComponent.template operator() < RunningAnimationState > (world_, entity1id);

		//	editComponent.template operator() < DriverTransform3D > (world_, entity1id);

		//	editComponent.template operator() < MeshEntity > (world_, entity1id);
			//if (world_->IsComponentExist<MeshEntity>(entityId)) {
			//	auto meshEntity = world_->GetComponent<MeshEntity>(entityId);
			//	ImGui::Indent(20.0f);
			//	ShowEntityInfo(world_, meshEntity->entityId_);
			//	ImGui::Unindent(20.0f);
			//}

			//if (world_->IsComponentExist<MtlEntity>(entityId)) {
			//	auto mtlEntity = world_->GetComponent<MtlEntity>(entityId);
			//	ImGui::Indent(20.0f);
			//	ShowEntityInfo(world_, mtlEntity->entityId_);
			//	ImGui::Unindent(20.0f);
			//}
			//editComponent.template operator() < ObjEntity > (world_, entityId);
			//if (world_->IsComponentExist<ObjEntity>(entityId)) {
			//	auto objEntity = world_->GetComponent<ObjEntity>(entityId);
			//	ImGui::Indent(20.0f);
			//	ShowEntityInfo(world_, objEntity->entityId_);
			//	ImGui::Unindent(20.0f);
			//}
			//editComponent.template operator() < ResourceEntity > (world_, entityId);
			//if (world_->IsComponentExist<ResourceEntity>(entityId)) {
			//	auto resourceEntity = world_->GetComponent<ResourceEntity>(entityId);
			//	ImGui::Indent(20.0f);
			//	ShowEntityInfo(world_, resourceEntity->entityId_);
			//	ImGui::Unindent(20.0f);
			//}
			//editComponent.template operator() < ModelEntity > (world_, entityId);
			//if (world_->IsComponentExist<ModelEntity>(entityId)) {
			//	auto modelEntity = world_->GetComponent<ModelEntity>(entityId);
			//	ImGui::Indent(20.0f);
			//	ShowEntityInfo(world_, modelEntity->entityId_);
			//	ImGui::Unindent(20.0f);
			//}
			//editComponent.template operator() < Mesh2 > (world_, entityId);
			//editComponent.template operator() < ChildEntities > (world_, entityId);
			//if (world_->IsComponentExist<ChildEntities>(entityId)) {
			//	auto childEntity = world_->GetComponent<ChildEntities>(entityId);
			//	ImGui::Indent(20.0f);
			//	for (ECS::Entity::entityId entityId : childEntity->entitiesentityIds_) {
			//		ShowEntityInfo(world_, entityId);
			//	}
			//	ImGui::Unindent(20.0f);

			//}

			//editComponent.template operator() < Meshes > (world_, entityId);
			//if (world_->IsComponentExist<Meshes>(entityId)) {
			//	auto meshes = world_->GetComponent<Meshes>(entityId);
			//	ImGui::Indent(20.0f);
			//	for (ECS::Entity::entityId id : meshes->entitiesIds_) {
			//		ShowEntityInfo(world_, id);
			//	}
			//	ImGui::Unindent(20.0f);

			//}

		//	editComponent.template operator() < Vertices3D > (world_, entity1id);
		//	editComponent.template operator() < DriverVertexBuffer > (world_, entity1id);
		//	editComponent.template operator() < Normals > (world_, entity1id);
		//	editComponent.template operator() < Colors > (world_, entity1id);
		//	editComponent.template operator() < UVs > (world_, entity1id);
		//	editComponent.template operator() < Indices > (world_, entity1id);
		//	editComponent.template operator() < DriverIndexBuffer > (world_, entity1id);
		//	editComponent.template operator() < TextureInfo > (world_, entity1id);
		//	editComponent.template operator() < Texture > (world_, entity1id);
		//	editComponent.template operator() < DriverTexture > (world_, entity1id);
		//	editComponent.template operator() < DriverMesh > (world_, entity1id);

		//	//editComponent.template operator() < GeometryDescriptionFileEntity > (world_, id);
		//	//editComponent.template operator() < LoadGeometryDescriptionFileRequest > (world_, id);
		//	editComponent.template operator() < Camera > (world_, entity1id);
		//	//editComponent.template operator() < PointLight > (world_, id);
		//	//editComponent.template operator() < SkinnedGeometry > (world_, id);
		//	/*Resources*/
		//	editComponent.template operator() < LoadResourceRequest > (world_, entity1id);
		//	editComponent.template operator() < Resource > (world_, entity1id);
		//	editComponent.template operator() < AsyncTask > (world_, entity1id);
		//	editComponent.template operator() < WaitingForResourceLoading > (world_, entity1id);
		//	//editComponent.template operator() < ResourceWasLoadedEarly > (world_, id);
		//	/*UI*/
		//	//editComponent.template operator() < HandleKeyboardInputMarker > (world_, id);
		//	editComponent.template operator() < KeyboardInput > (world_, entity1id);
		//	//editComponent.template operator() < HandleMouseInputMarker > (world_, id);
		//	editComponent.template operator() < MouseInput > (world_, entity1id);
		//	editComponent.template operator() < Window > (world_, entity1id);
		//	ImGui::PopID();

		//	//auto& state = GetCreateState(id);
		//	ImGui::SeparatorText("Add component");
		//	ImGui::Indent(20.0f);
		//	const char* items[] = {
		//		StartPlaySound::GetName(),
		//		SoundInfo::GetName(),
		//		Behaviour::GetName(),
		//		Position3D::GetName(),
		//		MapRigidBodyToRenderGeometry::GetName(),
		//		Active::GetName(),
		//		DebugInfo::GetName(),
		//		ImGuiState::GetName(),
		//		MainMenuBar::GetName(),
		//		FramesCounter::GetName(),
		//		//DynamicRigidBodyBox::GetName(),
		//		DynamicRigidBodyCustomMeshShape::GetName(),
		//		//StaticRigidBodyCustomMeshShape::GetName(),
		//		ImmutableRenderGeometry::GetName(),
		//		Camera::GetName(),
		//		//PointLight::GetName(),
		//		//SkinnedGeometry::GetName(),
		//		LoadResourceRequest::GetName(),
		//		Resource::GetName(),
		//		//HandleKeyboardInputMarker::GetName(),
		//		KeyboardInput::GetName(),
		//		Window::GetName(),
		//		StartAnimation::GetName()

		//	};
		//	//ImGui::Combo("", &state.currentAddComponentIndex_, items, std::size(items));

		//	ImGui::PushID("Add");

		//	//const std::string currentComponent = items[state.currentAddComponentIndex_];
		//	//if (currentComponent == StartPlaySound::GetName()) {
		//	//	AddStartPlaySound(world_, id);
		//	//}
		//	//if (currentComponent == SoundInfo::GetName()) {
		//	//	Add<SoundInfo>(world_, id);
		//	//}
		//	//if (currentComponent == HandleKeyboardInputMarker::GetName()) {
		//	//	Add<HandleKeyboardInputMarker>(world_, id);
		//	//}
		//	//if (currentComponent == Behaviour::GetName()) {
		//	//	Add<Behaviour>(world_, id);
		//	//}
		//	//if (currentComponent == StartAnimation::GetName()) {
		//	//	Add<StartAnimation>(world_, id);
		//	//}
		//	//if (currentComponent == Active::GetName()) {
		//	//	Add<Active>(world_, id);
		//	//}
		//	//if (currentComponent == Position3D::GetName()) {
		//	//	Add<Position3D>(world_, id);
		//	//}
		//	//if (currentComponent == ImmutableRenderGeometry::GetName()) {
		//	//	Add<ImmutableRenderGeometry>(world_, id);
		//	//}
		//	//if (currentComponent == Camera::GetName()) {
		//	//	Add<Camera>(world_, id);
		//	//}
		//	ImGui::PopID();
		//	ImGui::Unindent(20.0f);
		//	ImGui::Unindent(20.0f);
		//}
		//ImGui::Separator();
		//ImGui::PopID();
	}

	//void CollectEntitiesInfo::BeforeUpdate(ECS::world_* world) {
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