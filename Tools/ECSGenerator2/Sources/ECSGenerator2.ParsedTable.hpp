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
        virtual std::shared_ptr<ParsedTable> Clone() const noexcept = 0;
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

        //From parent to child without start table.
        std::vector<std::shared_ptr<ParsedTable>> GetParentTables() {

            std::vector<std::shared_ptr<ParsedTable>> parentTables;
            parentTables.reserve(5); // Medium Max number of parents.

            ForEachParentTable([&](std::shared_ptr<ParsedTable> parentTable) {
                parentTables.insert(parentTables.begin(), parentTable);
                return true;
                });

            return parentTables;
        }

        using ProcessChildTable = std::function<bool(std::shared_ptr<ParsedTable>)>;

        void ForEachChildTable(const ProcessChildTable& processChildTable) {
            for (auto childTable : childTables_) {
                const bool isContinue = processChildTable(childTable);
                if (!isContinue) {
                    break;
                }
            }
        }

        
        void ForEachChildTableRecursive(const ProcessChildTable& processChildTable) {

            ForEachChildTable([&](std::shared_ptr<ParsedTable> childTable) {
                
                const bool isContinue = processChildTable(childTable);
                childTable->ForEachChildTableRecursive(processChildTable);
                return isContinue;
                
                });

        }

        void ForEachChildlessTable(const ProcessChildTable& processChildTables) {
            ForEachChildTableRecursive([&](std::shared_ptr<ParsedTable> childTable) {
                
                if (!childTable->HasChilds()) {
                    const bool isContinue = processChildTables(childTable);
                    return isContinue;
                }

                return true;
                
                });
        }

        [[nodiscard]]
        bool HasChilds() const noexcept {
            return !childTables_.empty();
        }

    };

    using ParsedTablePtr = std::shared_ptr<ParsedTable>;


    inline std::vector<std::string> ParseFullName(const std::string& fullName) {
        //"Behaviour::ScriptName::X" -> "Behaviour", "ScriptName", "X"
        auto parseIncludeName = [](const std::string& name) {
            std::vector<std::string> result;
            size_t start = 0;
            size_t end = name.find("::");

            while (end != std::string::npos) {
                result.push_back(name.substr(start, end - start));
                start = end + 2; // Пропускаем "::"
                end = name.find("::", start);
            }

            // Добавляем последнюю часть
            result.push_back(name.substr(start));

            return result;
            };

        return parseIncludeName(fullName);
    }

}