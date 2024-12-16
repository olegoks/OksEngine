#pragma once

#include <string>
#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct SoundInfo : public ECSComponent<SoundInfo> {
	public:
		SoundInfo() :
			ECSComponent{ nullptr }, soundName_{ "No name" } {

		}
		SoundInfo(std::string soundName) :
			ECSComponent{ nullptr }, soundName_{ soundName } {}

		std::string soundName_ = "No name";
	};


	template<>
	inline void Edit<SoundInfo>(SoundInfo* soundInfo) {
		ImGui::TextDisabled("Name %s:", soundInfo->soundName_.c_str());
	}

	template<>
	inline void Add<SoundInfo>(ECS::World* world, ECS::Entity::Id id) {
		static char name[100] = { "USPHit.ogg" };
		if (ImGui::CollapsingHeader("Create info")) {
			ImGui::InputText("Sound name:", name, sizeof(name));
		}
		if (ImGui::Button("Add component")) {
			if (!world->IsComponentExist<SoundInfo>(id)) {
				world->CreateComponent<SoundInfo>(id, name);
			}
		}

	}


	template<>
	inline void Bind<SoundInfo>(::Lua::Context& context) {
		context.GetGlobalNamespace()
			.beginClass<SoundInfo>("SoundInfo")
			.addConstructor<void(*)(std::string)>()
			.endClass();
	}











}
