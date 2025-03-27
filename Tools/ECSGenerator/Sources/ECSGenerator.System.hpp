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

		};

		struct CreateInfo {
			std::string name_;
			std::filesystem::path path_;
			std::vector<Entity> entities_;
			std::vector<std::string> createEntityComponents_; // we dont need info: dynamic or archetype entity to create because we need generate only to add components includes.
			bool mainThreadCall_ = false;
			bool processAllEntities_ = false;
		};

		

		ParsedSystemECSFile(const CreateInfo& createInfo)
			: ParsedECSFile{ createInfo.path_, createInfo.name_ },
			ci_{ createInfo } { }

		virtual Type GetType() const noexcept override {
			return Type::System;
		}

		CreateInfo ci_;
	};

	//struct SystemInfo {
	//	std::filesystem::path ecsFilePath_;
	//	std::string name_;
	//	std::string lowerName_;

	//	struct Include {
	//		std::string componentName_;
	//		std::string lowerComponentName_;
	//	};

	//	std::vector<Include> includes_;
	//	std::vector<std::string> excludes_;

	//	using ProcessInclude = std::function<bool(const Include& include, bool isLast)>;

	//	void ForEachInclude(ProcessInclude&& processInclude) const {
	//		for (Common::Index i = 0; i < includes_.size(); i++) {
	//			const Include& include = includes_[i];
	//			if (!processInclude(include, (i == includes_.size() - 1))) {
	//				break;
	//			}
	//		}
	//	}

	//	const std::string& GetName() const {
	//		return name_;
	//	}

	//	const std::string& GetLowerName() const {
	//		return lowerName_;
	//	}

	//};

	enum class SystemType {
		OneEntity,
		TwoEntity,
		AllEntities,
		Undefined
	};

	std::shared_ptr<ParsedSystemECSFile> ParseSystemEcsFile(const std::filesystem::path& ecsFilePath, ::Lua::Context& context) {

		std::vector<ParsedSystemECSFile::Entity> parsedEntities;

		luabridge::LuaRef system = context.GetGlobalAsRef("system");


		std::vector<std::string> createEntityComponents;
		//Parse components need to create entity.
		{
			luabridge::LuaRef createEntityComponent = system["createsEntity"];
			if (!createEntityComponent.isNil()) {
				for (luabridge::Iterator it(createEntityComponent); !it.isNil(); ++it) {
					luabridge::LuaRef componentName = it.value();
					createEntityComponents.push_back(componentName.cast<std::string>().value());
				}
			}
		}
		
		luabridge::LuaRef includes = system["includes"];
		luabridge::LuaRef processAllEntities = system["processAllEntities"];

		SystemType systemType = SystemType::Undefined;

		if (!processAllEntities.isNil()) {
			systemType = SystemType::AllEntities;
		}
		else if (!includes.isNil()) {
			systemType = SystemType::OneEntity;
		}

		if (systemType == SystemType::OneEntity) {
			std::vector<ParsedSystemECSFile::Include> parsedIncludes;

			for (luabridge::Iterator it(includes); !it.isNil(); ++it) {
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
		else if (systemType == SystemType::AllEntities) {

		}
		else {
			//Two entities system.
#pragma region Assert
			OS::AssertMessage(!system["entities"].isNil(), "");
#pragma endregion
			luabridge::LuaRef entities = system["entities"];
			for (luabridge::Iterator it(entities); !it.isNil(); ++it) {
				luabridge::LuaRef entity = it.value();

				//ENTITY INCLUDES
				luabridge::LuaRef entityIncludes = entity["includes"];

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
				luabridge::LuaRef entityCreates = entity["creates"];

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
			.entities_ = parsedEntities,
			.createEntityComponents_ = createEntityComponents
		};

		//Nain thread call.
		if (
			luabridge::LuaRef mainThreadCallRef = system["mainThreadCall"];
			!mainThreadCallRef.isNil() &&
			mainThreadCallRef.isBool() &&
			mainThreadCallRef.cast<bool>().value()) {
			ci.mainThreadCall_ = true;
		}

		//Process all entities.
		if (
			luabridge::LuaRef mainThreadCallRef = system["processAllEntities"];
			!mainThreadCallRef.isNil() &&
			mainThreadCallRef.isBool() &&
			mainThreadCallRef.cast<bool>().value()) {
			ci.processAllEntities_ = true;
		}

		auto parsedSystemFile = std::make_shared<ParsedSystemECSFile>(ci);
		OS::AssertMessage(std::isupper(parsedSystemFile->GetName()[0]), "First system name symbol must be uppercase.");
		return parsedSystemFile;
	}


}