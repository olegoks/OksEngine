
#include <Animation/OksEngine.Animation.hpp>

namespace OksEngine {


	Animation::Animation(Common::Double ticksNumber, float ticksPerSecond, std::vector<StateInfo> states) :
		ECSComponent{ nullptr },
		duration_{ ticksNumber },
		ticksPerSecond_{ ticksPerSecond },
		states_{ states } {}

	template<>
	void Edit<Animation>(Animation* animation) {
		ImGui::TextDisabled("Duration: %f", animation->duration_);
		ImGui::TextDisabled("Ticks per second: %f", animation->ticksPerSecond_);

	}

	template<>
	void Add<Animation>(ECS::World* world, ECS::Entity::Id id) {
		/*static char scriptName[100]{ "Camera.lua" };
		static char objectName[100]{ "Camera" };
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Script name:", scriptName, 100);
			ImGui::InputText("Object name:", objectName, 100);
			ImGui::Spacing();
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<Animation>(id)) {
				world->CreateComponent<Animation>(id, std::string{ scriptName }, std::string{ objectName });
			}
		}*/
	}

}