#pragma once 

#include <filesystem>
#include <string>
#include <functional>

#include <Common.hpp>

#include <ECSGenerator.Code.hpp>
#include <ECSGenerator.ParsedECSFile.hpp>

namespace ECSGenerator {


	class ParsedSystemECSFile : public ParsedECSFile {
	public:

		using Exclude = std::string;

		struct Include {
			std::string name_;
			bool readonly_ = true;
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

		};

		enum class Thread {
			Main,
			Child,
			Undefined
		};

		enum class SystemType {
			OneEntity,
			TwoEntities,
			AllEntities,
			Initialize,
			Undefined
		};

		struct CreateInfo {
			std::string name_;
			std::filesystem::path path_;
			std::vector<Entity> processesEntities_;
			std::vector<std::string> createEntityComponents_; // we dont need info: dynamic or archetype entity to create because we need generate only to add components includes.
			Thread thread_ = Thread::Undefined;
			SystemType type_ = SystemType::Undefined;
		};

		

		ParsedSystemECSFile(const CreateInfo& createInfo)
			: ParsedECSFile{ createInfo.path_, createInfo.name_ },
			ci_{ createInfo } { }


		using ProcessEntity= std::function<bool(const Entity& entity, bool isLast)>;

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

		SystemType GetSystemType() {
			return ci_.type_;
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

#pragma region Assert
				OS::AssertMessage(length == 1 || length == 2, "");
#pragma endregion
				if (length == 1) {
					systemType = ParsedSystemECSFile::SystemType::OneEntity;
				}
				else {
					systemType = ParsedSystemECSFile::SystemType::TwoEntities;
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
		else if(systemType == ParsedSystemECSFile::SystemType::TwoEntities){
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
				ParsedSystemECSFile::Entity parsedEntity{
					.includes_ = parsedEntityIncludes,
					.excludes_ = parsedEntityExcludes,
					.creates_ = parsedEntityCreates
				};
				parsedEntities.push_back(parsedEntity);
			}
		}



		ParsedSystemECSFile::CreateInfo ci{
			.name_ = system["name"].cast<std::string>().value(),
			.path_ = ecsFilePath,
			.processesEntities_ = parsedEntities,
			.createEntityComponents_ = createEntityComponents
		};

		ci.thread_ = GetThread(context);
		ci.type_ = systemType;

		auto parsedSystemFile = std::make_shared<ParsedSystemECSFile>(ci);
		OS::AssertMessage(std::isupper(parsedSystemFile->GetName()[0]), "First system name symbol must be uppercase.");
		return parsedSystemFile;
	}


}