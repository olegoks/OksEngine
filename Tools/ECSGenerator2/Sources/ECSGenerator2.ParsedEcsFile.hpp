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

		void ForEachTable(ProcessTable&& processTable) const {

			std::function<void(std::shared_ptr<ParsedTable>)> processParsedTable
				= [&](std::shared_ptr<ParsedTable> table) {
				processTable(table);
				if (table->GetType() == ParsedTable::Type::Namespace) {
					auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(table);
					parsedNamespace->ForEachChildTable([&](std::shared_ptr<ParsedTable> parsedTable) {
						processParsedTable(parsedTable);
						});
				}
				};
			for (auto table : ci_.tables_) {
				processParsedTable(table);
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
							parsedNamespace->ForEachChildTable([&](std::shared_ptr<ParsedTable> parsedTable) {
								processParsedTable(childTables, parsedTable);
								});
							childTables.pop_back();
						}

				};

			for (auto table : ci_.tables_) {
				processParsedTable(childTables, table);
			}
		}

		//Access systems.
		using ProcessSystem = std::function<bool(std::shared_ptr<ParsedSystem>)>;

		void ForEachSystem(ProcessSystem&& processSystem) const {
			ForEachTable([&](std::shared_ptr<ParsedTable> table) {
				if (table->GetType() == ParsedTable::Type::System) {
					auto parsedSystem = std::dynamic_pointer_cast<ParsedSystem>(table);
					const bool isContinue = processSystem(parsedSystem);
					if (!isContinue) {
						return false;
					}
				}
				return true;
				});
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
			ForEachTable([&](std::shared_ptr<ParsedTable> table) {
				if (table->GetType() == ParsedTable::Type::Struct) {
					auto parsedStruct = std::dynamic_pointer_cast<ParsedStruct>(table);
					processStruct(parsedStruct);
					//TODO: process

				}
				return true;
				});
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

		ParsedTablesPath tablesPath;
		Common::Index currentTable = 0;

		std::function<bool(ParsedTablePtr)> processTable = [&](ParsedTablePtr table) {

			if (table->GetName() == path[currentTable]) {
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

		parsedEcsFile->ForEachRootTable(std::move(processTable));
		
		if (!tablesPath.empty() && tablesPath.back()->GetName() != path.back()) {
			tablesPath.clear();
		}

		return tablesPath;
	}

};