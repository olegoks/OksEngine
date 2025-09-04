#include <System/ECSGenerator2.ParsedSystem.hpp>

namespace ECSGenerator2 {


	bool ParsedSystem::UpdateMethodInfo::IsCreatesComponent(const std::string& componentName) {

		bool isCreates = false;

		ForEachProcessEntity([&](const ProcessedEntity& entity, bool isLast) {

			auto itResult = std::find(
				entity.creates_.begin(),
				entity.creates_.end(),
				componentName);

			if (itResult != entity.creates_.end()) {
				isCreates = true;
				return false;
			}

			return true;
			});

		ForEachRandomAccessEntity([&](const RandomAccessEntity& entity, bool isLast) {

			auto itResult = std::find(
				entity.creates_.begin(),
				entity.creates_.end(),
				componentName);

			if (itResult != entity.creates_.end()) {
				isCreates = true;
				return false;
			}

			return true;
			});

		return isCreates;
	}

	[[nodiscard]]
	bool ParsedSystem::UpdateMethodInfo::IsChangesComponent(const std::string& component) const {
		bool isChangesComponent = false;
		ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {
			if (entity.IsChangesComponent(component)) {
				isChangesComponent = true;
				return false;
			}
			return true;
			});
		if (isChangesComponent) {
			return true;
		}
		ForEachRandomAccessEntity([&](const ParsedSystem::RandomAccessEntity& entity, bool isLast) {
			if (entity.IsChangesComponent(component)) {
				isChangesComponent = true;
				return false;
			}
			return true;
			});

		return isChangesComponent;

	}

	[[nodiscard]]
	bool ParsedSystem::UpdateMethodInfo::IsReadsComponent(const std::string& component) const {
		bool isReadsComponent = false;
		ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {
			if (entity.IsReadsComponent(component)) {
				isReadsComponent = true;
				return false;
			}
			return true;
			});

		if (isReadsComponent) {
			return true;
		}
		ForEachRandomAccessEntity([&](const ParsedSystem::RandomAccessEntity& entity, bool isLast) {
			if (entity.IsReadsComponent(component)) {
				isReadsComponent = true;
				return false;
			}
			return true;
			});

		return isReadsComponent;

	}

	[[nodiscard]]
	Common::Size ParsedSystem::UpdateMethodInfo::GetProcessesEntitiesNumber() const noexcept {
		return processesEntities_.size();
	}

	using ProcessComponentName = std::function<bool(const std::string& systemName, bool isLast)>;

	using ProcessRandomAccessEntity = std::function<bool(const ParsedSystem::RandomAccessEntity& entity, bool isLast)>;
	void ParsedSystem::UpdateMethodInfo::ForEachRandomAccessEntity(ProcessRandomAccessEntity&& processEntity) const {
		for (Common::Index i = 0; i < randomAccessesEntities_.size(); i++) {
			const RandomAccessEntity& entity = randomAccessesEntities_[i];
			if (!processEntity(entity, (i == randomAccessesEntities_.size() - 1))) {
				break;
			}
		}
	}

	void ParsedSystem::UpdateMethodInfo::ForEachRandomAccessComponent(ProcessComponentName&& processComponent) const {

		ForEachRandomAccessEntity([&](const RandomAccessEntity& entity, bool isLastEntity) {

			for (Common::Index i = 0; i < entity.includes_.size(); i++) {
				const std::string& componentName = entity.includes_[i].name_;
				if (!processComponent(componentName, isLastEntity && (i == entity.includes_.size() - 1))) {
					break;
				}
			}

			return true;
			});

	}

	//using ProcessConstRequestEntity = std::function<bool(const ParsedSystem::ProcessedEntity& entity, bool isLast)>;

	void ParsedSystem::UpdateMethodInfo::ForEachProcessEntity(ProcessConstRequestEntity&& processEntity) const {
		for (Common::Index i = 0; i < processesEntities_.size(); i++) {
			const ProcessedEntity& entity = processesEntities_[i];
			if (!processEntity(entity, (i == processesEntities_.size() - 1))) {
				break;
			}
		}
	}

	//using ProcessRequestEntity = std::function<bool(ParsedSystem::ProcessedEntity& entity, bool isLast)>;

	void ParsedSystem::UpdateMethodInfo::ForEachProcessEntity(ProcessRequestEntity&& processEntity) {
		for (Common::Index i = 0; i < processesEntities_.size(); i++) {
			ProcessedEntity& entity = processesEntities_[i];
			if (!processEntity(entity, (i == processesEntities_.size() - 1))) {
				break;
			}
		}
	}

	[[nodiscard]]
	bool ParsedSystem::UpdateMethodInfo::IsProcessesComponent(const std::string& component) {
		bool isProcessesComponent = false;
		ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {
			if (entity.IsProcessesComponent(component)) {
				isProcessesComponent = true;
				return false;
			}
			return true;
			});
		if (isProcessesComponent) {
			return true;
		}
		ForEachRandomAccessEntity([&](const ParsedSystem::RandomAccessEntity& entity, bool isLast) {
			if (entity.IsProcessesComponent(component)) {
				isProcessesComponent = true;
				return false;
			}
			return true;
			});

		return isProcessesComponent;
	}

	bool ParsedSystem::UpdateMethodInfo::IsAccessesComponentByRandomAccess(const std::string& componentName) {

		bool isAccess = false;

		ForEachRandomAccessEntity([&](const RandomAccessEntity& entity, bool isLast) {

			for (const auto& include : entity.includes_) {
				if (componentName == include.GetName()) {
					isAccess = true;
					break;
				}
			}


			return true;
			});

		return isAccess;
	}
}