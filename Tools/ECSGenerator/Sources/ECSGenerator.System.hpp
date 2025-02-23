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




		const bool isOneEntitySystem = !includes.isNil();

		if (isOneEntitySystem) {
			std::vector<ParsedSystemECSFile::Include> parsedIncludes;

			for (luabridge::Iterator it(includes); !it.isNil(); ++it) {
				luabridge::LuaRef include = it.value();
				ParsedSystemECSFile::Include parsedInclude;
				if (!include.isNil()) {
					luabridge::LuaRef includeName = include["name"];
					if (!includeName.isNil()) {
						parsedInclude.name_ = includeName.cast<std::string>().value();
					}
					luabridge::LuaRef isIncludeReadonly = include["readonly"];
					if (!isIncludeReadonly.isNil()) {
						parsedInclude.readonly_ = isIncludeReadonly.cast<bool>().value();
					}
					parsedInclude.name_ = include.cast<std::string>().value();
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

		auto parsedSystemFile = std::make_shared<ParsedSystemECSFile>(ci);
		OS::AssertMessage(std::isupper(parsedSystemFile->GetName()[0]), "First system name symbol must be uppercase.");
		return parsedSystemFile;
	}


	//std::string GenerateSystemRunFunction(const SystemInfo& system) {
	//	std::string code;

	//	using namespace ECSGenerator;
	//	std::vector<Function::Parameter> parameters;
	//	parameters.push_back({ "ECS2::World*", "world" });

	//	std::string realization;
	//	realization += system.GetName() + " " + system.GetLowerName() + ";\n";
	//	realization += "world->ForEachEntity<";
	//	system.ForEachInclude([&](const SystemInfo::Include& include, bool isLast) {
	//		realization += include.componentName_;
	//		if (!isLast) {
	//			realization += ", ";
	//		}
	//		return true;
	//		});
	//	realization += ">(ECS2::ComponentsFilter{}, [&](";
	//	system.ForEachInclude([&](const SystemInfo::Include& include, bool isLast) {
	//		realization += include.componentName_ + "* " + include.lowerComponentName_;
	//		if (!isLast) {
	//			realization += ", ";
	//		}
	//		return true;
	//		});
	//	realization += "){\n";
	//	realization += "\n" + system.GetLowerName() + ".Update(";
	//	system.ForEachInclude([&](const SystemInfo::Include& include, bool isLast) {
	//		realization += include.lowerComponentName_;
	//		if (!isLast) {
	//			realization += ", ";
	//		}
	//		return true;
	//		});
	//	realization += ");\n";

	//	realization += "});\n";
	//	//UpdateClock updateClock;
	//	//world->ForEachEntity<Clock>(ECS2::ComponentsFilter{}, [&](Clock* clock) {
	//	//	});
	//	/*Function::CreateInfo fci{
	//		.name_ = "Run" + system.name_ + "System",
	//		.parameters_ = parameters,
	//		.returnType_ = "void",
	//		.code_ = realization,
	//		.inlineModifier_ = true

	//	};
	//	Function function{ fci };
	//	code += function.GenerateRealization();*/

	//	return code;
	//}


	//std::string GenerateUpdateFunctionPrototype(const SystemInfo& system) {
	//	std::string code;

	//	code += "\t\tvoid Update(";

	//	system.ForEachInclude([&](const SystemInfo::Include& include, bool isLast) {
	//		code += include.componentName_ + "* " + include.lowerComponentName_;
	//		if (!isLast) {
	//			code += ", ";
	//		}

	//		return true;
	//		});

	//	code += ");\n";
	//	return code;
	//}


	////	std::string GetName() const noexcept override {
	////		return "UpdateClock";
	////	}
	//std::string GenerateGetNameFunction(const SystemInfo& system) {
	//	std::string code;

	//	code += "\t\tstd::string GetName() const noexcept override {\n";
	//	code += "\t\t\treturn \"" + system.GetName() + "\";\n";
	//	code += "\t\t};\n";

	//	return code;
	//}

	////	virtual Common::TypeId GetTypeId() const noexcept override {
	////		return Common::TypeInfo<UpdateClock>().GetId();
	////	}
	//std::string GenerateGetTypeIdFunction(const SystemInfo& system) {
	//	std::string code;

	//	code += "\t\tvirtual Common::TypeId GetTypeId() const noexcept override {\n";
	//	code += "\t\t\treturn Common::TypeInfo<" + system.GetName() + ">().GetId();\n";
	//	code += "\t\t};";

	//	return code;
	//}


	////struct UpdateClock : public ECS::System2 {

	//	//	void Update(Clock* clock);

	//	//	[[nodiscard]]
	//	//	std::string GetName() const noexcept override {
	//	//		return "UpdateClock";
	//	//	}

	//	//	virtual Common::TypeId GetTypeId() const noexcept override {
	//	//		return Common::TypeInfo<UpdateClock>().GetId();
	//	//	}

	//	//};

	//std::string GenerateSystemClassDefinition(const SystemInfo& system) {
	//	std::string code = "\n";



	//	code += "\tclass " + system.GetName() + ": public ECS::System2" + "{\n";
	//	code += "public:\n";
	//	code += "\n";
	//	//system.ForEachInclude([&](const SystemInfo::Include& include, bool isLast) {
	//	//	code += "\t\tclass " + include.componentName_ + ";\n";
	//	//	return true;
	//	//	});

	//	code += "\n";

	//	code += GenerateUpdateFunctionPrototype(system);
	//	code += "\n";
	//	code += GenerateGetNameFunction(system);
	//	code += "\n";
	//	code += GenerateGetTypeIdFunction(system);
	//	code += "\n";

	//	code += "\t};\n";

	//	return code;
	//}




	//std::string GenerateComponentIncludes(const SystemInfo& system) {

	//	//C:/Users/User/Desktop/OksEngine/Sources/Engine\Sources\Common\Clock\OksEngine.UpdateClock.ecs
	//	// ->
	//	//Sources\Common\Clock\

	//	auto getPath = [](const std::filesystem::path& fullPath) {
	//		const std::filesystem::path filePath = fullPath.parent_path();
	//		bool sourcesFound = false;
	//		std::filesystem::path result;
	//		std::filesystem::path::iterator sourcesIt;
	//		for (auto it = filePath.end(); it != filePath.begin(); --it) {
	//			if (*it == "Sources") {
	//				sourcesIt = it;
	//				break;
	//			}
	//		}
	//		for (auto it = ++sourcesIt;//skip Sources directory
	//			it != filePath.end();
	//			++it) {
	//			result /= *it;
	//		}
	//		return result;
	//		};

	//	std::filesystem::path projectPath = getPath(system.ecsFilePath_);

	//	std::string code = "\n";
	//	for (auto componentInclude : system.includes_) {
	//		const std::filesystem::path componentHPPIncludePath
	//			= projectPath / ("auto_OksEngine." + componentInclude.componentName_ + ".hpp");
	//		std::string componentHPPIncludePathString = componentHPPIncludePath.string();
	//		std::replace(componentHPPIncludePathString.begin(), componentHPPIncludePathString.end(), '\\', '/');
	//		code += "#include <" + componentHPPIncludePathString + ">\n";
	//	}
	//	return code;
	//}


}