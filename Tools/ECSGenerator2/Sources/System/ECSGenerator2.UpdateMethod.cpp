#include <System/ECSGenerator2.ParsedSystem.hpp>

namespace ECSGenerator2 {


	bool ParsedSystem::UpdateMethodInfo::IsCreatesComponent(const std::string& componentName) {

		bool isCreates = false;

		ForEachProcessEntity([&](const ProcessedEntity& entity, bool isLast) {

			auto itResult = std::find_if(
				entity.creates_.cbegin(),
				entity.creates_.cend(),
				[&](const ParsedSystem::Create& create) {
					return create.GetName() == componentName;
				});

			if (itResult != entity.creates_.end()) {
				isCreates = true;
				return false;
			}

			return true;
			});

		ForEachRandomAccessEntity([&](const RandomAccessEntity& entity, bool isLast) {

			auto itResult = std::find_if(
				entity.creates_.cbegin(),
				entity.creates_.cend(),
				[&](const ParsedSystem::Create& create) { 
					return create.GetName() == componentName;
				});

			if (itResult != entity.creates_.end()) {
				isCreates = true;
				return false;
			}

			return true;
			});

		return isCreates;
	}

	[[nodiscard]]
	bool ParsedSystem::UpdateMethodInfo::IsChangesComponent(const std::string& component) {
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
		ForEachRandomAccessEntity([&](ParsedSystem::RandomAccessEntity& entity, bool isLast) {
			if (entity.IsChangesComponent(component)) {
				isChangesComponent = true;
				return false;
			}
			return true;
			});

		return isChangesComponent;

	}

	[[nodiscard]]
	bool ParsedSystem::UpdateMethodInfo::IsReadsComponent(const std::string& component) {
		bool isReadsComponent = false;
		ForEachProcessEntity([&](ParsedSystem::ProcessedEntity& entity, bool isLast) {
			if (entity.IsReadsComponent(component)) {
				isReadsComponent = true;
				return false;
			}
			return true;
			});

		if (isReadsComponent) {
			return true;
		}
		ForEachRandomAccessEntity([&](ParsedSystem::RandomAccessEntity& entity, bool isLast) {
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

	using ProcessRandomAccessEntity = std::function<bool(ParsedSystem::RandomAccessEntity& entity, bool isLast)>;
	void ParsedSystem::UpdateMethodInfo::ForEachRandomAccessEntity(ProcessRandomAccessEntity&& processEntity) {
		for (Common::Index i = 0; i < randomAccessesEntities_.size(); i++) {
			 RandomAccessEntity& entity = randomAccessesEntities_[i];
			if (!processEntity(entity, (i == randomAccessesEntities_.size() - 1))) {
				break;
			}
		}
	}

	void ParsedSystem::UpdateMethodInfo::ForEachCreateEntity(ProcessCreateEntity&& processEntity) {
		for (Common::Index i = 0; i < createsEntities_.size(); i++) {
			CreatesEntity& entity = createsEntities_[i];
			if (!processEntity(entity, (i == createsEntities_.size() - 1))) {
				break;
			}
		}
	}


	void ParsedSystem::UpdateMethodInfo::ForEachRandomAccessComponent(ProcessComponentName&& processComponent){

		ForEachRandomAccessEntity([&](RandomAccessEntity& entity, bool isLastEntity) {

			for (Common::Index i = 0; i < entity.includes_.size(); i++) {
				std::string& componentName = entity.includes_[i].name_;
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
	bool ParsedSystem::UpdateMethodInfo::IsProcessesComponent(std::string& component) {
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
		ForEachRandomAccessEntity([&](ParsedSystem::RandomAccessEntity& entity, bool isLast) {
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