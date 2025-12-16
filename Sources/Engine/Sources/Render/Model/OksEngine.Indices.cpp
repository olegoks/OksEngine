#pragma once
#include <Render\Model\auto_OksEngine.Indices.hpp>

namespace OksEngine
{
	void EditIndices(std::shared_ptr<ECS2::World> ecsWorld, Indices* indices) {
		ImGui::PushID(Indices::GetTypeId());

		std::string text;

		for (Common::Index i = 0; i < indices->indices_.GetIndicesNumber(); i++) {

			text += std::to_string(indices->indices_[i]);
			if (i != indices->indices_.GetIndicesNumber() - 1) {
				text += ", ";
			}
		}

		ImGui::TextWrapped("%s", text.c_str());

		if (ImGui::SmallButton("Copy")) {
			ImGui::SetClipboardText(text.c_str());
		}

		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::Text("Copy to clipboard");
			ImGui::EndTooltip();
		}

		ImGui::PopID();

	};

}