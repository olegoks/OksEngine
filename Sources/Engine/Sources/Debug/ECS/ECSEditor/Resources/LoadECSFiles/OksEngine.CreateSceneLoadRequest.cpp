#include <Debug/ECS/ECSEditor/Resources/LoadECSFiles/auto_OksEngine.CreateSceneLoadRequest.hpp>

namespace OksEngine {
	void CreateSceneLoadRequest::Update(
		ECS2::Entity::Id entity0id, const ImGuiState* imGuiState0,
		const ECSEditorWindow* editorContext0,
		const LoadECSFilesButtonPressed* loadECSFilesButtonPressed0,
		ECS2::Entity::Id entity1id, const ECSController* eCSController1) {

		CreateComponent<LoadSceneRequest>(entity1id, "OksEngine.Test.scn", entity0id);
		RemoveComponent<LoadECSFilesButtonPressed>(entity0id);
	};
}