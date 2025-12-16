#pragma once
#include <Render\Model\auto_OksEngine.Vertices3D.hpp>

namespace OksEngine
{
void EditVertices3D(std::shared_ptr<ECS2::World> ecsWorld, Vertices3D *vertices3D) {

	ImGui::PushID(Vertices3D::GetTypeId());

	std::string text;

	for (Common::Index i = 0; i < vertices3D->vertices_.GetVerticesNumber(); i++) {
		auto vertex = vertices3D->vertices_[i];

		text += "{";

		text += "x = " + std::to_string(vertex.position_.GetX());
		text += ", ";
		text += "y = " + std::to_string(vertex.position_.GetY());
		text += ", ";
		text += "z = " + std::to_string(vertex.position_.GetZ());

		text += "}";

		if (i != vertices3D->vertices_.GetVerticesNumber() - 1) {
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