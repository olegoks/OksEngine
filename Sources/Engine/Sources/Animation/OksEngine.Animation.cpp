
#include <Animation/OksEngine.Animation.hpp>

namespace OksEngine {


	Animation::Animation(const CreateInfo& createInfo) :
		ECSComponent{ nullptr },
		name_{ createInfo.name_ },
		durationInTicks_{ createInfo.duration_ },
		ticksPerSecond_{ createInfo.ticksPerSecond_ },
		positionKeys_{ createInfo.positionKeys_ },
		rotationKeys_{ createInfo.rotationKeys_ },
		scalingKeys_{ createInfo.scalingKeys_ } { }

	template<>
	void Edit<Animation>(Animation* animation) {
		ImGui::TextDisabled("Name: %s", animation->name_.c_str());
		ImGui::TextDisabled("Duration in ticks: %f", animation->durationInTicks_);
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