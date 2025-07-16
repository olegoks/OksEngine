#pragma once
#include <Debug\ECS\Inspector\Window\auto_OksEngine.ECSInspector.hpp>

#include <auto_OksEngine.ECS.hpp>
#include <auto_OksEngine.EditEntity.hpp>

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

	void ShowEntitiesInfo::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0, ECS2::Entity::Id entity1id) {


		EditEntity(world_, entity1id);

	};

	void EndECSInspectorWindow::Update(ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const MainMenuBar* mainMenuBar0, const ECSMenu* eCSMenu0,
		const ECSInspectorMenuItem* eCSInspectorMenuItem0,
		const ECSInspectorWindow* eCSInspectorWindow0) {


		ImGui::End();

	};



} // namespace OksEngine