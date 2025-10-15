#pragma once
#include <Debug\ECS\Inspector\Window\auto_OksEngine.ECSInspector.hpp>

#include <auto_OksEngine.ECS.hpp>
#include <auto_OksEngine.Utils.hpp>

namespace OksEngine
{
	void BeginECSInspectorWindow::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
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

	void ProcessCreateEntityButton::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0, const MainMenuBar* mainMenuBar0,
		const ECSMenu* eCSMenu0, const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0) {

		if (ImGui::Button("Create entity")) {
			CreateEntity();
		}


	}

	void ProcessEntityFilter::Update(
		ECS2::Entity::Id entity0id,
		const ImGuiState* imGuiState0,
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
				ECS2::ComponentsFilter includes;
				ECS2::ComponentsFilter excludes;
				CreateComponent<EntityFilter>(entity0id, includes, excludes);
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

				const char* items[] = {
					COMPONENTS_LIST_NAMES()
				};
				static char buffer[256] = { 0 };
				ImGui::InputText("Name", buffer, 256);

				std::vector<const char*> suitableNames;
				suitableNames.reserve(sizeof(items) / 8);

				for (Common::Index i = 0; i < sizeof(items) / 8; i ++) {
					if (std::string{ items[i] }.starts_with(buffer)) {
						suitableNames.push_back(items[i]);
					}
				}

				static int addComponentIndex = 0;
				const char* currentComponent = nullptr;
				if (!suitableNames.empty()) {
					ImGui::Combo("Includes list", &addComponentIndex, suitableNames.data(), std::size(suitableNames));
					currentComponent = suitableNames[addComponentIndex];
				}
				else {
					ImGui::Combo("Includes list", &addComponentIndex, items, std::size(items));
					currentComponent = items[addComponentIndex];
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
					entityFilter->includes_.ForEachSetComponent([&](ECS2::ComponentTypeId componentTypeId) {
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
					entityFilter->excludes_.ForEachSetComponent([&](ECS2::ComponentTypeId componentTypeId) {
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
		}


	}

	void ShowEntitiesInfo::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0, ECS2::Entity::Id entity1id) {

		//EditEntity(world_, entity1id);

		if (IsComponentExist<EntityFilter>(entity0id)) {
			const auto* requaredFilter =  GetComponent<EntityFilter>(entity0id);
			const ECS2::ComponentsFilter componentFilter = GetComponentsFilter(entity1id);

			if (entity1id == 93) {
				Common::BreakPointLine();
			}
			if (!requaredFilter->includes_.IsSubsetOf(componentFilter) || !requaredFilter->includes_.IsSetOneMoreComponent()) {
				return;
			}

			if (requaredFilter->excludes_.IsSetOneMoreComponent() && requaredFilter->excludes_.IsSubsetOf(componentFilter)) {
				return;
			}

		}

        const std::string idString = std::to_string(entity1id);
        ImGui::PushID(idString.c_str());
        std::string name;
        if (world_->IsComponentExist<Name>(entity1id))
        {
            auto* nameComponent = world_->GetComponent<Name>(entity1id);
            name = nameComponent->value_;
        }
        if (ImGui::CollapsingHeader(
            ("Id: " + idString + "  " + magic_enum::enum_name(world_->GetEntityType(entity1id)).data() + " " + name)
            .c_str()))
        {
            ImGui::Indent(20.f);
            auto editComponent = []<class ComponentType>(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) {
                bool isExist = world->IsComponentExist<ComponentType>(id);
                if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist))
                {
                    ComponentType* component = world->GetComponent<ComponentType>(id);
                    Edit<ComponentType>(world, component);
                    ImGui::Spacing();
                }
                if (!isExist)
                {
                    if (world->IsComponentExist<ComponentType>(id))
                    {
                        world->RemoveComponent<ComponentType>(id);
                    }
                }
            };
            {
                ImGui::PushID("Edit");
                FOR_EACH_COMPONENT(editComponent.template operator()<, > (world_, entity1id);)
                ImGui::PopID();
                ImGui::SeparatorText("Add component");
                ImGui::Indent(20.0f);
                const char* items[] = {
                    COMPONENTS_LIST_NAMES()
                };



				static char buffer[256] = { 0 };
				ImGui::InputText("Name", buffer, 256);

				std::vector<const char*> suitableNames;
				suitableNames.reserve(sizeof(items) / 8);

				for (Common::Index i = 0; i < sizeof(items) / 8; i++) {
					if (std::string{ items[i] }.starts_with(buffer)) {
						suitableNames.push_back(items[i]);
					}
				}

				static int addComponentIndex = 0;
				const char* currentComponent = nullptr;
				if (!suitableNames.empty()) {
					ImGui::Combo("Components list", &addComponentIndex, suitableNames.data(), std::size(suitableNames));
					currentComponent = suitableNames[addComponentIndex];
				}
				else {
					ImGui::Combo("Components list", &addComponentIndex, items, std::size(items));
					currentComponent = items[addComponentIndex];
				}


                //static int addComponentIndex = 0;
                //ImGui::Combo("", &addComponentIndex, items, std::size(items));
                ImGui::PushID("Add");
                //const char* currentComponent = items[addComponentIndex];
				AddComponentWithName(currentComponent, world_.get(), entity1id);
                ImGui::PopID();
                ImGui::Unindent(20.0f);
            }
            ImGui::Separator();
            ImGui::Unindent(20.f);
        }
        ImGui::PopID();

	};

	void EndECSInspectorWindow::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0) {


		ImGui::End();

	};



} // namespace OksEngine