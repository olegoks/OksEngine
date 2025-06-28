#pragma once 

#include <filesystem>
#include <string>
#include <functional>

#include <Common.hpp>

#include <ECSGenerator.Code.hpp>
#include <ECSGenerator.ParsedECSFile.hpp>
#include <ECSGenerator.ProjectContext.hpp>

namespace ECSGenerator {


	class ParsedSystemECSFile : public ParsedECSFile {
	public:

		using Exclude = std::string;

		struct Include {
			std::string name_;
			bool readonly_ = true;

			[[nodiscard]]
			bool IsReadonly() const {
				return readonly_;
			}

			[[nodiscard]]
			const std::string GetName() const noexcept {
				return name_;
			}

			[[nodiscard]]
			const std::string GetLowerName() const noexcept {
				return std::string{ (char)std::tolower(GetName()[0]) } + GetName().substr(1);
			}
		};

		struct RandomAccessEntity {
			std::vector<Include> includes_;
			std::vector<std::string> creates_;
			bool randomAccessComponents_ = false;

			using ProcessInclude = std::function<bool(const Include& include, bool isLast)>;

			void ForEachInclude(ProcessInclude&& processInclude) const {
				for (Common::Index i = 0; i < includes_.size(); i++) {
					const Include& include = includes_[i];
					if (!processInclude(include, (i == includes_.size() - 1))) {
						break;
					}
				}
			}

			[[nodiscard]]
			bool IsProcessesComponent(const std::string& component) const {
				bool isProcessesComponent = false;
				ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component) {
						isProcessesComponent = true;
						return false;
					}
					return true;
					});
				return isProcessesComponent;
			}

			[[nodiscard]]
			bool IsReadsComponent(const std::string& component) const {
				bool isReadsComponent = false;
				ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && dependenceSystemInclude.readonly_) {
						isReadsComponent = true;
						return false;
					}
					return true;
					});
				return isReadsComponent;
			}

			[[nodiscard]]
			bool IsChangesComponent(const std::string& component) const {
				bool isChangesComponent = false;
				ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && !dependenceSystemInclude.readonly_) {
						isChangesComponent = true;
						return false;
					}
					return true;
					});
				return isChangesComponent;
			}

		};

		struct RequestEntity {
			std::vector<Include> includes_;
			bool processesAllCombinations_ = false;
			std::vector<Exclude> excludes_;
			std::vector<std::string> creates_;
			std::vector<std::string> removes_;
			bool randomAccessComponents_ = false; // Process components that are not required using IsComponentExist()/GetComponentsFilter() and GetComponent()

			using ProcessInclude = std::function<bool(const Include& include, bool isLast)>;

			void ForEachInclude(ProcessInclude&& processInclude) const {
				for (Common::Index i = 0; i < includes_.size(); i++) {
					const Include& include = includes_[i];
					if (!processInclude(include, (i == includes_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessExclude = std::function<bool(const Exclude& include, bool isLast)>;

			void ForEachExclude(ProcessExclude&& processExclude) const {
				for (Common::Index i = 0; i < excludes_.size(); i++) {
					const Exclude& include = excludes_[i];
					if (!processExclude(include, (i == excludes_.size() - 1))) {
						break;
					}
				}
			}

			[[nodiscard]]
			bool IsProcessesComponent(const std::string& component) const {
				bool isProcessesComponent = false;
				ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component) {
						isProcessesComponent = true;
						return false;
					}
					return true;
					});
				return isProcessesComponent;
			}

			[[nodiscard]]
			bool IsReadsComponent(const std::string& component) const {
				bool isReadsComponent = false;
				ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && dependenceSystemInclude.readonly_) {
						isReadsComponent = true;
						return false;
					}
					return true;
					});
				return isReadsComponent;
			}

			[[nodiscard]]
			bool IsChangesComponent(const std::string& component) const {
				bool isChangesComponent = false;
				ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && !dependenceSystemInclude.readonly_) {
						isChangesComponent = true;
						return false;
					}
					return true;
					});
				return isChangesComponent;
			}

		};

		enum class Thread {
			Main,
			Child,
			Undefined
		};

		enum class SystemType {
			OneEntity,
			OneMoreEntities,
			Initialize,
			Undefined
		};

		struct CreateInfo {
			std::string name_;
			std::filesystem::path path_;
			std::vector<RequestEntity> processesEntities_;
			//std::vector<
			std::vector<std::vector<std::string>> createsEntities_; // we dont need info: dynamic or archetype entity to create because we need generate only to add components includes.
			//std::vector<std::string> accessEntityComponents_;

			// Access entity using Entity::Id.
			// Excludes must be empty.
			std::vector<RandomAccessEntity> accessesEntities_;

			bool afterUpdateMethod_ = false;
			Thread thread_ = Thread::Undefined;
			SystemType type_ = SystemType::Undefined;
			std::vector<std::string> runAfter_;
			std::vector<std::string> runBefore_;
		};



		ParsedSystemECSFile(const CreateInfo& createInfo)
			: ParsedECSFile{ createInfo.path_, createInfo.name_ },
			ci_{ createInfo } { }

		using ProcessComponentName = std::function<bool(const std::string& systemName, bool isLast)>;

		bool IsCreatesComponent(const std::string& componentName) {

			bool isCreates = false;

			ForEachRequestEntity([&](const RequestEntity& entity, bool isLast) {

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

		bool IsAccessesComponentByRandomAccess(const std::string& componentName) {

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

		//void ForEachCreateComponent(ProcessComponentName&& processComponent) const {
		//	for (Common::Index i = 0; i < ci_.createEntityComponents_.size(); i++) {
		//		const std::string& componentName = ci_.createEntityComponents_[i];
		//		if (!processComponent(componentName, (i == ci_.createEntityComponents_.size() - 1))) {
		//			break;
		//		}
		//	}
		//}

		void ForEachRandomAccessComponent(ProcessComponentName&& processComponent) const {

			ForEachRandomAccessEntity([&](const RandomAccessEntity& entity, bool isLast) {

				for (Common::Index i = 0; i < entity.includes_.size(); i++) {
					const std::string& componentName = entity.includes_[i].name_;
					if (!processComponent(componentName, (i == entity.includes_.size() - 1))) {
						break;
					}
				}

				return true;
				});

		}

		using ProcessRequestEntity = std::function<bool(const RequestEntity& entity, bool isLast)>;

		void ForEachRequestEntity(ProcessRequestEntity&& processEntity) const {
			for (Common::Index i = 0; i < ci_.processesEntities_.size(); i++) {
				const RequestEntity& entity = ci_.processesEntities_[i];
				if (!processEntity(entity, (i == ci_.processesEntities_.size() - 1))) {
					break;
				}
			}
		}
		Common::Size GetRequestEntitiesNumber() const noexcept {
			return ci_.processesEntities_.size();
		}

		using ProcessRandomAccessEntity = std::function<bool(const RandomAccessEntity& entity, bool isLast)>;
		void ForEachRandomAccessEntity(ProcessRandomAccessEntity&& processEntity) const {
			for (Common::Index i = 0; i < ci_.accessesEntities_.size(); i++) {
				const RandomAccessEntity& entity = ci_.accessesEntities_[i];
				if (!processEntity(entity, (i == ci_.accessesEntities_.size() - 1))) {
					break;
				}
			}
		}

		Thread GetThread() {
			return ci_.thread_;
		}

		virtual Type GetType() const noexcept override {
			return Type::System;
		}

		std::string GetName() const {
			return ci_.name_;
		}

		std::string GetLowerName() const {
			return std::string{ (char)std::tolower(GetName()[0]) } + GetName().substr(1);
		}

		SystemType GetSystemType() const {
			return ci_.type_;
		}

		[[nodiscard]]
		bool IsInitializeSystem() const {
			return GetSystemType() == SystemType::Initialize;
		}

		using ProcessSystemName = std::function<bool(const std::string&)>;

		void ForEachRunAfterSystem(ProcessSystemName&& processSystemName) {
			for (const std::string& runAfterSystemName : ci_.runAfter_) {
				const bool stop = !processSystemName(runAfterSystemName);
				if (stop) {
					break;
				}
			}
		}
		void ForEachRunBeforeSystem(ProcessSystemName&& processSystemName) {
			for (const std::string& runBeforeSystemName : ci_.runBefore_) {
				const bool stop = !processSystemName(runBeforeSystemName);
				if (stop) {
					break;
				}
			}
		}

		[[nodiscard]]
		bool IsProcessesComponent(const std::string& component) {
			bool isProcessesComponent = false;
			ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {
				if (entity.IsProcessesComponent(component)) {
					isProcessesComponent = true;
					return false;
				}
				return true;
				});
			if (isProcessesComponent) {
				return true;
			}
			ForEachRandomAccessEntity([&](const ParsedSystemECSFile::RandomAccessEntity& entity, bool isLast) {
				if (entity.IsProcessesComponent(component)) {
					isProcessesComponent = true;
					return false;
				}
				return true;
				});

			return isProcessesComponent;
		}
		[[nodiscard]]
		bool IsReadsComponent(const std::string& component) const {
			bool isReadsComponent = false;
			ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {
				if (entity.IsReadsComponent(component)) {
					isReadsComponent = true;
					return false;
				}
				return true;
				});

			if (isReadsComponent) {
				return true;
			}
			ForEachRandomAccessEntity([&](const ParsedSystemECSFile::RandomAccessEntity& entity, bool isLast) {
				if (entity.IsReadsComponent(component)) {
					isReadsComponent = true;
					return false;
				}
				return true;
				});

			return isReadsComponent;

		}

		[[nodiscard]]
		bool IsChangesComponent(const std::string& component) const {
			bool isChangesComponent = false;
			ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {
				if (entity.IsChangesComponent(component)) {
					isChangesComponent = true;
					return false;
				}
				return true;
				});
			if (isChangesComponent) {
				return true;
			}
			ForEachRandomAccessEntity([&](const ParsedSystemECSFile::RandomAccessEntity& entity, bool isLast) {
				if (entity.IsChangesComponent(component)) {
					isChangesComponent = true;
					return false;
				}
				return true;
				});

			return isChangesComponent;

		}

		bool IsDependsFromSystem(const std::shared_ptr<ParsedSystemECSFile> system) const {

			if (GetName() == system->GetName()) {
				return false;
			}

			bool isDepends = false;
			//Check create entity dependecise.
			//ForEachCreateComponent([&](const std::string& componentName, bool isLast) {
			//	if (system->IsCreatesComponent(componentName)) {
			//		isDepends = true;
			//		OS::LogInfo("Dependence", 
			//			{ "System {} depends from {}, because they create the same component {}.",
			//			GetName(),
			//			system->GetName(),
			//			componentName });
			//		return false;
			//	}
			//	if (system->IsProcessesComponent(componentName)) {
			//		isDepends = true;
			//		OS::LogInfo("Dependence",
			//			{ "System {} depends from {}, because system {} creates component {} that system {} processes.",
			//			GetName(),
			//			system->GetName(),
			//			GetName(),
			//			componentName,
			//			system->GetName() });
			//		return false;
			//	}
			//	if (system->IsAccessesComponent(componentName)) {
			//		isDepends = true;
			//		OS::LogInfo("Dependence",
			//			{ "System {} depends from {}, because system {} creates component {} that system {} accesses.",
			//			GetName(),
			//			system->GetName(),
			//			GetName(),
			//			componentName,
			//			system->GetName() });
			//		return false;
			//	}
			//	return true;
			//	});

			//if (isDepends) {
			//	return true;
			//}

			//ForEachAccessComponent([&](const std::string& componentName, bool isLast) {
			//	if (system->IsCreatesComponent(componentName)) {
			//		isDepends = true;
			//		OS::LogInfo("Dependence",
			//			{ "System {} depends from {}, because they create the same component {}.",
			//			GetName(),
			//			system->GetName(),
			//			componentName });
			//		return false;
			//	}
			//	if (system->IsProcessesComponent(componentName)) {
			//		isDepends = true;
			//		OS::LogInfo("Dependence",
			//			{ "System {} depends from {}, because system {} accesses component {} that system {} processes.",
			//			GetName(),
			//			system->GetName(),
			//			GetName(),
			//			componentName,
			//			system->GetName() });
			//		return false;
			//	}
			//	if (system->IsAccessesComponent(componentName)) {
			//		isDepends = true;
			//		OS::LogInfo("Dependence",
			//			{ "System {} depends from {}, because they both access component {}.",
			//			GetName(),
			//			system->GetName(),
			//			componentName });
			//		return false;
			//	}
			//	return true;
			//	});

			//if (isDepends) {
			//	return true;
			//}

			ForEachRequestEntity([&](const RequestEntity& entity, bool isLast) {
				if (entity.randomAccessComponents_ || entity.processesAllCombinations_) {
					isDepends = true;
					return false;
				}
				entity.ForEachInclude([&](const Include& include, bool isLast) {
					if (system->IsCreatesComponent(include.name_)) {
						isDepends = true;
						OS::LogInfo("Dependence",
							{ "System {} depends from {}, because system {} processes component {} that system {} creates.",
							GetName(),
							system->GetName(),
							GetName(),
							include.name_,
							system->GetName() });
						return false;
					}
					if ((include.IsReadonly() && system->IsChangesComponent(include.name_)) ||
						(!include.IsReadonly() && system->IsChangesComponent(include.name_)) ||
						(!include.IsReadonly() && system->IsReadsComponent(include.name_))) {
						isDepends = true;
						return false;
					}
					if (system->IsProcessesComponent(include.name_)) {
						isDepends = true;
						OS::LogInfo("Dependence",
							{ "System {} depends from {},  because they both processes component {}.",
							GetName(),
							system->GetName(),
							include.name_ });
						return false;
					}

					if (system->IsAccessesComponentByRandomAccess(include.name_)) {
						isDepends = true;
						OS::LogInfo("Dependence",
							{ "System {} depends from {}, because system {} processes component {} that system {} accesses.",
							GetName(),
							system->GetName(),
							GetName(),
							include.name_,
							system->GetName() });
						return false;
					}
					return true;
					});
				return true;
				});
			return isDepends;
		}

		CreateInfo ci_;
	};


	ParsedSystemECSFile::Thread GetThread(::Lua::Context& context) {
		luabridge::LuaRef system = context.GetGlobalAsRef("system");
		auto thread = ParsedSystemECSFile::Thread::Child;
		//Nain thread call.
		if (
			luabridge::LuaRef mainThreadCallRef = system["thread"];
			!mainThreadCallRef.isNil() &&
			mainThreadCallRef.isString()) {

			const std::string threadString = mainThreadCallRef.cast<std::string>().value();
			if (threadString == "Main") {
				thread = ParsedSystemECSFile::Thread::Main;
			}
		}
		return thread;
	}

	ParsedSystemECSFile::SystemType GetSystemType(::Lua::Context& context) {

		luabridge::LuaRef system = context.GetGlobalAsRef("system");

		luabridge::LuaRef createsEntities = system["createsEntities"];
		luabridge::LuaRef createsComponents = system["createsComponents"];

		luabridge::LuaRef processesComponents = system["processesComponents"];
		luabridge::LuaRef processesEntities = system["processesEntities"];
		luabridge::LuaRef initSystem = system["initSystem"];

		ParsedSystemECSFile::SystemType systemType = ParsedSystemECSFile::SystemType::Undefined;

		if (!initSystem.isNil()) {
			systemType = ParsedSystemECSFile::SystemType::Initialize;
		}
		else if (!processesComponents.isNil()) {
			systemType = ParsedSystemECSFile::SystemType::OneEntity;
		}
		else if (!processesEntities.isNil()) {
			if (processesEntities.isTable()) {
				int length = context.GetLength(processesEntities);

				if (length == 1) {
					systemType = ParsedSystemECSFile::SystemType::OneEntity;
				}
				else {
					systemType = ParsedSystemECSFile::SystemType::OneMoreEntities;
				}
			}

		}

		return systemType;
	}

	std::shared_ptr<ParsedSystemECSFile> ParseSystemEcsFile(const std::filesystem::path& ecsFilePath, ::Lua::Context& context) {

		auto parseAfterUpdateMethod = [](luabridge::LuaRef systemRef) {

			luabridge::LuaRef afterUpdateMethodRef = systemRef["afterUpdateMethod"];
			if (!afterUpdateMethodRef.isNil()) {
				return afterUpdateMethodRef.cast<bool>().value();
			}
			return false;

			};
		auto parseProcessesComponents = [](luabridge::LuaRef entityOrSystemRef) {

			luabridge::LuaRef entityIncludes = entityOrSystemRef["processesComponents"];



			std::vector<ParsedSystemECSFile::Include> parsedEntityIncludes;


			if (entityIncludes.isNil()) {
				return parsedEntityIncludes;
			}
			for (luabridge::Iterator it(entityIncludes); !it.isNil(); ++it) {
				luabridge::LuaRef include = it.value();
				ParsedSystemECSFile::Include parsedInclude;
				if (!include.isNil()) {
					luabridge::LuaRef includeName = include["name"];
					if (!includeName.isNil()) {
						parsedInclude.name_ = includeName.cast<std::string>().value();
					}
					else {
						parsedInclude.name_ = include.cast<std::string>().value();
					}
					luabridge::LuaRef isIncludeReadonly = include["readonly"];
					if (!isIncludeReadonly.isNil()) {
						parsedInclude.readonly_ = isIncludeReadonly.cast<bool>().value();
					}

				}


				parsedEntityIncludes.push_back(parsedInclude);
				OS::AssertMessage(std::isupper(parsedEntityIncludes.back().name_[0]), "");
			}

			return parsedEntityIncludes;
			};
		auto parseCreatesComponents = [](luabridge::LuaRef systemOrEntityRef) {

			luabridge::LuaRef entityCreates = systemOrEntityRef["createsComponents"];

			std::vector<std::string> parsedEntityCreates;
			if (!entityCreates.isNil()) {
				for (luabridge::Iterator it(entityCreates); !it.isNil(); ++it) {
					luabridge::LuaRef toCreate = it.value();
					parsedEntityCreates.push_back(toCreate.cast<std::string>().value());
					OS::AssertMessage(std::isupper(parsedEntityCreates.back()[0]), "");
				}
			}

			return parsedEntityCreates;

			};
		auto parseRemovesComponents = [](luabridge::LuaRef systemOrEntityRef) {

			luabridge::LuaRef entityRemoves = systemOrEntityRef["removesComponents"];

			std::vector<std::string> parsedEntityRemoves;
			if (!entityRemoves.isNil()) {
				for (luabridge::Iterator it(entityRemoves); !it.isNil(); ++it) {
					luabridge::LuaRef toRemove = it.value();
					parsedEntityRemoves.push_back(toRemove.cast<std::string>().value());
					OS::AssertMessage(std::isupper(parsedEntityRemoves.back()[0]), "");
				}
			}

			return parsedEntityRemoves;

			};
		auto parseCreatesEntities = [&](luabridge::LuaRef systemRef) {

			std::vector<std::vector<std::string>> createsEntities_;

			luabridge::LuaRef createsEntity = systemRef["createsEntities"];
			if (!createsEntity.isNil()) {
				for (luabridge::Iterator it(createsEntity); !it.isNil(); ++it) {

					std::vector<std::string> createsComponents = parseCreatesComponents(it.value());

					createsEntities_.push_back(createsComponents);
				}
			}
			return createsEntities_;
			};
		auto parseExcludes = [](luabridge::LuaRef systemOrEntityRef) {
			//ENTITY EXCLUDES
			luabridge::LuaRef entityExcludes = systemOrEntityRef["excludes"];
			std::vector<ParsedSystemECSFile::Exclude> parsedEntityExcludes;
			if (!entityExcludes.isNil()) {
				for (luabridge::Iterator it(entityExcludes); !it.isNil(); ++it) {
					luabridge::LuaRef exclude = it.value();
					parsedEntityExcludes.push_back(exclude.cast<std::string>().value());

					OS::AssertMessage(std::isupper(parsedEntityExcludes.back()[0]), "");

				}
			}
			return parsedEntityExcludes;

			};
		auto parseProcessesEntities = [&](luabridge::LuaRef systemRef) {

			std::vector<ParsedSystemECSFile::RequestEntity> parsedEntities;

			luabridge::LuaRef entities = systemRef["processesEntities"];
			if (entities.isNil()) {
				return parsedEntities;
			}
			for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
				luabridge::LuaRef entity = it.value();

				luabridge::LuaRef processesAllCombinations = entity["processesAllCombinations"];

				std::vector<ParsedSystemECSFile::Include> parsedEntityIncludes
					= parseProcessesComponents(entity);

				OS::AssertMessage(
					(parsedEntityIncludes.empty() && !processesAllCombinations.isNil())
					|| (!parsedEntityIncludes.empty() && processesAllCombinations.isNil()),
					"System description error."
					" Using processesAllCombinations && processesComponents together is incorrect.");

				std::vector<ParsedSystemECSFile::Exclude> parsedEntityExcludes
					= parseExcludes(entity);

				std::vector<std::string> createsComponents = parseCreatesComponents(entity);

				ParsedSystemECSFile::RequestEntity parsedEntity{
					.includes_ = parsedEntityIncludes,
					.processesAllCombinations_ = !processesAllCombinations.isNil(),
					.excludes_ = parsedEntityExcludes,
					.creates_ = createsComponents
				};
				parsedEntities.push_back(parsedEntity);

			}
			return parsedEntities;

			};
		auto parseRunAfter = [](luabridge::LuaRef systemRef) {

			std::vector<std::string> runAfterSystems;
			luabridge::LuaRef runAfterRef = systemRef["runAfter"];
			if (!runAfterRef.isNil()) {
				if (runAfterRef.isTable()) {
					for (luabridge::Iterator it(runAfterRef); !it.isNil(); ++it) {
						luabridge::LuaRef runAfterSystemRef = it.value();
						runAfterSystems.push_back(runAfterSystemRef.cast<std::string>().value());
					}
				}
				else {
					runAfterSystems.push_back(runAfterRef.cast<std::string>().value());
				}
			}
			return runAfterSystems;
			};
		auto parseRunBefore = [](luabridge::LuaRef systemRef) {
			std::vector<std::string> runBeforeSystems;
			luabridge::LuaRef runBeforeRef = systemRef["runBefore"];
			if (!runBeforeRef.isNil()) {
				if (runBeforeRef.isTable()) {
					for (luabridge::Iterator it(runBeforeRef); !it.isNil(); ++it) {
						luabridge::LuaRef runAfterSystemRef = it.value();
						runBeforeSystems.push_back(runAfterSystemRef.cast<std::string>().value());
					}
				}
				else {
					runBeforeSystems.push_back(runBeforeRef.cast<std::string>().value());
				}
			}
			return runBeforeSystems;
			};
		auto parseAccessesEntities = [](luabridge::LuaRef systemRef) {

			std::vector<ParsedSystemECSFile::RandomAccessEntity> accessesEntities;



			//Access entities by id.
			luabridge::LuaRef accessingEntities = systemRef["accessingEntities"];


			if (!accessingEntities.isNil()) {
				for (luabridge::Iterator it(accessingEntities); !it.isNil(); ++it) {

					std::vector<ParsedSystemECSFile::Include> accessesComponents;
					std::vector<std::string> createsComponents;

					luabridge::LuaRef toAccess = it.value();
					luabridge::LuaRef accessingComponentsRef = toAccess["accessingComponents"];
					for (luabridge::Iterator itJ(accessingComponentsRef); !itJ.isNil(); ++itJ) {
						luabridge::LuaRef accessingComponentRef = itJ.value();

						ParsedSystemECSFile::Include include{
							.name_ = accessingComponentRef.cast<std::string>().value(),
							.readonly_ = false // TODO: add option to access component only to read.
						};
						accessesComponents.push_back(include);
						OS::AssertMessage(std::isupper(accessesComponents.back().name_[0]), "");
					}
					luabridge::LuaRef createsComponentsRef = toAccess["createsComponents"];
					if (!createsComponentsRef.isNil()) {
						for (luabridge::Iterator itJ(createsComponentsRef); !itJ.isNil(); ++itJ) {
							luabridge::LuaRef createsComponentRef = itJ.value();
							createsComponents.push_back(createsComponentRef.cast<std::string>().value());
							OS::AssertMessage(std::isupper(createsComponents.back()[0]), "");
						}
					}
					ParsedSystemECSFile::RequestEntity parsedEntity{
						.includes_ = accessesComponents,
						.creates_ = createsComponents,
					};

				}
			}

			return accessesEntities;
			};
		auto parseRandomAccessComponents = [](luabridge::LuaRef systemOrEntityRef) {
			luabridge::LuaRef randomAccessComponentsRef = systemOrEntityRef["randomAccessComponents"];
			bool randomAccessComponents = false;
			if (!randomAccessComponentsRef.isNil()) {
				randomAccessComponents = randomAccessComponentsRef.cast<bool>().value();
			}
			return randomAccessComponents;
			};

		luabridge::LuaRef system = context.GetGlobalAsRef("system");

		if (system["disable"].cast<bool>().value()) {
			return nullptr;
		}

		ParsedSystemECSFile::SystemType systemType = GetSystemType(context);

		if (system["name"].cast<std::string>().value() == "FindScenes") {
			//__debugbreak();
		}

		std::vector<ParsedSystemECSFile::RequestEntity> requestEntities;

		//One entity.
		auto parsedIncludes = parseProcessesComponents(system);
		auto parsedExcludes = parseExcludes(system);

		if (system["name"].cast<std::string>().value() == "CreateSceneEntities") {
			systemType = systemType;
		}
		auto parsedCreatesComponents = parseCreatesComponents(system);
		auto parsedRemovesComponents = parseRemovesComponents(system);
		bool randomAccessComponents = parseRandomAccessComponents(system);

		//One more entities.
		auto parsedRequestEntities = parseProcessesEntities(system);

		if (parsedRequestEntities.empty() && !parsedIncludes.empty()) {
			//OneEntity.

			ParsedSystemECSFile::RequestEntity requestEntity{
				.includes_ = parsedIncludes,
				.excludes_ = parsedExcludes,
				.creates_ = parsedCreatesComponents,
				.removes_ = parsedRemovesComponents,
				.randomAccessComponents_ = randomAccessComponents,
			};
			requestEntities.push_back(requestEntity);
		}
		else {
			//One more entities.
			OS::AssertMessage(parsedIncludes.empty(), "");
			requestEntities.insert(
				requestEntities.end(),
				std::make_move_iterator(parsedRequestEntities.begin()),
				std::make_move_iterator(parsedRequestEntities.end()));
		}

		auto runAfterSystems = parseRunAfter(system);
		auto runBeforeSystems = parseRunBefore(system);

		if (ecsFilePath.filename() == "OksEngine.CreateRenderDriver.ecs") {
			//__debugbreak();
		}

		auto createsEntities = parseCreatesEntities(system);
		auto accessesEntities = parseAccessesEntities(system);

		ParsedSystemECSFile::CreateInfo ci{
			.name_ = system["name"].cast<std::string>().value(),
			.path_ = ecsFilePath,
			.processesEntities_ = requestEntities,
			.createsEntities_ = createsEntities,
			.accessesEntities_ = accessesEntities,
			.afterUpdateMethod_ = parseAfterUpdateMethod(system),
			.thread_ = GetThread(context),
			.type_ = systemType,
			.runAfter_ = runAfterSystems,
			.runBefore_ = runBeforeSystems
		};

		auto parsedSystemFile = std::make_shared<ParsedSystemECSFile>(ci);
		return parsedSystemFile;
	}

	class SystemFileStructureGenerator {
	public:

		struct CreateInfo {
			std::string includeDirectory_;
		};

		SystemFileStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> GenerateSystemRealization(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

			auto generateIncludes = [](std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
				File::Includes includes{ };


				const std::string systemName = systemEcsFile->GetName();
				std::filesystem::path systemsIncludesFilePath;
				std::filesystem::path systemFullPath = systemEcsFile->GetPath();
				systemsIncludesFilePath = systemFullPath.parent_path();
				std::filesystem::path systemIncludePath = GetSubPath(
					systemFullPath.parent_path(),
					projectContext->includeDirectory_,
					ResultRange::FromStartFolderToEnd,
					SearchDirection::FromEndToBegin,
					true);

				systemIncludePath /= ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
				includes.paths_.insert(systemIncludePath);

				return includes;
				};

			const File::Includes includes = generateIncludes(projectContext, systemEcsFile);
			
			auto namespaceObject = std::make_shared<Namespace>("OksEngine");
			{
				using namespace std::string_literals;

				auto generateUpdateMethod = [](std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
					
					auto generateUpdateMethodParameters =
						[](
							std::shared_ptr<ProjectContext> projectContext,
							std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

								std::vector<Function::Parameter> updateMethodParameters;

								Common::UInt64 currentEntityIndex = 0;
								systemEcsFile->ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {

									updateMethodParameters.push_back({
										"ECS2::Entity::Id",
										std::format("entity{}id", currentEntityIndex) });

									entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
										Function::Parameter parameter;
										if (include.IsReadonly()) {
											parameter.inputType_ = std::format("const {}*", include.GetName());
										}
										else {
											parameter.inputType_ = std::format("{}*", include.GetName());
										}

										parameter.valueName_ = std::format("{}{}", include.GetLowerName(), currentEntityIndex);
										updateMethodParameters.push_back(parameter);
										return true;
										});

									++currentEntityIndex;
									return true;
									});

								return updateMethodParameters;

						};
					
					//Update method.
					const std::vector<Function::Parameter> updateMethodParameters
						= generateUpdateMethodParameters(projectContext, systemEcsFile);

					////TODO: process more than 2 entities.
					//for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_.size(); i++) {

					//	std::string entityIdString = "";
					//	if (systemEcsFile->ci_.processesEntities_.size() == 2) {
					//		entityIdString = (i == 0) ? ("1") : ("2");
					//	}

					//	updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
					//	const ParsedSystemECSFile::RequestEntity& entity = systemEcsFile->ci_.processesEntities_[i];
					//	entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
					//		std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
					//		updateMethodParameters.push_back({
					//			((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
					//			lowerIncludeName });

					//		return true;
					//		});
					//}

					Function::CreateInfo updateMethodCI{
						.name_ = systemEcsFile->GetName() + "::Update",
						.parameters_ = updateMethodParameters,
						.returnType_ = "void",
						.code_ = "",
						.isPrototype_ = false,
						.inlineModifier_ = false
					};

					auto updateMethod = std::make_shared<Function>(updateMethodCI);

					return updateMethod;
					};

				namespaceObject->Add(generateUpdateMethod(projectContext, systemEcsFile));

				auto generateAfterUpdateMethod = [](
					std::shared_ptr<ProjectContext> projectContext,
					std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

						Function::CreateInfo afterUpdateMethodCI{
							.name_ = systemEcsFile->GetName() + "::AfterUpdate",
							.parameters_ = { },
							.returnType_ = "void",
							.code_ = "",
							.isPrototype_ = false,
							.inlineModifier_ = false
						};

						auto afterUpdateMethod = std::make_shared<Function>(afterUpdateMethodCI);

						return afterUpdateMethod;

					};

				if (systemEcsFile->ci_.afterUpdateMethod_) {
					namespaceObject->Add(generateAfterUpdateMethod(projectContext, systemEcsFile));
				}
			}

			File::CreateInfo fci{
				.isHpp_ = false,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			std::filesystem::path componentHppFileFullPath
				= systemEcsFile->GetPath().parent_path() / ("OksEngine." + systemEcsFile->GetName() + ".cpp");
			std::string componentHppFileFullPathString = componentHppFileFullPath.string();
			std::replace(componentHppFileFullPathString.begin(), componentHppFileFullPathString.end(), '\\', '/');
			return { std::filesystem::path{ componentHppFileFullPathString }, file };

		}


		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> GenerateECSCXXFilesStructure(
				std::shared_ptr<ProjectContext> projectContext,
				std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
			{
			// // ������� CompilerInstance
			// clang::CompilerInstance CI;
			// CI.createDiagnostics();
			// CI.getLangOpts().CPlusPlus = true; // �������� ��������� C++

			// // ������� ASTContext
			// CI.createASTContext();

			// // ������� Rewriter ��� ��������� ����
			// clang::Rewriter R;
			// R.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());

			// // ������� TranslationUnitDecl (�������� ���� AST)
			// clang::ASTContext& AST = CI.getASTContext();
			// clang::TranslationUnitDecl* TU = AST.getTranslationUnitDecl();

			// // ��������� ��������� �������������
			// R.InsertText(clang::SourceLocation(), "#pragma once\n");
			// R.InsertText(clang::SourceLocation(), "#include <Common.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <ECS2.Component.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <ECS2.Entity.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <ECS2.World.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <Lua.Context.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <glm/glm.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <glm/gtc/quaternion.hpp>\n");
			// R.InsertText(clang::SourceLocation(), "#include <imgui.h>\n");
			// R.InsertText(clang::SourceLocation(), "#include <string>\n");
			// R.InsertText(clang::SourceLocation(), "#include <vector>\n\n");

			// // ������� ������������ ���� OksEngine
			// clang::NamespaceDecl* OksEngine = clang::NamespaceDecl::Create(
			// 	AST, TU, false, clang::SourceLocation(), clang::SourceLocation(),
			// 	&AST.Idents.get("OksEngine"), nullptr, false);

			// // ������� ��������� Animation
			// clang::RecordDecl* Animation = clang::CXXRecordDecl::Create(
			// 	AST, clang::TagDecl::TagKind::Struct, OksEngine, clang::SourceLocation(),
			// 	clang::SourceLocation(), &AST.Idents.get("Animation"));

			// // ��������� ������� ����� ECS2::IComponent<Animation>
			// clang::QualType BaseType = AST.getTemplateSpecializationTypeInfo(
			// 	AST.getRecordType(AST.getRecordType(AST.getTypeDeclType(
			// 		AST.getTypeDecl(TU, clang::SourceLocation(), &AST.Idents.get("ECS2")))),
			// 		{ AST.getTypeDeclType(Animation) });
			// clang::CXXBaseSpecifier * BaseSpec = clang::CXXBaseSpecifier::Create(
			// 	AST, clang::SourceLocation(), false, false, BaseType, clang::SourceLocation(),
			// 	clang::SourceLocation());
			// Animation->setBases(BaseSpec, 1);

			// // ��������� ����������� �� ���������
			// clang::CXXConstructorDecl * DefaultCtor = clang::CXXConstructorDecl::Create(
			// 	AST, Animation, clang::SourceLocation(), clang::DeclarationName(), clang::QualType(),
			// 	nullptr, clang::Function::FunctionKind::FK_DefaultConstructor, false, false,
			// 	false, false);
			// DefaultCtor->setAccess(clang::AccessSpecifier::AS_public);
			// Animation->addDecl(DefaultCtor);

			// // ��������� ����
			// clang::FieldDecl * NameField = clang::FieldDecl::Create(
			// 	AST, Animation, SourceLocation(), SourceLocation(),
			// 	&AST.Idents.get("name_"), AST.getStdStringType(), nullptr, nullptr,
			// 	false, false);
			// NameField->setAccess(clang::AccessSpecifier::AS_private);
			// Animation->addDecl(NameField);

			// clang::FieldDecl * DurationField = clang::FieldDecl::Create(
			// 	AST, Animation, SourceLocation(), SourceLocation(),
			// 	&AST.Idents.get("durationInTicks_"), AST.getDoubleType(), nullptr,
			// 	nullptr, false, false);
			// DurationField->setAccess(clang::AccessSpecifier::AS_private);
			// Animation->addDecl(DurationField);

			// // ��������� ����������� � �����������
			// QualType ParamTypes[] = {
			// 	AST.getStdStringType(),
			// 	AST.getDoubleType(),
			// 	AST.FloatTy,
			// 	AST.getTemplateSpecializationType(
			// 		AST.getRecordType(
			// 			AST.getRecordDecl(
			// 				AST.getTypeDeclType(
			// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
			// 				)
			// 			)
			// 		)
			// 	),
			// 	AST.getTemplateSpecializationType(
			// 		AST.getRecordType(
			// 			AST.getRecordDecl(
			// 				AST.getTypeDeclType(
			// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
			// 				)
			// 			)
			// 		)
			// 	),
			// 	AST.getTemplateSpecializationType(
			// 		AST.getRecordType(
			// 			AST.getRecordDecl(
			// 				AST.getTypeDeclType(
			// 					AST.getTypeDecl(TU, SourceLocation(), &AST.Idents.get("std::vector"))
			// 				)
			// 			)
			// 		)
			// 	)
			// };
			// CXXConstructorDecl * ParamCtor = CXXConstructorDecl::Create(
			// 	AST, Animation, SourceLocation(), DeclarationName(), QualType(),
			// 	nullptr, Function::FunctionKind::FK_Constructor, false, false, false,
			// 	false);
			// ParamCtor->setAccess(AccessSpecifier::AS_public);
			// Animation->addDecl(ParamCtor);

			// // ��������� ����� GetName
			// FunctionDecl * GetNameMethod = FunctionDecl::Create(
			// 	AST, Animation, SourceLocation(), SourceLocation(),
			// 	DeclarationName(&AST.Idents.get("GetName")), AST.getPointerType(AST.CharTy),
			// 	nullptr, Function::FunctionKind::FK_MemberFunction, false, false, false,
			// 	false);
			// GetNameMethod->setAccess(AccessSpecifier::AS_public);
			// Animation->addDecl(GetNameMethod);

			// // ���������� ���
			// Animation->print(R.getEditBuffer(CI.getSourceManager().getMainFileID()));

			// // ������� ��������������� ���
			// R.getEditBuffer(CI.getSourceManager().getMainFileID()).write(llvm::outs());
			}
			auto generateIncludes = [](
				std::shared_ptr<ProjectContext> projectContext,
				std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {


					File::Includes includes{ };
					includes.paths_.insert("ECS2.hpp");
					includes.paths_.insert("chrono");
					if (systemEcsFile->GetName() == "CreateLinksFromECSFiles") {
						systemEcsFile = systemEcsFile;
					}

					for (auto& createsEntities : systemEcsFile->ci_.createsEntities_) {

						for (auto& createsEntityComponent : createsEntities) {
							auto componentEcsFile = projectContext->GetEcsFileByName(createsEntityComponent);

							const std::filesystem::path includePath = GetSubPath(
								componentEcsFile->GetPath().parent_path(),
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							includes.paths_.insert(includePath / ("auto_OksEngine." + createsEntityComponent + ".hpp"));
						}
					}

					systemEcsFile->ForEachRandomAccessEntity([&](
						const ParsedSystemECSFile::RandomAccessEntity& entity,
						bool isLast) {

							entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {

								auto componentEcsFile = projectContext->GetEcsFileByName(include.GetName());

								const std::filesystem::path includePath = GetSubPath(
									componentEcsFile->GetPath().parent_path(),
									projectContext->includeDirectory_,
									ResultRange::FromStartFolderToEnd,
									SearchDirection::FromEndToBegin,
									true);
								includes.paths_.insert(includePath / ("auto_OksEngine." + include.GetName() + ".hpp"));
								return true;
								});

							return true;
						});



					for (auto& entity : systemEcsFile->ci_.processesEntities_) {



						//Generate includes for components that system uses as input.
						for (auto componentInclude : entity.includes_) {
							auto componentEcsFile = projectContext->GetEcsFileByName(componentInclude.name_);
							const std::filesystem::path includePath = GetSubPath(
								componentEcsFile->GetPath().parent_path(),
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);

							includes.paths_.insert(includePath / ("auto_OksEngine." + componentInclude.name_ + ".hpp"));
						}

						/*if(systemEcsFile->GetName() == "CreateImGuiInterface"){
							__debugbreak();
						}*/


						//Generate includes for components that system creates.
						for (auto componentCreates : entity.creates_) {
							auto componentEcsFile = projectContext->GetEcsFileByName(componentCreates);
							const std::filesystem::path includePath = GetSubPath(
								componentEcsFile->GetPath().parent_path(),
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							includes.paths_.insert(includePath / ("auto_OksEngine." + componentCreates + ".hpp"));
						}

						for (auto componentRemoves : entity.removes_) {
							auto componentEcsFile = projectContext->GetEcsFileByName(componentRemoves);
							const std::filesystem::path includePath = GetSubPath(
								componentEcsFile->GetPath().parent_path(),
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							includes.paths_.insert(includePath / ("auto_OksEngine." + componentRemoves + ".hpp"));
						}

						//Generate includes for components that system creates.
						for (auto componentExclude : entity.excludes_) {
							auto componentEcsFile = projectContext->GetEcsFileByName(componentExclude);
							const std::filesystem::path includePath = GetSubPath(
								componentEcsFile->GetPath().parent_path(),
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							includes.paths_.insert(includePath / ("auto_OksEngine." + componentExclude + ".hpp"));
						}

					}


					return includes;

				};
			auto generateGetComponentsFilter = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//CreateEntity method.
				Function::CreateInfo createDynamicEntityCI{
					.name_ = "GetComponentsFilter",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "ECS2::ComponentsFilter",
					.code_ = "return world_->GetEntityComponentsFilter(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto getEntityComponentsFilterMethod = std::make_shared<Function>(createDynamicEntityCI);

				return getEntityComponentsFilterMethod;
				};
			auto generateAfterUpdateMethodPrototype = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				Function::CreateInfo updateMethodCI{
					.name_ = "AfterUpdate",
					.parameters_ = { },
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				return updateMethod;

				};
			auto generateUpdateMethodPrototype = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				using namespace std::string_literals;
				//Update method.
				std::vector<Function::Parameter> updateMethodParameters;

				Common::UInt64 currentEntityIndex = 0;
				systemEcsFile->ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {

					updateMethodParameters.push_back({
						"ECS2::Entity::Id",
						std::format("entity{}id", currentEntityIndex) });

					entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
						Function::Parameter parameter;
						if (include.IsReadonly()) {
							parameter.inputType_ = std::format("const {}*", include.GetName());
						}
						else {
							parameter.inputType_ = std::format("{}*", include.GetName());
						}

						parameter.valueName_ = std::format("{}{}", include.GetLowerName(), currentEntityIndex);
						updateMethodParameters.push_back(parameter);
						return true;
						});

					++currentEntityIndex;
					return true;
					});

				//for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_.size(); i++) {

				//	std::string entityIdString = "";
				//	if (systemEcsFile->ci_.processesEntities_.size() == 2) {
				//		entityIdString = std::to_string(i);
				//	}

				//	updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
				//	const ParsedSystemECSFile::RequestEntity& entity = systemEcsFile->ci_.processesEntities_[i];
				//	entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
				//		std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
				//		updateMethodParameters.push_back({
				//			((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
				//			lowerIncludeName });

				//		return true;
				//		});
				//}

				//Add indices if two entities contain the same component.
				{
					for (Common::Index i = 0; i < updateMethodParameters.size(); i++) {
						Function::Parameter& iParameter = updateMethodParameters[i];
						for (Common::Index j = 0; j < updateMethodParameters.size(); j++) {
							Function::Parameter& jParameter = updateMethodParameters[j];
							if (i != j && iParameter.valueName_ == jParameter.valueName_) {
								iParameter.valueName_ += "1";
								jParameter.valueName_ += "2";
							}
						}
					}
				}

				//Add entity index to component names if there are two components with the same name.



				Function::CreateInfo updateMethodCI{
					.name_ = "Update",
					.parameters_ = updateMethodParameters,
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				return updateMethod;

				};
			auto generateCreateDynamicEntityRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//CreateEntity method.
				Function::CreateInfo createDynamicEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "return world_->CreateEntity();",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto createDynamicEntityMethod = std::make_shared<Function>(createDynamicEntityCI);

				return createDynamicEntityMethod;
				};
			auto generateDestroyEntityRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//CreateEntity method.
				Function::CreateInfo createDynamicEntityCI{
					.name_ = "DestroyEntity",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }},
					.returnType_ = "void",
					.code_ = "world_->DestroyEntity(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto createDynamicEntityMethod = std::make_shared<Function>(createDynamicEntityCI);

				return createDynamicEntityMethod;
				};
			auto generateCreateArchetypeEntityRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {


				//CreateEntity<> method.
				Function::CreateInfo createArchetypeEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "return world_->CreateEntity<Components...>();",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto createArchetypeEntityMethod = std::make_shared<Function>(createArchetypeEntityCI);

				return createArchetypeEntityMethod;
				};
			auto generateGetComponentMethodRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				Code getComponentCode;
				{
					if (!systemEcsFile->ci_.accessesEntities_.empty()) {
						//Add Assert.
						getComponentCode.Add("OS::AssertMessage(");
						systemEcsFile->ForEachRandomAccessComponent(
							[&](const std::string& componentName, bool isLast) {
								getComponentCode.Add(std::format("std::is_same_v<Component, {}>", componentName));
								if (!isLast) {
									getComponentCode.Add(" || ");
								}
								return true;
							});
						getComponentCode.Add(
							",std::format(\"Attempt to access component{} that system("
							+ systemEcsFile->GetName() +
							") can't access. Added access entities description to .ecs file that corresponds to system\", Component::GetName()));"
						);
					}
					getComponentCode.Add("return world_->GetComponent<Component>(entityId);");
				}
				Function::CreateInfo getComponentCI{
					.name_ = "GetComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "Component*",
					.code_ = getComponentCode,
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component" }
				};

				auto getComponentMethod = std::make_shared<Function>(getComponentCI);

				return getComponentMethod;
				};
			auto generateCreateComponentMethodRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//CreateComponent method.
				Function::CreateInfo createComponentCI{
					.name_ = "CreateComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
					.returnType_ = "void",
					.code_ = "world_->CreateComponent<Component>(entityId, std::forward<Args>(args)...);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component", "...Args" }
				};

				auto createComponentMethod = std::make_shared<Function>(createComponentCI);

				return createComponentMethod;
				};
			auto generateRemoveComponentMethodRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//CreateComponent method.
				Function::CreateInfo removeComponentCI{
					.name_ = "RemoveComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
					.returnType_ = "void",
					.code_ = "world_->RemoveComponent<Component>(entityId, std::forward<Args>(args)...);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component", "...Args" }
				};

				auto removeComponentMethod = std::make_shared<Function>(removeComponentCI);

				return removeComponentMethod;
				};
			auto generateIsComponentExistMethodRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//IsComponentExist method.
				Function::CreateInfo isComponentExistCI{
					.name_ = "IsComponentExist",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "bool",
					.code_ = "return world_->IsComponentExist<Component>(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component" }
				};

				auto isComponentExistMethod = std::make_shared<Function>(isComponentExistCI);

				return isComponentExistMethod;
				};
			auto generateIsEntityExistMethodRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//IsEntityExist method.
				Function::CreateInfo isEntityExistCI{
					.name_ = "IsEntityExist",
					.parameters_ = { },
					.returnType_ = "bool",
					.code_ = "return world_->IsEntityExist<Components...>();",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto isEntityExistMethod = std::make_shared<Function>(isEntityExistCI);

				return isEntityExistMethod;
				};
			auto generateIsEntityExist2MethodRealization = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				//IsEntityExist method.
				Function::CreateInfo isEntityExistCI{
					.name_ = "IsEntityExist",
					.parameters_ = { { "ECS2::Entity::Id", "entityId"} },
					.returnType_ = "bool",
					.code_ = "return world_->IsEntityExist(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { }
				};

				auto isEntityExistMethod = std::make_shared<Function>(isEntityExistCI);

				return isEntityExistMethod;
				};

			File::Includes includes = generateIncludes(projectContext, systemEcsFile);

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");

			//namespaceObject->Add(generateUpdateMethodPrototype(systemEcsFile));

			std::vector<std::shared_ptr<Function>> methods{
				generateUpdateMethodPrototype(systemEcsFile),
				generateCreateComponentMethodRealization(systemEcsFile),
				generateGetComponentsFilter(systemEcsFile),
				generateRemoveComponentMethodRealization(systemEcsFile),
				generateGetComponentMethodRealization(systemEcsFile),
				generateIsComponentExistMethodRealization(systemEcsFile),
				generateCreateDynamicEntityRealization(systemEcsFile),
				generateDestroyEntityRealization(systemEcsFile),
				generateCreateArchetypeEntityRealization(systemEcsFile),
				generateIsEntityExistMethodRealization(systemEcsFile),
				generateIsEntityExist2MethodRealization(systemEcsFile)
			};

			if (systemEcsFile->ci_.afterUpdateMethod_) { 
				methods.insert(methods.begin() + 1, generateAfterUpdateMethodPrototype(systemEcsFile));
			}

			Struct::CreateInfo sci{
				.name_ = systemEcsFile->GetName(),
				.parent_ = "",
				.fields_ = { { "std::shared_ptr<ECS2::World>", "world" } },
				.methods_ = methods
			};
			auto structObject = std::make_shared<Struct>(sci);

			namespaceObject->Add(structObject);

			auto generateRunSystemCode = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				auto generateUpdateMethodCallCode = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

					Code realization;

					Common::UInt64 currentEntityIndex = 0;
					realization.Add(systemEcsFile->GetLowerName() + ".Update(");
					systemEcsFile->ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {

						realization.Add(std::format("entity{}Id", currentEntityIndex));

						if (!entity.includes_.empty()) {
							realization.Add(", ");
						}
						realization.NewLine();
						entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(std::format("{}{}",
								include.GetLowerName(), currentEntityIndex));
							if (!isLast) {
								realization.Add(", ");
							}
							return true;
							});
						if (!isLast) {
							realization.Add(", ");
						}
						++currentEntityIndex;
						return true;
						});

					realization.Add(");");

					return realization;
					};


				Code realization;
				//Render render{ world };
				realization.Add(std::format(
					"{} {}{{ world }};",
					systemEcsFile->GetName(),
					systemEcsFile->GetLowerName()
				));
				Common::UInt64 currentEntityIndex = 0;
				systemEcsFile->ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {

					//Generate exclude.
					{
						realization.Add(std::format("ECS2::ComponentsFilter excludeEntity{};", currentEntityIndex));
						realization.NewLine();
						if (!entity.excludes_.empty()) {
							realization.Add(std::format("excludeEntity{}.SetBits<", currentEntityIndex));
						}


						for (Common::Index i = 0; i < entity.excludes_.size(); ++i) {
							realization.Add(entity.excludes_[i]);
							if (i != entity.excludes_.size() - 1) {
								realization.Add(", ");
							}

						}
						if (!entity.excludes_.empty()) {
							realization.Add(">();");
							realization.NewLine();
						}
					}


					//Generate ForEachEntity
					if (entity.processesAllCombinations_) {
						realization.Add(std::format("world->ForEachEntity([&](ECS2::Entity::Id entity{}Id) {{", currentEntityIndex));

					}
					else {
						realization.Add("world->ForEachEntity<");
						realization.NewLine();
						entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(include.name_);
							if (!isLast) {
								realization.Add(", ");
							}
							realization.NewLine();
							return true;
							});
						realization.Add(std::format(">(excludeEntity{}, [&](", currentEntityIndex));
						realization.Add(std::format("ECS2::Entity::Id entity{}Id, ", currentEntityIndex));
						entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(std::format("{}*{}{}",

								include.name_, include.GetLowerName(), currentEntityIndex));
							if (!isLast) {
								realization.Add(", ");
							}
							realization.NewLine();
							return true;
							});
						realization.Add("){");
					}

					//Generate Update method if its last entity;
					if (currentEntityIndex == systemEcsFile->ci_.processesEntities_.size() - 1) {
						realization.Add(generateUpdateMethodCallCode(systemEcsFile));
					}

					++currentEntityIndex;
					return true;
					});

				systemEcsFile->ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {
					realization.Add("});");
					return true;
					});

				if (currentEntityIndex == 0) {
					//if (systemEcsFile->IsAllEntitiesSystem()) {
					//	realization.Add("world->ForEachEntity([&](ECS2::Entity::Id entityId) {");
					//	realization.Add(systemEcsFile->GetLowerName() + ".Update(entityId);");
					//	realization.Add("});");
					//}
					//else {
						//If there are no entities just call update method.
					realization.Add(systemEcsFile->GetLowerName() + ".Update();");
					//}
				}

				if (systemEcsFile->ci_.afterUpdateMethod_) {
					realization.Add(std::format("{}.AfterUpdate();", systemEcsFile->GetLowerName()));
				}

				return realization;
				};

			std::vector<Function::Parameter> parameters;
			parameters.push_back({ "std::shared_ptr<ECS2::World>", "world" });

			Code realization;
			
			realization.Add(generateRunSystemCode(systemEcsFile));


			Function::CreateInfo funcci{
				.name_ = systemEcsFile->GetName() + "System",
				.parameters_ = parameters,
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true

			};
			auto runSystem = std::make_shared<Function>(funcci);

			namespaceObject->Add(runSystem);


			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			std::filesystem::path systemHppFileFullPath
				= systemEcsFile->GetPath().parent_path() / ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
			std::string systemHppFileFullPathString = systemHppFileFullPath.string();
			std::replace(systemHppFileFullPathString.begin(), systemHppFileFullPathString.end(), '\\', '/');
			return { std::filesystem::path{ systemHppFileFullPathString }, file };


		}



		CreateInfo ci_;
	};


}