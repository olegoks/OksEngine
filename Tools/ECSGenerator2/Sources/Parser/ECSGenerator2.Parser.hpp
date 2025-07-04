#pragma once 

#include <ECSGenerator2.ParsedECSFile.hpp>

#include <System/ECSGenerator2.SystemParser.hpp>

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

			for (const std::string& globalName : tableNames) {
				if (globalName.ends_with("Component")) {
					luabridge::LuaRef componentRef = ecsFile[globalName];

					auto parsedComponent = ParseComponent(componentRef, globalName);
					parsedComponents.push_back(parsedComponent);
				}
				if (globalName.ends_with("System")) {
					luabridge::LuaRef systemRef = ecsFile[globalName];
					SystemParser systemParser;
					auto parsedSystem = systemParser.Parse(systemRef, globalName);
					parsedSystems.push_back(parsedSystem);
				}
			}

			ParsedECSFile::CreateInfo pEcsFileCI{
				.path_ = ecsFilePath,
				.systems_ = parsedSystems,
				.components_ = parsedComponents
			};

			auto parsedEcsFile = std::make_shared<ParsedECSFile>(pEcsFileCI);

			return parsedEcsFile;
		}

	};

}