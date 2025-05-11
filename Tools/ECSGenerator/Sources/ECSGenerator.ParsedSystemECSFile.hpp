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
		};

		struct Entity {
			std::vector<Include> includes_;
			std::vector<Exclude> excludes_;
			std::vector<std::string> creates_;

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
			AllEntities,
			Initialize,
			Undefined
		};

		struct CreateInfo {
			std::string name_;
			std::filesystem::path path_;
			std::vector<Entity> processesEntities_;
			std::vector<std::string> createEntityComponents_; // we dont need info: dynamic or archetype entity to create because we need generate only to add components includes.
			std::vector<std::string> accessEntityComponents_;
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
			auto itResult = std::find(
				ci_.createEntityComponents_.begin(),
				ci_.createEntityComponents_.end(),
				componentName);
			return itResult != ci_.createEntityComponents_.end();
		}

		bool IsAccessesComponent(const std::string& componentName) {
			auto itResult = std::find(
				ci_.accessEntityComponents_.begin(),
				ci_.accessEntityComponents_.end(),
				componentName);
			return itResult != ci_.accessEntityComponents_.end();
		}

		void ForEachCreateComponent(ProcessComponentName&& processComponent) const {
			for (Common::Index i = 0; i < ci_.createEntityComponents_.size(); i++) {
				const std::string& componentName = ci_.createEntityComponents_[i];
				if (!processComponent(componentName, (i == ci_.createEntityComponents_.size() - 1))) {
					break;
				}
			}
		}

		void ForEachAccessComponent(ProcessComponentName&& processComponent) const {
			for (Common::Index i = 0; i < ci_.accessEntityComponents_.size(); i++) {
				const std::string& componentName = ci_.accessEntityComponents_[i];
				if (!processComponent(componentName, (i == ci_.accessEntityComponents_.size() - 1))) {
					break;
				}
			}
		}

		using ProcessEntity = std::function<bool(const Entity& entity, bool isLast)>;

		void ForEachEntity(ProcessEntity&& processEntity) const {
			for (Common::Index i = 0; i < ci_.processesEntities_.size(); i++) {
				const Entity& entity = ci_.processesEntities_[i];
				if (!processEntity(entity, (i == ci_.processesEntities_.size() - 1))) {
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

		[[nodiscard]]
		bool IsAllEntitiesSystem() const {
			return GetSystemType() == SystemType::AllEntities;
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
			ForEachEntity([&](const ParsedSystemECSFile::Entity& entity, bool isLast) {
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
			ForEachEntity([&](const ParsedSystemECSFile::Entity& entity, bool isLast) {
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
			ForEachEntity([&](const ParsedSystemECSFile::Entity& entity, bool isLast) {
				if (entity.IsChangesComponent(component)) {
					isChangesComponent = true;
					return false;
				}
				return true;
				});

			return isChangesComponent;

		}


		bool IsDependsFromSystem(const std::shared_ptr<ParsedSystemECSFile> system) const {

			/*if (IsAllEntitiesSystem()) {
				return true;
			}*/

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

			ForEachEntity([&](const Entity& entity, bool isLast) {
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
					
					if (system->IsAccessesComponent(include.name_)) {
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

		luabridge::LuaRef createsEntity = system["createsEntity"];
		luabridge::LuaRef processesComponents = system["processesComponents"];
		luabridge::LuaRef processesEntities = system["processesEntities"];
		luabridge::LuaRef processAllEntities = system["processAllEntities"];
		luabridge::LuaRef initSystem = system["initSystem"];

		ParsedSystemECSFile::SystemType systemType = ParsedSystemECSFile::SystemType::Undefined;

		if (!initSystem.isNil()) {
			systemType = ParsedSystemECSFile::SystemType::Initialize;
#pragma region Assert
			OS::AssertMessage(!createsEntity.isNil(),
				"Invalid .ecs file. Initialize system must create entity.");
			OS::AssertMessage(processesComponents.isNil(),
				"Invalid .ecs file. Initialize system can not process components.");
			OS::AssertMessage(processAllEntities.isNil(),
				"Invalid .ecs file. Initialize system can not process all entities.");
#pragma endregion

		}
		else if (!processAllEntities.isNil()) {
			systemType = ParsedSystemECSFile::SystemType::AllEntities;
#pragma region Assert
			OS::AssertMessage(initSystem.isNil(),
				"Invalid .ecs file. System that process all entities can not be init system.");
#pragma endregion
		}
		else if (!processesComponents.isNil()) {
			systemType = ParsedSystemECSFile::SystemType::OneEntity;
#pragma region Assert
			//OS::AssertMessage(!createsEntity.isNil(), 
			//	"Invalid .ecs file. Initialize system must create entity.");
			OS::AssertMessage(processAllEntities.isNil(),
				"Invalid .ecs file. Initialize system can not process all entities.");
			OS::AssertMessage(processAllEntities.isNil(),
				"Invalid .ecs file. Initialize system can not process all entities.");
#pragma endregion
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

		std::vector<ParsedSystemECSFile::Entity> parsedEntities;

		luabridge::LuaRef system = context.GetGlobalAsRef("system");

		ParsedSystemECSFile::SystemType systemType = GetSystemType(context);

		std::vector<std::string> createEntityComponents;
		////Parse components need to create entity.
		//{
		//	if (!createEntityComponent.isNil()) {
		//		for (luabridge::Iterator it(createEntityComponent); !it.isNil(); ++it) {
		//			luabridge::LuaRef componentName = it.value();
		//			createEntityComponents.push_back(componentName.cast<std::string>().value());
		//		}
		//	}
		//}

		if (systemType == ParsedSystemECSFile::SystemType::OneEntity) {
			std::vector<ParsedSystemECSFile::Include> parsedIncludes;
			luabridge::LuaRef processesComponents = system["processesComponents"];
			if (!processesComponents.isNil()) {
				for (luabridge::Iterator it(processesComponents); !it.isNil(); ++it) {
					luabridge::LuaRef include = it.value();
					ParsedSystemECSFile::Include parsedInclude;
					if (!include.isNil()) {
						luabridge::LuaRef includeName = include["name"];
						if (!includeName.isNil()) {
							parsedInclude.name_ = includeName.cast<std::string>().value();
							luabridge::LuaRef isIncludeReadonly = include["readonly"];
							if (!isIncludeReadonly.isNil()) {
								parsedInclude.readonly_ = isIncludeReadonly.cast<bool>().value();
							}
						}
						else {
							parsedInclude.name_ = include.cast<std::string>().value();
						}


					}

					parsedIncludes.push_back(parsedInclude);

					OS::AssertMessage(std::isupper(parsedIncludes.back().name_[0]), "");
				}

				luabridge::LuaRef excludes = system["excludes"];
				std::vector<ParsedSystemECSFile::Exclude> parsedExcludes;
				if (!excludes.isNil()) {
					for (luabridge::Iterator it(excludes); !it.isNil(); ++it) {
						luabridge::LuaRef exclude = it.value();
						parsedExcludes.push_back(exclude.cast<std::string>().value());

						OS::AssertMessage(std::isupper(parsedExcludes.back()[0]), "");

					}
				}

				luabridge::LuaRef creates = system["creates"];

				std::vector<std::string> parsedCreates;
				if (!creates.isNil()) {
					for (luabridge::Iterator it(creates); !it.isNil(); ++it) {
						luabridge::LuaRef toCreate = it.value();
						parsedCreates.push_back(toCreate.cast<std::string>().value());

						OS::AssertMessage(std::isupper(parsedCreates.back()[0]), "");

					}
				}
				ParsedSystemECSFile::Entity parsedEntity{
					.includes_ = parsedIncludes,
					.excludes_ = parsedExcludes,
					.creates_ = parsedCreates
				};


				parsedEntities.push_back(parsedEntity);
			}
			else if (!system["processesEntities"].isNil()) {

				luabridge::LuaRef entities = system["processesEntities"];
				for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
					luabridge::LuaRef entity = it.value();

					//ENTITY INCLUDES
					luabridge::LuaRef entityIncludes = entity["processesComponents"];

					std::vector<ParsedSystemECSFile::Include> parsedEntityIncludes;

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

					//ENTITY EXCLUDES
					luabridge::LuaRef entityExcludes = entity["excludes"];
					std::vector<ParsedSystemECSFile::Exclude> parsedEntityExcludes;
					if (!entityExcludes.isNil()) {
						for (luabridge::Iterator it(entityExcludes); !it.isNil(); ++it) {
							luabridge::LuaRef exclude = it.value();
							parsedEntityExcludes.push_back(exclude.cast<std::string>().value());

							OS::AssertMessage(std::isupper(parsedEntityExcludes.back()[0]), "");

						}
					}


					//ENTITY CREATES
					luabridge::LuaRef entityCreates = entity["createsComponents"];

					std::vector<std::string> parsedEntityCreates;
					if (!entityCreates.isNil()) {
						for (luabridge::Iterator it(entityCreates); !it.isNil(); ++it) {
							luabridge::LuaRef toCreate = it.value();
							parsedEntityCreates.push_back(toCreate.cast<std::string>().value());
							OS::AssertMessage(std::isupper(parsedEntityCreates.back()[0]), "");
						}
					}
					ParsedSystemECSFile::Entity parsedEntity{
						.includes_ = parsedEntityIncludes,
						.excludes_ = parsedEntityExcludes,
						.creates_ = parsedEntityCreates
					};
					parsedEntities.push_back(parsedEntity);
				}
			}

			luabridge::LuaRef createsEntity = system["createsEntity"];
			if (!createsEntity.isNil()) {
				for (luabridge::Iterator it(createsEntity); !it.isNil(); ++it) {
					createEntityComponents.push_back(it.value().cast<std::string>().value());
				}
			}

		}
		else if (systemType == ParsedSystemECSFile::SystemType::AllEntities) {
			//Do nothing.
		}
		else if (systemType == ParsedSystemECSFile::SystemType::Initialize) {
			luabridge::LuaRef createsEntity = system["createsEntity"];
			for (luabridge::Iterator it(createsEntity); !it.isNil(); ++it) {
				createEntityComponents.push_back(it.value().cast<std::string>().value());
			}
		}
		else if (systemType == ParsedSystemECSFile::SystemType::OneMoreEntities) {
			//Two entities system.
#pragma region Assert
			OS::AssertMessage(!system["processesEntities"].isNil(), "");
#pragma endregion
			luabridge::LuaRef entities = system["processesEntities"];
			for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
				luabridge::LuaRef entity = it.value();

				//ENTITY INCLUDES
				luabridge::LuaRef entityIncludes = entity["processesComponents"];

				std::vector<ParsedSystemECSFile::Include> parsedEntityIncludes;

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

				//ENTITY EXCLUDES
				luabridge::LuaRef entityExcludes = entity["excludes"];
				std::vector<ParsedSystemECSFile::Exclude> parsedEntityExcludes;
				if (!entityExcludes.isNil()) {
					for (luabridge::Iterator it(entityExcludes); !it.isNil(); ++it) {
						luabridge::LuaRef exclude = it.value();
						parsedEntityExcludes.push_back(exclude.cast<std::string>().value());

						OS::AssertMessage(std::isupper(parsedEntityExcludes.back()[0]), "");

					}
				}

				//ENTITY CREATES
				luabridge::LuaRef entityCreates = entity["createsComponents"];

				std::vector<std::string> parsedEntityCreates;
				if (!entityCreates.isNil()) {
					for (luabridge::Iterator it(entityCreates); !it.isNil(); ++it) {
						luabridge::LuaRef toCreate = it.value();
						parsedEntityCreates.push_back(toCreate.cast<std::string>().value());
						OS::AssertMessage(std::isupper(parsedEntityCreates.back()[0]), "");
					}
				}

				luabridge::LuaRef createsEntity = system["createsEntity"];
				if (!createsEntity.isNil()) {
					for (luabridge::Iterator it(createsEntity); !it.isNil(); ++it) {
						createEntityComponents.push_back(it.value().cast<std::string>().value());
					}
				}

				ParsedSystemECSFile::Entity parsedEntity{
					.includes_ = parsedEntityIncludes,
					.excludes_ = parsedEntityExcludes,
					.creates_ = parsedEntityCreates
				};
				parsedEntities.push_back(parsedEntity);
			}
		}

		std::vector<std::string> runAfterSystems;
		std::vector<std::string> runBeforeSystems;
		{
			{
				luabridge::LuaRef runAfterRef = system["runAfter"];
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
			}
			{
				luabridge::LuaRef runBeforeRef = system["runBefore"];
				if (!runBeforeRef.isNil()) {
					if (runBeforeRef.isTable()) {
						for (luabridge::Iterator it(runBeforeRef); !it.isNil(); ++it) {
							luabridge::LuaRef runAfterSystemRef = it.value();
							runAfterSystems.push_back(runAfterSystemRef.cast<std::string>().value());
						}
					}
					else {
						runBeforeSystems.push_back(runBeforeRef.cast<std::string>().value());
					}
				}
			}

		}
		std::vector<std::string> parsedEntityAccessing;
		{

			//ENTITY CREATES
			luabridge::LuaRef accessingEntities = system["accessingEntities"];


			if (!accessingEntities.isNil()) {
				for (luabridge::Iterator it(accessingEntities); !it.isNil(); ++it) {
					luabridge::LuaRef toAccess = it.value();
					luabridge::LuaRef accessingComponents = toAccess["accessingComponents"];
					for (luabridge::Iterator itJ(accessingComponents); !itJ.isNil(); ++itJ) {
						luabridge::LuaRef accessingComponent = itJ.value();
						parsedEntityAccessing.push_back(accessingComponent.cast<std::string>().value());
						OS::AssertMessage(std::isupper(parsedEntityAccessing.back()[0]), "");
					}
					luabridge::LuaRef createsComponents = toAccess["createsComponents"];
					if (!createsComponents.isNil()) {
						for (luabridge::Iterator itJ(createsComponents); !itJ.isNil(); ++itJ) {
							luabridge::LuaRef createsComponent = itJ.value();
							createEntityComponents.push_back(createsComponent.cast<std::string>().value());
							OS::AssertMessage(std::isupper(createEntityComponents.back()[0]), "");
						}
					}
				}
			}
		}


		ParsedSystemECSFile::CreateInfo ci{
			.name_ = system["name"].cast<std::string>().value(),
			.path_ = ecsFilePath,
			.processesEntities_ = parsedEntities,
			.createEntityComponents_ = createEntityComponents,
			.accessEntityComponents_ = parsedEntityAccessing,
			.runAfter_ = runAfterSystems,
			.runBefore_ = runBeforeSystems
		};

		ci.thread_ = GetThread(context);
		ci.type_ = systemType;

		auto parsedSystemFile = std::make_shared<ParsedSystemECSFile>(ci);
		OS::AssertMessage(std::isupper(parsedSystemFile->GetName()[0]), "First system name symbol must be uppercase.");
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
			includes.paths_.push_back(systemIncludePath);


			auto namespaceObject = std::make_shared<Namespace>("OksEngine");
			{
				using namespace std::string_literals;
				//Update method.
				std::vector<Function::Parameter> updateMethodParameters;


				//If system  process all entities need only one paramter Entity::Id
				if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize) {
					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entityId" });
				}


				for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_.size(); i++) {

					std::string entityIdString = "";
					if (systemEcsFile->ci_.processesEntities_.size() == 2) {
						entityIdString = (i == 0) ? ("1") : ("2");
					}

					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
					const ParsedSystemECSFile::Entity& entity = systemEcsFile->ci_.processesEntities_[i];
					entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
						std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
						updateMethodParameters.push_back({
							((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
							lowerIncludeName });

						return true;
						});
				}

				Function::CreateInfo updateMethodCI{
					.name_ = systemEcsFile->GetName() + "::Update",
					.parameters_ = updateMethodParameters,
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				namespaceObject->Add(updateMethod);
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

			auto generateIncludes = [](
				std::shared_ptr<ProjectContext> projectContext,
				std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {


					File::Includes includes{ };
					includes.paths_.push_back("ECS2.hpp");
					includes.paths_.push_back("chrono");
					if (!systemEcsFile->ci_.createEntityComponents_.empty()) {
						for (auto& createEntityComponent : systemEcsFile->ci_.createEntityComponents_) {

							auto componentEcsFile = projectContext->GetEcsFileByName(createEntityComponent);

							const std::filesystem::path includePath = GetSubPath(
								componentEcsFile->GetPath().parent_path(),
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							includes.paths_.push_back(includePath / ("auto_OksEngine." + createEntityComponent + ".hpp"));
						}
					}

					if (!systemEcsFile->ci_.accessEntityComponents_.empty()) {
						for (auto& accessEntityComponent : systemEcsFile->ci_.accessEntityComponents_) {

							auto componentEcsFile = projectContext->GetEcsFileByName(accessEntityComponent);

							const std::filesystem::path includePath = GetSubPath(
								componentEcsFile->GetPath().parent_path(),
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							includes.paths_.push_back(includePath / ("auto_OksEngine." + accessEntityComponent + ".hpp"));
						}
					}

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

							includes.paths_.push_back(includePath / ("auto_OksEngine." + componentInclude.name_ + ".hpp"));
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
							includes.paths_.push_back(includePath / ("auto_OksEngine." + componentCreates + ".hpp"));
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
							includes.paths_.push_back(includePath / ("auto_OksEngine." + componentExclude + ".hpp"));
						}

					}

					return includes;

				};

			auto generateUpdateMethodPrototype = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

				using namespace std::string_literals;
				//Update method.
				std::vector<Function::Parameter> updateMethodParameters;

				//If system  process all entities need only one paramter Entity::Id
				if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::AllEntities) {
					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entityId" });
				}

				for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_.size(); i++) {

					std::string entityIdString = "";
					if (systemEcsFile->ci_.processesEntities_.size() == 2) {
						entityIdString = (i == 0) ? ("1") : ("2");
					}

					updateMethodParameters.push_back({ "ECS2::Entity::Id", "entity"s + entityIdString + "Id" });
					const ParsedSystemECSFile::Entity& entity = systemEcsFile->ci_.processesEntities_[i];
					entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
						std::string lowerIncludeName = std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1);
						updateMethodParameters.push_back({
							((include.readonly_) ? ("const ") : ("")) + include.name_ + "*",
							lowerIncludeName });

						return true;
						});
				}

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
					if (!systemEcsFile->ci_.accessEntityComponents_.empty()) {
						//Add Assert.
						getComponentCode.Add("OS::AssertMessage(");
						systemEcsFile->ForEachAccessComponent(
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


			File::Includes includes = generateIncludes(projectContext, systemEcsFile);

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");

			//namespaceObject->Add(generateUpdateMethodPrototype(systemEcsFile));




			Struct::CreateInfo sci{
				.name_ = systemEcsFile->GetName(),
				.parent_ = "",
				.fields_ = { { "std::shared_ptr<ECS2::World>", "world" } },
				.methods_ = { 
					generateUpdateMethodPrototype(systemEcsFile),
					generateCreateComponentMethodRealization(systemEcsFile),
					generateGetComponentMethodRealization(systemEcsFile),
					generateIsComponentExistMethodRealization(systemEcsFile),
					generateCreateDynamicEntityRealization(systemEcsFile),
					generateCreateArchetypeEntityRealization(systemEcsFile) 
				}
			};
			auto structObject = std::make_shared<Struct>(sci);



			namespaceObject->Add(structObject);


			auto generateRunSystemCode = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {


				auto generateUpdateMethodCallCode = [](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {

					Code realization;
					Common::UInt64 currentEntityIndex = 0;
					realization.Add(systemEcsFile->GetLowerName() + ".Update(");
					systemEcsFile->ForEachEntity([&](const ParsedSystemECSFile::Entity& entity, bool isLast) {

						realization.Add(std::format("entity{}Id", currentEntityIndex));
						realization.Add(", ");
						realization.NewLine();
						entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
							realization.Add(std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
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
				systemEcsFile->ForEachEntity([&](const ParsedSystemECSFile::Entity& entity, bool isLast) {

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
					//
					{
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
							realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
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

				systemEcsFile->ForEachEntity([&](const ParsedSystemECSFile::Entity& entity, bool isLast) {
					realization.Add("});");
					return true;
					});
				return realization;
				};

			std::vector<Function::Parameter> parameters;
			parameters.push_back({ "std::shared_ptr<ECS2::World>", "world" });

			Code realization;
			if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::OneEntity ||
				systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::OneMoreEntities) {
				realization.Add(generateRunSystemCode(systemEcsFile));
				//	//Render render{ world };
				//	realization.Add(std::format(
				//		"{} {}{{ world }};",
				//		systemEcsFile->GetName(),
				//		systemEcsFile->GetLowerName()
				//	));

				//	realization.NewLine();

				//	//Excludes
				//	realization.Add("ECS2::ComponentsFilter excludeEntity1;");
				//	realization.NewLine();
				//	if ((systemEcsFile->GetSystemType() == ParsedSystemECSFile::SystemType::OneEntity ||
				//		systemEcsFile->GetSystemType() == ParsedSystemECSFile::SystemType::TwoEntities)
				//		&& !systemEcsFile->ci_.processesEntities_[0].excludes_.empty()) {
				//		realization.Add("excludeEntity1.SetBits<");
				//	}


				//	for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_[0].excludes_.size(); ++i) {
				//		realization.Add(systemEcsFile->ci_.processesEntities_[0].excludes_[i]);
				//		if (i != systemEcsFile->ci_.processesEntities_[0].excludes_.size() - 1) {
				//			realization.Add(", ");
				//		}

				//	}
				//	if (!systemEcsFile->ci_.processesEntities_[0].excludes_.empty()) {
				//		realization.Add(">();");
				//		realization.NewLine();
				//	}

				//	realization.Add("world->ForEachEntity<");
				//	realization.NewLine();
				//	systemEcsFile->ci_.processesEntities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
				//		realization.Add(include.name_);
				//		if (!isLast) {
				//			realization.Add(", ");
				//		}
				//		realization.NewLine();
				//		return true;
				//		});
				//	realization.Add(">(excludeEntity1, [&](");
				//	realization.Add("ECS2::Entity::Id entity1Id, ");
				//	systemEcsFile->ci_.processesEntities_[0].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
				//		realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
				//		if (!isLast) {
				//			realization.Add(", ");
				//		}
				//		realization.NewLine();
				//		return true;
				//		});
				//	realization.Add("){");
				//	realization.NewLine();

				//	if (systemEcsFile->ci_.processesEntities_.size() == 1) {
				//		realization.Add(generateUpdateMethodCallCode(systemEcsFile));
				//	}
				//	else if (systemEcsFile->ci_.processesEntities_.size() == 3) {
				//		//Excludes
				//		realization.Add("ECS2::ComponentsFilter excludeEntity2;");
				//		if (!systemEcsFile->ci_.processesEntities_[1].excludes_.empty()) {
				//			realization.Add("excludeEntity2.SetBits<");
				//		}
				//		for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_[1].excludes_.size(); ++i) {
				//			realization.Add(systemEcsFile->ci_.processesEntities_[1].excludes_[i]);
				//			if (i != systemEcsFile->ci_.processesEntities_[1].excludes_.size() - 1) {
				//				realization.Add(", ");
				//			}

				//		}
				//		if (!systemEcsFile->ci_.processesEntities_[1].excludes_.empty()) {
				//			realization.Add(">();");
				//		}

				//		realization.Add("world->ForEachEntity<");
				//		systemEcsFile->ci_.processesEntities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
				//			realization.Add(include.name_);
				//			if (!isLast) {
				//				realization.Add(", ");

				//			}
				//			return true;
				//			});
				//		realization.Add(">(excludeEntity2, [&](");
				//		realization.Add("ECS2::Entity::Id entity2Id, ");
				//		realization.NewLine();
				//		systemEcsFile->ci_.processesEntities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
				//			realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
				//			if (!isLast) {
				//				realization.Add(", ");
				//			}
				//			return true;
				//			});
				//		realization.Add("){");
				//		{
				//		}
				//		realization.Add("});");
				//	}
				//}
				//else if (systemEcsFile->ci_.processesEntities_.size() == 2) {
				//	//Excludes
				//	realization.Add("ECS2::ComponentsFilter excludeEntity2;");
				//	realization.NewLine();
				//	if (!systemEcsFile->ci_.processesEntities_[1].excludes_.empty()) {
				//		realization.Add("excludeEntity2.SetBits<");
				//	}
				//	for (Common::Index i = 0; i < systemEcsFile->ci_.processesEntities_[1].excludes_.size(); ++i) {
				//		realization.Add(systemEcsFile->ci_.processesEntities_[1].excludes_[i]);
				//		if (i != systemEcsFile->ci_.processesEntities_[1].excludes_.size() - 1) {
				//			realization.Add(", ");
				//		}

				//	}
				//	if (!systemEcsFile->ci_.processesEntities_[1].excludes_.empty()) {
				//		realization.Add(">();");
				//		realization.NewLine();
				//	}

				//	realization.Add("world->ForEachEntity<");
				//	realization.NewLine();
				//	systemEcsFile->ci_.processesEntities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
				//		realization.Add(include.name_);
				//		if (!isLast) {
				//			realization.Add(", ");

				//		}
				//		realization.NewLine();
				//		return true;
				//		});
				//	realization.Add(">(excludeEntity2, [&](");
				//	realization.Add("ECS2::Entity::Id entity2Id, ");
				//	realization.NewLine();
				//	systemEcsFile->ci_.processesEntities_[1].ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
				//		realization.Add(include.name_ + "* " + std::string{ (char)std::tolower(include.name_[0]) } + include.name_.substr(1));
				//		if (!isLast) {
				//			realization.Add(", ");
				//		}
				//		return true;
				//		});
				//	realization.Add("){");
				//	realization.Add(generateUpdateMethodCallCode(systemEcsFile));
				//	realization.Add("});");
				//	realization.NewLine();

				//	realization.Add("});");
				//	realization.NewLine();
			}
			else if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::AllEntities) {
				realization.Add(systemEcsFile->GetName() + " " + systemEcsFile->GetLowerName() + "{ world };");
				realization.NewLine();
				realization.Add("world->ForEachEntity([&](ECS2::Entity::Id entityId){");
				realization.Add(systemEcsFile->GetLowerName() + ".Update(");
				realization.Add("entityId);");
				realization.Add("});");

			}
			else if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize) {

				realization.Add(std::format(
					"{} {}{{ world }};",
					systemEcsFile->GetName(),
					systemEcsFile->GetLowerName()
				));
				realization.Add(std::format(
					"{}.Update();",
					systemEcsFile->GetLowerName()
				));
			}

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