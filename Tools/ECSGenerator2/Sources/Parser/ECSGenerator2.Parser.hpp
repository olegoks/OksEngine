#pragma once 

#include <ECSGenerator2.ParsedECSFile.hpp>

#include <Namespace/ECSGenerator2.ParsedNamespace.hpp>
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
			{
				for (auto it = luabridge::pairs(ecsFile).begin(); !it.isNil(); ++it) {
					luabridge::LuaRef key = it.key();
					luabridge::LuaRef value = it.value();

					if (value.isTable() && key.isString()) {
						tableNames.push_back(key.tostring());
					}
				}
			}

			auto sortTables = [](std::vector<std::shared_ptr<ParsedTable>>& tables) {

				//Sort tables:
				//1. Structs
				//2. Components
				//3. Systems
				std::sort(tables.begin(), tables.end(), [](
					std::shared_ptr<ParsedTable> first,
					std::shared_ptr<ParsedTable> second) {
						const auto type1 = first->GetType();
						const auto type2 = second->GetType();

						// Определяем приоритеты типов
						const auto getPriority = [](ParsedTable::Type type) {
							switch (type) {
							case ParsedTable::Type::Struct:    return 0; // Высший приоритет
							case ParsedTable::Type::Component: return 1;
							case ParsedTable::Type::System:    return 2; // Низший приоритет
							default:                           return 3;
							}
							};

						const int priority1 = getPriority(type1);
						const int priority2 = getPriority(type2);

						// Сначала сортируем по приоритету типа
						if (priority1 != priority2) {
							return priority1 < priority2;
						}
						return false;
					});
				};

			std::vector<std::shared_ptr<ParsedTable>> parsedTables;

			std::function<std::shared_ptr<ParsedTable>(const std::string&, luabridge::LuaRef)> processTable
				= [&](
					const std::string& name,
					luabridge::LuaRef table) -> std::shared_ptr<ParsedTable> {

						if (name.ends_with("Namespace")) {
							const std::string namespaceName = name.substr(0, name.rfind("Namespace"));

							std::vector<std::shared_ptr<ParsedTable>> parsedNamespaceTables;

							for (auto it = luabridge::Iterator(table); !it.isNil(); ++it) {
								luabridge::LuaRef key = it.key();
								luabridge::LuaRef value = it.value();

								if (value.isTable()) {
									if (key.isString()) {
										const std::string tableName = key.tostring();
										auto parsedTable = processTable(tableName, value);

#pragma region Assert
										OS::AssertMessage(parsedTable != nullptr , "");
#pragma endregion

										parsedNamespaceTables.push_back(parsedTable);

									}
								}
							}
							sortTables(parsedNamespaceTables);
							ParsedNamespace::CreateInfo namespaceCI{
								.name_ = namespaceName,
								.childTables_ = parsedNamespaceTables
							};
							auto parsedNamespace = std::make_shared<ParsedNamespace>(namespaceCI);

							return parsedNamespace;
						}

						if (name.ends_with("Struct")) {

							StructParser structParser;
							auto parsedStruct = structParser.Parse(table,
								name.substr(0, name.rfind("Struct")));
							return parsedStruct;
						}
						if (name.ends_with("Component")) {

							ComponentParser componentParser;
							auto parsedComponent = componentParser.Parse(table,
								name.substr(0, name.rfind("Component")));
							return parsedComponent;
						}
						if (name.ends_with("System")) {

							if (name == "CreateResourceSystemSystem") {
								Common::BreakPointLine();
							}

							SystemParser systemParser;
							auto parsedSystem = systemParser.Parse(table,
								name.substr(0, name.rfind("System")));
							return parsedSystem;

						}


						return nullptr;
				};

			for (const std::string& globalName : tableNames) {
				if (globalName == "BehaviourComponent") {
					Common::BreakPointLine();
				}
				luabridge::LuaRef table = ecsFile[globalName];
				auto parsedTable = processTable(globalName, table);
				if (parsedTable != nullptr) {
					parsedTables.push_back(parsedTable);
				}
			}
			sortTables(parsedTables);

			ParsedECSFile::CreateInfo pEcsFileCI{
				.path_ = ecsFilePath,
				.tables_ = parsedTables
			};

			auto parsedEcsFile = std::make_shared<ParsedECSFile>(pEcsFileCI);

			return parsedEcsFile;
		}

	};

}