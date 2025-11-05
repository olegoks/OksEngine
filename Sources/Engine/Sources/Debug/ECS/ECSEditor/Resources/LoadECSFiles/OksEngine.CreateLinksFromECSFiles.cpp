#include <Debug/ECS/ECSEditor/Resources/LoadECSFiles\auto_OksEngine.CreateLinksFromECSFiles.hpp>

namespace OksEngine {
	void CreateLinksFromECSFiles::Update(
		ECS2::Entity::Id entity0id, const ImGUI::State* imGuiState0,
		const EditorContext* editorContext0,
		const ECSEditorWindow* eCSEditorWindow0,
		const CreateLinksRequest* createLinksRequest0,
		ECS2::Entity::Id entity1id, const CallGraphNode* callGraphNode1,
		const Name* name1, const RunAfter* runAfter1,
		const RunBefore* runBefore1, const BeforePin* beforePin1,
		const AfterPin* afterPin1, 
		
		ECS2::Entity::Id entity2id, const CallGraphNode* callGraphNode2,
		const Name* name2, const RunAfter* runAfter2, const RunBefore* runBefore2,
		const BeforePin* beforePin2, const AfterPin* afterPin2) {


		if (std::find(
			runAfter1->runAfterSystems_.cbegin(),
			runAfter1->runAfterSystems_.cend(),
			name2->value_) != runAfter1->runAfterSystems_.cend()) {

			const ECS2::Entity::Id linkEntityId = CreateEntity();
			CreateComponent<Link>(
				linkEntityId,
				afterPin1->id_,
				beforePin2->id_);
			CreateComponent<Serializable>(linkEntityId);

		}

	};

	void CreateLinksFromECSFiles::AfterUpdate() {

	};
}  // namespace OksEngine