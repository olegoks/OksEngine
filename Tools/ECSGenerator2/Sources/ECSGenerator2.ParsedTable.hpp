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

    };

    using ParsedTablePtr = std::shared_ptr<ParsedTable>;

}