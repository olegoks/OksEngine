#include <Debug/ECSCallGraphEditor/Resources\auto_OksEngine.SaveChanges.hpp>

namespace OksEngine {
	void SaveChanges::Update(ECS2::Entity::Id entityId1, const ImGuiState* imGuiState,
		const EditorContext* editorContext,
		const SaveChangesButtonPressed* saveChangesButtonPressed,
		ECS2::Entity::Id entityId2, const CallGraphNode* callGraphNode,
		const Position2D* position2D, const RootNode* rootNode,
		const FilesystemPath* filesystemPath,
		const ResourcePath* resourcePath, const Name* name,
		ECS2::Entity::Id entityId, ResourceSystem* resourceSystem) {

		std::string luaSettingsFile =
			"node = {\n"
			"		position2D = { " + std::to_string(position2D->x_) + ", " + std::to_string(position2D->y_)  +" }\n"
			"}";
		Resources::ResourceData settingsFileData{ luaSettingsFile.c_str(), luaSettingsFile.size() };

		auto createSettingsFileTask = resourceSystem->system_->CreateNewFile(Subsystem::Type::Debug, filesystemPath->path_.parent_path() / ("OksEngine." + name->value_ + ".dgs"), std::move(settingsFileData)); // draw graph settings
		resourceSystem->system_->CreateNewFile(Subsystem::Type::Debug, createSettingsFileTask);
	
	};
}