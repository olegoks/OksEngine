
#include <Behaviour/OksEngine.Behaviour.hpp>

namespace OksEngine {

	Behaviour::Behaviour(const std::string& scriptName, const std::string& objectName) :
		ECSComponent{ nullptr },
		scriptName_{ scriptName },
		objectName_{ objectName } {}

	template<>
	void Edit<Behaviour>(Behaviour* behaviour) {
		ImGui::TextDisabled("Script name: %s", behaviour->scriptName_.c_str());
		ImGui::TextDisabled("Object name: %s", behaviour->objectName_.c_str());
	}


	template<>
	void Add<Behaviour>(ECS::World* world, ECS::Entity::Id id) {
		static char scriptName[100]{ "Camera.lua" };
		static char objectName[100]{ "Camera" };
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Script name:", scriptName, 100);
			ImGui::InputText("Object name:", objectName, 100);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Behaviour>(id)) {
				world->CreateComponent<Behaviour>(id, std::string{ scriptName }, std::string{ objectName });
			}
		}
	}

}