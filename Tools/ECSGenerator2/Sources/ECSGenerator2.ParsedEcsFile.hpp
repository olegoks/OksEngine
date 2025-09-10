#pragma once

#include <System/ECSGenerator2.ParsedSystem.hpp>
#include <Component/ECSGenerator2.ParsedComponent.hpp>
#include <Struct/ECSGenerator2.ParsedStruct.hpp>
#include <Namespace/ECSGenerator2.ParsedNamespace.hpp>

namespace ECSGenerator2 {

	class ParsedECSFile {
	public:

		struct CreateInfo {
			std::filesystem::path path_;
			std::vector<std::shared_ptr<ParsedTable>> tables_;
		};

		ParsedECSFile(const CreateInfo& ci) :
			ci_{ ci } {  }


		//std::pair<
		//	std::vector<std::shared_ptr<ParsedTable>>,
		//	std::shared_ptr<ParsedTable>> GetTableByName(const std::string& name) {
		//	ForEachTable([](std::vector<std::shared_ptr<ParsedTable>> childTables,
		//		std::shared_ptr<ParsedTable> table) {
		//		
		//			if (table->) {

		//			}
		//		
		//		});
		//}

		//Access tables.
		using ProcessTable = std::function<bool(std::shared_ptr<ParsedTable> table)>;
		void ForEachRootTable(ProcessTable&& processTable) {

			//std::function<void(std::shared_ptr<ParsedTable>)> processTable
			//	= [](std::shared_ptr<ParsedTable> table) {
			//		
			//	};
			for (Common::Index i = 0; i < ci_.tables_.size(); i++) {
				auto parsedTable = ci_.tables_[i];
				const bool isContinue = processTable(parsedTable);
				if (!isContinue) {
					break;
				}
			}
		}

		void ForEachTable(const ProcessTable& processTable) const {
			for (auto table : ci_.tables_) {
				processTable(table);
				table->ForEachChildTable(processTable);
			}

		}

		void ForEachChildlessTable(const ProcessTable& processChildLessTable) {
			for (auto table : ci_.tables_) {
				if (!table->HasChilds()) {
					const bool isContinue = processChildLessTable(table);
					if (!isContinue) {
						break;
					}
				}
				else {
					table->ForEachChildlessTable(processChildLessTable);
				}
			}
		}

		using ProcessTableWithChilds
			= std::function<void(
				std::vector<std::shared_ptr<ParsedTable>>& childTables,
				std::shared_ptr<ParsedTable> table)>;

		void ForEachTable(ProcessTableWithChilds&& processTable) const {

			std::vector<std::shared_ptr<ParsedTable>> childTables;

			std::function<void(
				std::vector<std::shared_ptr<ParsedTable>>&,
				std::shared_ptr<ParsedTable>)> processParsedTable
				= [&](std::vector<std::shared_ptr<ParsedTable>>& thisChildTables,
					std::shared_ptr<ParsedTable> table) {

						processTable(thisChildTables, table);

						if (table->GetType() == ParsedTable::Type::Namespace) {
							auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(table);
							childTables.push_back(table);

							auto processChildTable = [&](std::shared_ptr<ParsedTable> parsedTable) {
								processParsedTable(childTables, parsedTable);
								return true;
								};

							parsedNamespace->ForEachChildTable(processChildTable);
							childTables.pop_back();
						}

				};

			for (auto table : ci_.tables_) {
				processParsedTable(childTables, table);
			}
		}

		//Access systems.
		using ProcessSystem = std::function<bool(std::shared_ptr<ParsedSystem>)>;

		void ForEachSystem(const ProcessSystem& processSystem) const {
			auto processTable = [&](std::shared_ptr<ParsedTable> table) {
				if (table->GetType() == ParsedTable::Type::System) {
					auto parsedSystem = std::dynamic_pointer_cast<ParsedSystem>(table);
					const bool isContinue = processSystem(parsedSystem);
					if (!isContinue) {
						return false;
					}
				}
				return true;
				};

			ForEachTable(processTable);
		}

		//Access components.
		using ProcessComponent = std::function<bool(std::shared_ptr<ParsedComponent>)>;

		void ForEachComponent(ProcessComponent&& processComponent) {
			ForEachTable([&](std::shared_ptr<ParsedTable> table) {
				if (table->GetType() == ParsedTable::Type::Component) {
					auto parsedComponent = std::dynamic_pointer_cast<ParsedComponent>(table);
					const bool isContinue = processComponent(parsedComponent);
					return isContinue;
					//TODO: process

				}
				return true;
				});
		}

		using ProcessComponentWithChilds
			= std::function<void(
				std::vector<std::shared_ptr<ParsedTable>>& childTables,
				std::shared_ptr<ParsedComponent> table)>;

		void ForEachComponent(ProcessComponentWithChilds&& processComponent) const {
			ForEachTable([&](
				std::vector<std::shared_ptr<ParsedTable>>& childTables,
				std::shared_ptr<ParsedTable> table) {
					if (table->GetType() == ParsedTable::Type::Component) {
						auto parsedComponent = std::dynamic_pointer_cast<ParsedComponent>(table);
						processComponent(childTables, parsedComponent);
					}
				});
		}

		using ProcessStruct = std::function<void(std::shared_ptr<ParsedStruct>)>;

		void ForEachStruct(ProcessStruct&& processStruct) {
			auto processTable = [&](std::shared_ptr<ParsedTable> table) {
				if (table->GetType() == ParsedTable::Type::Struct) {
					auto parsedStruct = std::dynamic_pointer_cast<ParsedStruct>(table);
					processStruct(parsedStruct);
					//TODO: process

				}
				return true;
				};

			ForEachTable(processTable);
		}

		[[nodiscard]]
		bool IsContainsSystems() const noexcept {
			bool isContainsSystems = false;
			ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {
				isContainsSystems = true;
				return false;
				});
			return isContainsSystems;
		}

		[[nodiscard]]
		bool IsContainsComponent(const std::string& componentName) {

			const auto parsedFullName = ParseFullName(componentName);
			bool isContainsComponent = false;

			std::vector<ParsedTablePtr> tablesPath;
			Common::Index currentTable = 0;

			std::function<bool(ParsedTablePtr)> processTable = [&](ParsedTablePtr table) {

				if (table->GetName() == parsedFullName[currentTable]) {
					tablesPath.push_back(table);
					++currentTable;
					for (auto childTable : table->childTables_) {
						const bool isContinue = processTable(childTable);
						if (!isContinue) {
							break;
						}
					}
					return false;
				}

				return true;

				};

			ForEachRootTable(std::move(processTable));

			if (!tablesPath.empty() && tablesPath.back()->GetName() != parsedFullName.back()) {
				tablesPath.clear();
			}



			if (tablesPath.empty()) {
				return false;
			}


#pragma region Assert

			OS::AssertMessage(tablesPath.back()->GetName() == parsedFullName.back(), "");
#pragma endregion

			if (tablesPath.back()->GetName() == parsedFullName.back()) {
				return true;
			}
			return false;

			//ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {
			//	if (componentName == parsedComponent->GetName()) {
			//		isContainsComponent = true;
			//	}
			//	});
			//return isContainsComponent;
		}

		[[nodiscard]]
		const std::filesystem::path GetPath() const noexcept {
			return ci_.path_;
		}

		[[nodiscard]]
		const std::string GetName() const noexcept {
			return GetPath().filename().stem().string();
		}

		


	private:

		CreateInfo ci_;
	};

	using ParsedECSFilePtr = std::shared_ptr<ParsedECSFile>;
	using ParsedTablesPath = std::vector<ParsedTablePtr>;

	//Get ParsedTables path if ParsedECSFile contains needed tables otherwise function returs empty ParsedTablesPath.
	ParsedTablesPath GetTablePathByFullName(ParsedECSFilePtr parsedEcsFile, const std::vector<std::string>& path) {

#pragma region Assert
		OS::AssertMessage(!path.empty(), "");
#pragma endregion

		std::vector<std::string> parentRecursivePath;
		for (Common::Int64 i = path.size() - 1; i >= 0; i--) {
			parentRecursivePath.push_back(path[i]);
		}
		parentRecursivePath.erase(parentRecursivePath.begin());

		if ("OksEngine.ResourceSystem" == parsedEcsFile->GetName() && path[0] == "ResourceSystem") {
			Common::BreakPointLine();
		}


		ParsedTablesPath resultTablePath;

		bool pathIsFound = false;


		auto processChildLessTable = [&](ParsedTablePtr childLessTable) {

			if (path.back() == childLessTable->GetName()) {

				if (parentRecursivePath.empty()) {
					resultTablePath.push_back(childLessTable);
					pathIsFound = true;
					return false;
				}

				ParsedTablesPath tablesPath;
				Common::Index index = 0;
				//Found table with needed name.
				childLessTable->ForEachParentTable([&](ParsedTablePtr table) {
					if (table->GetName() == parentRecursivePath[index]) {
						tablesPath.insert(tablesPath.begin(), table);
						++index;
						return true;
					} else {
						return false;
					}

					});

				pathIsFound = tablesPath.size() == parentRecursivePath.size();
				if (pathIsFound) {
					tablesPath.push_back(childLessTable);
					for (auto& tablePath : tablesPath) {
						resultTablePath.push_back(tablePath);
					}
					return false;
				}
			}

			return true;
			};

		parsedEcsFile->ForEachChildlessTable(processChildLessTable);

		if (!pathIsFound) {
			resultTablePath.clear();
		}
		return resultTablePath;
	}

};