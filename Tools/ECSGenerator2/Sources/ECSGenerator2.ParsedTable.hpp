#pragma once 

#include <OS.Assert.hpp>

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


        //Now all parent tables are only namespaces. Get namespace as "XXX", "YYY".
        inline std::vector<std::string> GetNamespace() {

            std::vector<std::string> namespaceStrings;
            namespaceStrings.reserve(5);
            ForEachParentTable([&](std::shared_ptr<ParsedTable> parentParsedTable) {

#pragma region Assert
                ASSERT_FMSG(parentParsedTable->GetType() == Type::Namespace, "");
#pragma endregion

                namespaceStrings.insert(namespaceStrings.begin(), parentParsedTable->GetName());

                return true;

                });

            return namespaceStrings;
        }

        inline std::string GetConcatedNamespace() {
            const auto separatedNamespace = GetNamespace();
            std::string concatedNamespace;

            for (Common::Index i = 0; i < separatedNamespace.size(); i++) {
                concatedNamespace += separatedNamespace[i];
                if (i != separatedNamespace.size() - 1) {
                    concatedNamespace += "::";
                }
            }
            return concatedNamespace;
        }


        //Get full name as XXX::YYY::Zzzz
        inline std::string GetFullName() {

            std::vector<std::string> namespaceStrings = GetNamespace();
            namespaceStrings.push_back(GetName());
            decltype(namespaceStrings)& fullNameStrings = namespaceStrings;

            std::string fullNamespace;
            for (Common::Index i = 0; i < fullNameStrings.size(); i++) {
                fullNamespace += fullNameStrings[i];
                if (i != fullNameStrings.size() - 1) {
                    fullNamespace += "::";
                }
            }

            return fullNamespace;

        }

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

        bool HasParent() const {
            return parentTable_ != nullptr;
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

        using ProcessTablePath = std::function<bool(std::vector<std::shared_ptr<ParsedTable>>&)>;

        void ForEachTablePath(const ProcessTablePath& processTablePath) {



            std::function<void(
                std::shared_ptr<ParsedTable>,
                std::vector<std::shared_ptr<ParsedTable>>&, 
                std::vector<std::vector<std::shared_ptr<ParsedTable>>>&)> dfs = [&](
                    std::shared_ptr<ParsedTable> node,
                std::vector<std::shared_ptr<ParsedTable>>& currentPath,
                std::vector<std::vector<std::shared_ptr<ParsedTable>>>& allPaths) {

                // Добавляем текущую ноду в путь
                currentPath.push_back(node);

                // Если это лист (нет дочерних) - сохраняем путь
                if (node->childTables_.empty()) {
                    allPaths.push_back(currentPath);
                }
                else {
                    // Рекурсивно обходим всех детей
                    for (std::shared_ptr<ParsedTable> child : node->childTables_) {
                        dfs(child, currentPath, allPaths);
                    }
                }

                // Backtracking - удаляем текущую ноду из пути
                currentPath.pop_back();
                
                };

            std::vector<std::vector<std::shared_ptr<ParsedTable>>> allPaths;
            std::vector<std::shared_ptr<ParsedTable>> currentPath;

            auto root = std::shared_ptr<ParsedTable>{this, [](ParsedTable* table) { /*Do nothing.*/ }};

            dfs(root, currentPath, allPaths);

            for (auto& path : allPaths) {
                processTablePath(path);
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