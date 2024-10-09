
#include <Behaviour/OksEngine.LuaScript.hpp>

#include <Behaviour/OksEngine.Behaviour.hpp>

namespace OksEngine {


	template<>
	void Edit<LuaScript>(LuaScript* luaScript) {
		/*ImGui::TextDisabled("Script id: %d", luaScript->id_);*/
	}


	template<>
	void Add<LuaScript>(ECS::World* world, ECS::Entity::Id id) {
		/*static char scriptName[100]{ "Camera.lua" };
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
		}*/
	}

}