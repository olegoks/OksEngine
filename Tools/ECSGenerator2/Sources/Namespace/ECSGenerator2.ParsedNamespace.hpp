#pragma once 

#include <string>
#include <vector>
#include <memory>

#include <ECSGenerator2.ParsedTable.hpp>

namespace ECSGenerator2 {

	class ParsedNamespace : public ParsedTable {
	public:

		struct CreateInfo {
			std::string name_;
			//std::vector<std::shared_ptr<ParsedTable>> childTables_;
		};

		ParsedNamespace(const CreateInfo& ci) : ci_{ ci } {

		}

		virtual ParsedTable::Type GetType() const noexcept override {
			return ParsedTable::Type::Namespace;
		}

		virtual std::shared_ptr<ParsedTable> Clone() const noexcept override {
			return std::make_shared<ParsedNamespace>(*this);
		}

		virtual const std::string& GetName() const noexcept override {
			return ci_.name_;
		}

		//using ProcessTable = std::function<void(std::shared_ptr<ParsedTable>)>;
		//void ForEachChildTable(ProcessTable&& processTable) {
		//	for (Common::Index i = 0; i < ci_.childTables_.size(); i++) {
		//		auto childTable = ci_.childTables_[i];
		//		processTable(childTable);
		//	}
		//}

		CreateInfo ci_;
	};

	//ParsedTable(Behaviour) -> ParsedTable(ScriptName)
	//Convert to:
	//Behaviour::ScriptName
	inline static std::string GetFullTableNameWithNamespace(ParsedTablePtr table) {
		std::string fullName;
		fullName.reserve(512);
		fullName = table->GetName();
		ParsedTablePtr currentTable = table;
		while (currentTable->parentTable_ != nullptr) {
			fullName.insert(0, currentTable->parentTable_->GetName() + "::");

			currentTable = currentTable->parentTable_;
		}
		return fullName;
	}


}