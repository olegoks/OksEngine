#pragma once
#include <Debug\ECS\Inspector\Window\auto_OksEngine.ECSInspector.hpp>

#include <auto_OksEngine.ECS.hpp>
#include <auto_OksEngine.Utils.hpp>
#include <OksEngine.Utils.hpp>

namespace OksEngine
{
	void BeginECSInspectorWindow::Update(ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		ECSInspectorWindow* eCSInspectorWindow0) {


		bool isOpen = true;

		ImGui::Begin("ECS Inspector",
			&isOpen,
			ImGuiWindowFlags_AlwaysVerticalScrollbar);

		if (!isOpen) {
			RemoveComponent<ECSInspectorWindow>(entity0id);
		}


	};

	void ProcessCreateEntityButton::Update(ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0, const MainMenuBar* mainMenuBar0,
		const ECSMenu* eCSMenu0, const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0) {

		if (ImGui::Button("Create entity")) {

			//TODO: return functionality
			world_->CreateEntity();
		}


	}

	void ProcessEntityFilter::Update(
		ECS2::Entity::Id entity0id,
		const ImGUI::State* imGuiState0,
		const MainMenuBar* mainMenuBar0,
		const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0) {

		bool isFilterExist = IsComponentExist<EntityFilter>(entity0id);
		const bool cachedIsFilterExist = isFilterExist;
		ImGui::Checkbox("Entity filter", &isFilterExist);

		if (isFilterExist != cachedIsFilterExist) {
			if (isFilterExist) {
				//User created entity filter, need to create component.
				ECS2::ComponentsFilter archetypes;
				ECS2::ComponentsFilter includes;
				ECS2::ComponentsFilter excludes;
				CreateComponent<EntityFilter>(entity0id, 0, archetypes, includes, excludes);
			}
			else {
				//User want to remove entity filter.
				RemoveComponent<EntityFilter>(entity0id);
			}
		}
		else {
			if (isFilterExist) {
				//Entity filter exist and user want to use it.

				auto* entityFilter = GetComponent<EntityFilter>(entity0id);

				const char* archetypeNames[] = {
					ARCHETYPES_LIST_NAMES()
				};

				static char currentArchetypeName[256] = { 0 };
				ImGui::InputText("Archetype", currentArchetypeName, 256);

				std::vector<const char*> suitableArchetypeNames;
				suitableArchetypeNames.reserve(sizeof(archetypeNames) / 8);

				for (Common::Index i = 0; i < sizeof(archetypeNames) / 8; i++) {
					if (std::string{ archetypeNames[i] }.starts_with(currentArchetypeName)) {
						suitableArchetypeNames.push_back(archetypeNames[i]);
					}
				}
				static int currentArchetypeIndex = 0;
				const char* currentArchetype = nullptr;
				if (!suitableArchetypeNames.empty()) {
					ImGui::Combo("Archetypes list", &currentArchetypeIndex, suitableArchetypeNames.data(), std::size(suitableArchetypeNames));
					currentArchetype = suitableArchetypeNames[currentArchetypeIndex];
				}
				else {
					ImGui::Combo("Archetypes list", &currentArchetypeIndex, archetypeNames, std::size(archetypeNames));
					currentArchetype = archetypeNames[currentArchetypeIndex];
				}

				if (ImGui::Button("Use archetype")) {
					const ECS2::ComponentsFilter currentArchetypeComponentsFilter = GetArchetypeComponentsFilterByArchetypeName(currentArchetype);
					entityFilter->archetype_ = currentArchetypeComponentsFilter;
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset archetype")) {
					entityFilter->archetype_ = ECS2::ComponentsFilter::GetEmpty();
				}

				if(entityFilter->archetype_.IsSetOneMoreComponent()){
					ImGui::Indent(20.0f);
					ImGui::Text("Selected archetype: ");
					ImGui::SameLine();
					ImGui::Text(GetArchetypeNameByArchetypeComponentsFilter(entityFilter->archetype_));
					ImGui::Unindent(20.0f);
				}

				//Do not use components filter if archetype selected.
				if (!entityFilter->archetype_.IsSetOneMoreComponent()) {

					const char* componentNames[] = {
						COMPONENTS_LIST_NAMES()
					};
					static char currentComponentName[256] = { 0 };
					ImGui::InputText("Component", currentComponentName, 256);

					std::vector<const char*> suitableComponentNames;
					suitableComponentNames.reserve(sizeof(componentNames) / 8);

					for (Common::Index i = 0; i < sizeof(componentNames) / 8; i++) {
						if (std::string{ componentNames[i] }.starts_with(currentComponentName)) {
							suitableComponentNames.push_back(componentNames[i]);
						}
					}

					static int addComponentIndex = 0;
					const char* currentComponent = nullptr;
					if (!suitableComponentNames.empty()) {
						ImGui::Combo("Includes list", &addComponentIndex, suitableComponentNames.data(), std::size(suitableComponentNames));
						currentComponent = suitableComponentNames[addComponentIndex];
					}
					else {
						ImGui::Combo("Includes list", &addComponentIndex, componentNames, std::size(componentNames));
						currentComponent = componentNames[addComponentIndex];
					}


					if (ImGui::Button("Add include")) {
						const ECS2::ComponentTypeId componentTypeIdToInclude = GetComponentTypeIdByName(currentComponent);

						if (!entityFilter->excludes_.IsSet(componentTypeIdToInclude)) {
							//Discard attempt to set the component to excludes and includes. 
							entityFilter->includes_.SetBit(componentTypeIdToInclude);
						}

					}

					if (ImGui::Button("Add exclude")) {

						const ECS2::ComponentTypeId componentTypeIdToInclude = GetComponentTypeIdByName(currentComponent);
						if (!entityFilter->includes_.IsSet(componentTypeIdToInclude)) {
							entityFilter->excludes_.SetBit(componentTypeIdToInclude);
						}

					}

					{
						ImGui::Indent(20.0f);
						ImGui::TextDisabled("Includes:");
						entityFilter->includes_.ForEachSetComponent([&](ECS2::ComponentTypeId componentTypeId, bool isLast) {
							ImGui::TextDisabled(GetComponentNameByTypeId(componentTypeId));
							ImGui::SameLine();
							ImGui::PushID(componentTypeId);
							if (ImGui::Button("Remove")) {
								entityFilter->includes_.RemoveBit(componentTypeId);
							}
							ImGui::PopID();
							});
						ImGui::Unindent(20.0f);
					}
					{
						ImGui::Indent(20.0f);
						ImGui::TextDisabled("Excludes:");
						entityFilter->excludes_.ForEachSetComponent([&](ECS2::ComponentTypeId componentTypeId, bool isLast) {
							ImGui::TextDisabled(GetComponentNameByTypeId(componentTypeId));
							ImGui::SameLine();
							ImGui::PushID(componentTypeId);
							if (ImGui::Button("Remove")) {
								entityFilter->excludes_.RemoveBit(componentTypeId);
							}
							ImGui::PopID();
							});
						ImGui::Unindent(20.0f);
					}
				}
				else {
					//If we use archetype need to reset components filter.
					entityFilter->includes_.ResetAllBits();
					entityFilter->excludes_.ResetAllBits();
				}

				ImGui::Text(std::to_string(entityFilter->selectedEntitiesNumber_).c_str());
				entityFilter->selectedEntitiesNumber_ = 0;
			}
		}


	}

	void ShowEntitiesInfo::Update(ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0, ECS2::Entity::Id entity1id) {

		//EditEntity(world_, entity1id);
		const ECS2::ComponentsFilter componentFilter = GetComponentsFilter(entity1id);
		
		if (IsComponentExist<EntityFilter>(entity0id)) {
			
			auto* requiredFilter = GetComponent<EntityFilter>(entity0id);
			if (requiredFilter->archetype_.IsSetOneMoreComponent() && world_->IsArchetypeEntity(entity1id)) {
				if (requiredFilter->archetype_ != world_->GetArchetypeComponents(entity1id)) {
					return;
				}
			}
			else {

				if (!requiredFilter->includes_.IsSubsetOf(componentFilter) || !requiredFilter->includes_.IsSetOneMoreComponent()) {
					return;
				}

				if (requiredFilter->excludes_.IsSetOneMoreComponent() && requiredFilter->excludes_.IsSubsetOf(componentFilter)) {
					return;
				}
			}
			++(requiredFilter->selectedEntitiesNumber_);
		}

		EditEntity(world_, entity1id);

	};

	void EndECSInspectorWindow::Update(ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0) {


		ImGui::End();

	};



} // namespace OksEngine