#pragma once
#include <UI\KeyMappings\auto_OksEngine.KeyMappings.hpp>

namespace OksEngine
{
	namespace UI
	{
		void EditKeyMappings(std::shared_ptr<ECS2::World> ecsWorld, KeyMappings* keyMappings) {
			
			if (ImGui::BeginTable("KeyMappings", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
				// Заголовки таблицы
				ImGui::TableSetupColumn("Key");
				ImGui::TableSetupColumn("Value");
				ImGui::TableHeadersRow();

				// Вывод данных
				for (const auto& [key, value] : keyMappings->mappings_) {
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%s", key.c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%s", value.c_str());
				}

				ImGui::EndTable();
			}
		
		};

		void AddKeyMappings(ECS2::World* ecsWorld, ECS2::Entity::Id entityId) {
		
		
		
		};

		void BindKeyMappings(::Lua::Context& context) {
		
		
		
		};

		KeyMappings ParseKeyMappings(luabridge::LuaRef& keyMappingsRef) {

			KeyMappings keyMappings;

			for (luabridge::Iterator it(keyMappingsRef); !it.isNil(); ++it) {
				luabridge::LuaRef record = it.value();

				const std::string key = record["key"].cast<std::string>().value();
				const std::string value = record["value"].cast<std::string>().value();

				keyMappings.mappings_[key] = value;

			}

			return keyMappings;

		};

		std::string SerializeKeyMappings(const KeyMappings* keyMappings) {

			using namespace std::string_literals;
			
			std::string luaScript = "keyMappings = {";

			for (auto [key, value] : keyMappings->mappings_) {
				luaScript += "{ key = \"" + key + "\", value = \"" + value + "\" },";
			}

			luaScript += "}";

			return luaScript;
		};

	} // namespace UI

} // namespace OksEngine