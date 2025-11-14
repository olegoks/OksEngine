#pragma once 

#include <ranges>
#include <regex>
#include <algorithm>

#include <ECSGenerator2.ParsedECSFile.hpp>

#include <Namespace/ECSGenerator2.ParsedNamespace.hpp>
#include <System/ECSGenerator2.SystemParser.hpp>
#include <Component/ECSGenerator2.ComponentParser.hpp>
#include <Constant/ECSGenerator2.ConstantParser.hpp>
#include <Archetype/ECSGenerator2.ArchetypeParser.hpp>
#include <Struct/ECSGenerator2.StructParser.hpp>


namespace ECSGenerator2 {

	class Parser {
	public:

		std::shared_ptr<ParsedECSFile> Parse(const std::filesystem::path ecsFilePath, const std::string& ecsFileText) {

			::Lua::Script script{ ecsFileText };

			::Lua::Context context;
			context.LoadScript(script);
			luabridge::LuaRef ecsFile = luabridge::getGlobal(context.state_, "_G");

			std::vector<std::string> abstractionsOrder;
			// Get ECS abstraction order from .lua script source.
			//TODO: Take into account namespaces and their heir
			{

				auto processLine = [&](const std::string& line) {
					std::smatch match;
					std::regex systemRegex{ R"(\w+System\s*=\s*\{)" };
					std::regex componentRegex{ R"(\w+Component\s*=\s*\{)" };
					std::regex structRegex{ R"(\w+Struct\s*=\s*\{)" };
					std::regex constantRegex{ R"(\w+Constant\s*=\s*\{)" };
					std::regex archetypeRegex{ R"(\w+Archetype\s*=\s*\{)" };
					// Ищем System
					if (std::regex_search(line, match, systemRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("=")); // Убираем "= {"
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
						name.erase(name.size() - sizeof("System") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// Ищем Component
					else if (std::regex_search(line, match, componentRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());
						name.erase(name.size() - sizeof("Component") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// Ищем Struct
					else if (std::regex_search(line, match, structRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

						name.erase(name.size() - sizeof("Struct") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// Ищем Constant
					else if (std::regex_search(line, match, constantRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

						name.erase(name.size() - sizeof("Constant") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					// Ищем Archetype
					else if (std::regex_search(line, match, archetypeRegex)) {
						std::string name = match[0].str();
						name = name.substr(0, name.find("="));
						name.erase(std::remove_if(name.begin(), name.end(), ::isspace), name.end());

						name.erase(name.size() - sizeof("Archetype") + 1, name.size());
						abstractionsOrder.push_back(name);
					}
					};
				std::string line;
				std::stringstream stream{ ecsFileText };
				while (std::getline(stream, line)) {
					processLine(line);
				}
			}

			//BRK_IF(ecsFilePath == "D:/OksEngine/Tools/ECSGenerator2/../../Sources/Engine/Sources/Render/Texture/OksEngine.Texture.ecs");

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

								if (value.isTable() ||
									value.isString()/*For aliases*/) {
									if (key.isString()) {
										const std::string tableName = key.tostring();
										//TODO: move setting of child table to processTable function.
										auto parsedTable = processTable(tableName, value);
										ASSERT_FMSG(parsedTable != nullptr, 
											"Can't parse table {} in {}. Maybe it doesn't have keyword as Component/System/Archetype/Struct...",
											tableName, ecsFilePath.string());
										parsedNamespaceTables.push_back(parsedTable);
									}
								}
							}

							ParsedNamespace::CreateInfo namespaceCI{
								.name_ = namespaceName
							};
							auto parsedNamespace = std::make_shared<ParsedNamespace>(namespaceCI);

							parsedNamespace->childTables_ = parsedNamespaceTables;

							for (auto childTable : parsedNamespaceTables) {
								childTable->parentTable_ = parsedNamespace;
							}

							return parsedNamespace;
						} else if (name.ends_with("Struct")) {

							StructParser structParser;
							auto parsedStruct = structParser.Parse(table,
								name.substr(0, name.rfind("Struct")));
							return parsedStruct;
						} else if (name.ends_with("Component")) {

							ComponentParser componentParser;
							auto parsedComponent = componentParser.Parse(table,
								name.substr(0, name.rfind("Component")));
							return parsedComponent;
						} else if (name.ends_with("Constant")) {

							ConstantParser constantParser;
							auto parsedConstant = constantParser.Parse(table,
								name.substr(0, name.rfind("Constant")));
							return parsedConstant;
						} else if (name.ends_with("Archetype")) {

							ArchetypeParser archetypeParser;
							auto parsedArchetype = archetypeParser.Parse(table,
								name.substr(0, name.rfind("Archetype")));
							return parsedArchetype;
						} else if (name.ends_with("System")) {

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

			for (auto it = luabridge::pairs(ecsFile).begin(); !it.isNil(); ++it) {

				luabridge::LuaRef key = it.key();
				luabridge::LuaRef value = it.value();

				//Key must be string.
				if (!key.isString()) {
					continue;
				}
				
				//Value must be a table(description of component, system...) or a string(component alias)
				if (!(value.isTable() || value.isString())) {
					continue;
				}


				luabridge::LuaRef table = key;
				auto parsedTable = processTable(key.tostring(), ecsFile[key.tostring()]);

				//Separate namespace tables:
				//namespace {
				//	struct{}
				//	struct {}
				//}
				//to:
				//namespace {
				//struct {}
				// }
				// namespace {
				// struct {}
				// }
				if (parsedTable == nullptr) {
					continue;
				}
				std::vector<ParsedTablePtr> separatedTables;

				parsedTable->ForEachChildlessTable([&](ParsedTablePtr childlessTable) {

#pragma region Assert
					ASSERT_FMSG(!childlessTable->HasChilds(), "");
#pragma endregion							
					auto parentTables = childlessTable->GetParentTables();

					std::vector<std::shared_ptr<ParsedTable>> clonedTables;
					std::ranges::for_each(parentTables, [&](std::shared_ptr<ParsedTable> parentTable) {
						auto clonedParentTable = parentTable->Clone();
						clonedParentTable->childTables_.clear();
						clonedParentTable->parentTable_ = nullptr;
						clonedTables.push_back(clonedParentTable);

						});
					auto clonedChildlessTable = childlessTable->Clone();
					clonedChildlessTable->childTables_.clear();
					clonedChildlessTable->parentTable_ = nullptr;

					clonedTables.push_back(clonedChildlessTable);

					{
						const Common::Index lastTableIndex = clonedTables.size() - 1;
						for (Common::Index i = 0; i < clonedTables.size(); i++) {
							auto clonedTable = clonedTables[i];
							const Common::Index nextTableIndex = i + 1;
							if (nextTableIndex <= lastTableIndex) {
								auto nextClonedTable = clonedTables[nextTableIndex];
								nextClonedTable->parentTable_ = clonedTable;
								clonedTable->childTables_.push_back(nextClonedTable);
							}
						}
					}

					separatedTables.push_back(clonedTables[0]);

					return true;
					});

				//If there are no separated tables it means that parsed table doesnt have childs.
				if (separatedTables.empty()) {
					separatedTables.push_back(parsedTable);
				}

				for (auto separatedTable : separatedTables) {
					parsedTables.push_back(separatedTable);
				}
			}


			auto sortTables = [](std::vector<std::string>& abstractionsOrder, std::vector<std::shared_ptr<ParsedTable>>& tables) {

				//Sort tables:
				//1. Structs
				//2. Components
				//3. Systems
				std::sort(tables.begin(), tables.end(), [&](
					ParsedTablePtr first,
					ParsedTablePtr second) {

						auto getChildLessTableType = [](ParsedTablePtr table) {
							ParsedTable::Type type1 = ParsedTable::Type::Undefined;
							if (!table->HasChilds()) {
								type1 = table->GetType();
							}
							else {
								auto processChildLessTable = [&](ParsedTablePtr childLessTable) {
									type1 = childLessTable->GetType();
									return false;
									};
								table->ForEachChildlessTable(processChildLessTable);
							}
							return type1;
							};

						auto type1 = getChildLessTableType(first);
						auto type2 = getChildLessTableType(second);

						// Определяем приоритеты типов
						const auto getPriority = [](ParsedTable::Type type) {

							switch (type) {
							case ParsedTable::Type::Constant:		return 0; // Высший приоритет
							case ParsedTable::Type::Struct:			return 1; // Высший приоритет
							case ParsedTable::Type::Component:		return 2;
							case ParsedTable::Type::Archetype:		return 3; // Высший приоритет
							case ParsedTable::Type::System:			return 4; // Низший приоритет
							default:
								ASSERT_FAIL_MSG("Unknown table type!");
							}
							};

						const int abstractionPriority1 = getPriority(type1);
						const int abstractionPriority2 = getPriority(type2);

						ASSERT(first->GetChildlessTablesNumber() == 1);
						ASSERT(second->GetChildlessTablesNumber() == 1);

						std::string firstName;
						if (first->HasChilds()) {
							firstName = first->GetChildLessTables()[0]->GetName();
						}
						else {
							firstName = first->GetName();
						}

						std::string secondName;
						if (second->HasChilds()) {
							secondName = second->GetChildLessTables()[0]->GetName();
						}
						else {
							secondName = second->GetName();
						}

						auto firstIt = std::ranges::find(abstractionsOrder, firstName);
						Common::Index firstIndex = std::distance(abstractionsOrder.begin(), firstIt);

						auto secondIt = std::ranges::find(abstractionsOrder, secondName);
						Common::Index secondIndex = std::distance(abstractionsOrder.begin(), secondIt);

						if (abstractionPriority1 == abstractionPriority2) {
							return firstIndex < secondIndex;

						}
						else {
							return abstractionPriority1 < abstractionPriority2;
						}

					});
				};

			sortTables(abstractionsOrder, parsedTables);



			ParsedECSFile::CreateInfo pEcsFileCI{
				.path_ = ecsFilePath,
				.tables_ = parsedTables
			};

			auto parsedEcsFile = std::make_shared<ParsedECSFile>(pEcsFileCI);

			return parsedEcsFile;
		}

	};

}