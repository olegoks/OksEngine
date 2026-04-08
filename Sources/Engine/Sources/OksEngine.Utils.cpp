#include <OksEngine.Utils.hpp>

#include <auto_OksEngine.Utils.hpp>

namespace OksEngine {
	void EditEntity(std::shared_ptr<ECS2::World> world_, ECS2::Entity::Id entity1id) {
		const std::string idString = std::to_string(entity1id);
		ImGui::PushID(idString.c_str());

		const std::string headerText = [&]() -> std::string {

			std::string name;
			if (world_->IsComponentExist<Name>(entity1id))
			{
				auto* nameComponent = world_->GetComponent<Name>(entity1id);
				name = nameComponent->value_;
			}
			return std::vformat(
				"Id:{} {} {}",
				std::make_format_args(
					idString,
					[&]() {
						ECS2::Entity::Type entityType = world_->GetEntityType(entity1id);

						std::string result = magic_enum::enum_name(entityType).data();

						if (entityType == ECS2::Entity::Type::Archetype) {
							ASSERT(world_->IsArchetypeEntity(entity1id));
							const char* archetypeName = GetArchetypeNameByArchetypeComponentsFilter(world_->GetArchetypeComponents(entity1id));
							result += " ";
							result += archetypeName;
						}
						return result;
					}(),
					name));
			}();





		if (ImGui::CollapsingHeader(headerText.c_str()))
		{
			ImGui::Indent(20.f);
			auto editComponent = []<class ComponentType>(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) {
				bool isExistInECS = world->IsComponentExist<ComponentType>(id);
				if (isExistInECS) {
					bool isExistInInspector = isExistInECS;
					if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExistInInspector))
					{
						ComponentType* component = world->GetComponent<ComponentType>(id);
						Edit<ComponentType>(world, id, component);
						ImGui::Spacing();
					}
					if (!isExistInInspector)
					{
						//If user removes component in Inspector, but component exists in ECS need to remove component.
						if (world->IsComponentExist<ComponentType>(id))
						{
							world->RemoveComponent<ComponentType>(id);
						}
					}
				}
				else {
					if (world->IsArchetypeEntity(id)) {
						const ECS2::ComponentsFilter archetypeComponents = world->GetArchetypeComponents(id);
						if (archetypeComponents.IsSet<ComponentType>()) {
							const char* missingComponentName = GetComponentNameByTypeId(ComponentType::GetTypeId());
							ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
							if (ImGui::CollapsingHeader(missingComponentName)) {}
							ImGui::PopStyleColor();
						}

					}
				}
			};
			{
				ImGui::PushID("Edit");
				FOR_EACH_COMPONENT(editComponent.template operator() < , > (world_, entity1id);)
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

				ImGui::PushID("Add");
				AddComponentWithName(currentComponent, world_.get(), entity1id);
				ImGui::PopID();
				ImGui::Unindent(20.0f);
			}
			ImGui::Separator();
			ImGui::Unindent(20.f);
		}
		ImGui::PopID();
	}

}