#pragma once 

#include <ECSGenerator2.ParsedECSFile.hpp>

#include <System/ECSGenerator2.SystemParser.hpp>
#include <Component/ECSGenerator2.ComponentParser.hpp>
#include <Struct/ECSGenerator2.StructParser.hpp>

namespace ECSGenerator2 {

	class Parser {
	public:

		std::shared_ptr<ParsedECSFile> Parse(const std::filesystem::path ecsFilePath, const std::string& ecsFileText) {

			::Lua::Script script{ ecsFileText };

			::Lua::Context context;
			context.LoadScript(script);
			luabridge::LuaRef ecsFile = luabridge::getGlobal(context.state_, "_G");

			//Get all global tables in the script.
			std::vector<std::string> tableNames;
			for (auto it = luabridge::pairs(ecsFile).begin(); !it.isNil(); ++it) {
				luabridge::LuaRef key = it.key();
				luabridge::LuaRef value = it.value();

				if (value.isTable() && key.isString()) {
					tableNames.push_back(key.tostring());
				}
			}

			std::vector<std::shared_ptr<ParsedSystem>> parsedSystems;
			std::vector<std::shared_ptr<ParsedComponent>> parsedComponents;
			std::vector<std::shared_ptr<ParsedStruct>> parsedStructs;



			for (const std::string& globalName : tableNames) {
				if (globalName == "BehaviourComponent") {
					Common::BreakPointLine();
				}
				if (globalName.ends_with("Struct")) {
					luabridge::LuaRef structRef = ecsFile[globalName];
					StructParser structParser;
					auto parsedStruct = structParser.Parse(structRef,
						globalName.substr(0, globalName.rfind("Struct")));
					parsedStructs.push_back(parsedStruct);
				}
				if (globalName.ends_with("Component")) {

					luabridge::LuaRef componentRef = ecsFile[globalName];
					ComponentParser componentParser;
					auto parsedComponent = componentParser.Parse(componentRef,
						globalName.substr(0, globalName.rfind("Component")));
					parsedComponents.push_back(parsedComponent);
				}
				if (globalName.ends_with("System")) {

					if (globalName == "CreateResourceSystemSystem") {
						Common::BreakPointLine();
					}

					luabridge::LuaRef systemRef = ecsFile[globalName];
					SystemParser systemParser;
					auto parsedSystem = systemParser.Parse(systemRef,
						globalName.substr(0, globalName.rfind("System")));
					parsedSystems.push_back(parsedSystem);
				}
			}

			ParsedECSFile::CreateInfo pEcsFileCI{
				.path_ = ecsFilePath,
				.systems_ = parsedSystems,
				.components_ = parsedComponents,
				.structs_ = parsedStructs
			};

			auto parsedEcsFile = std::make_shared<ParsedECSFile>(pEcsFileCI);

			return parsedEcsFile;
		}

	};

}