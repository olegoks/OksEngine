#pragma once 

namespace ECSGenerator2 {

    class ParsedTable {
    public:

        enum class Type {
            Namespace,
            System,
            Component,
            Struct,
            Undefined
        };

        virtual Type GetType() const noexcept = 0;
        virtual const std::string& GetName() const noexcept = 0;

        virtual ~ParsedTable() = default;
        
        std::vector<std::shared_ptr<ParsedTable>> childTables_;
        std::shared_ptr<ParsedTable> parentTable_ = nullptr;

        using ProcessParentTable = std::function<bool(std::shared_ptr<ParsedTable>)>;

        //From child to root.
        void ForEachParentTable(ProcessParentTable&& processParentTable) {
            ProcessParentTable processParent = [&](std::shared_ptr<ParsedTable> parsedTable) {
                
                const bool isContinue = processParentTable(parsedTable);
                
                if (!isContinue) {
                    return false;
                }

                if (parsedTable->parentTable_ != nullptr) {
                    return processParent(parsedTable->parentTable_);
                }

                return true;

                };

            if (parentTable_ != nullptr) {
                processParent(parentTable_);
            }
        }

        using ProcessChildTable = std::function<bool(std::shared_ptr<ParsedTable>)>;

        void ForEachChildTable(ProcessChildTable&& processChildTable) {
            for (auto childTable : childTables_) {
                const bool isContinue = processChildTable(childTable);
                if (!isContinue) {
                    break;
                }
            }
        }

        void ForEachChildTableRecursive(ProcessChildTable&& processChildTable) {

            ForEachChildTable([&](std::shared_ptr<ParsedTable> childTable) {
                
                const bool isContinue = processChildTable(childTable);
                childTable->ForEachChildTableRecursive(std::move(processChildTable));
                return isContinue;
                
                });

        }


    };

    using ParsedTablePtr = std::shared_ptr<ParsedTable>;

}