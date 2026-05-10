#pragma once

#include <string>
#include <regex>
#include <luabridge3/LuaBridge/LuaBridge.h>

namespace OksEngine::ECS::File::Table {

#define ECS__FILE__TABLE__PARSE_CONSTANT(luaRef, name)\
    [this](luabridge::LuaRef constant,\
    const std::string& constantName){                               \
        const ECS2::Entity::Id constantEntityId = CreateEntity<ECS__FILE__TABLE__CONSTANT__CONSTANT>();   \
        luabridge::LuaRef type = constant["type"];                  \
        luabridge::LuaRef value = constant["value"];                \
        CreateComponent<ECS::File::Table::Tag>(constantEntityId);\
        CreateComponent<ECS::File::Table::Constant::Tag>(constantEntityId);\
        CreateComponent<ECS::File::Table::Name>(constantEntityId, constantName);\
        CreateComponent<Constant::TypeName>(constantEntityId, type.cast<std::string>().value());\
        CreateComponent<Constant::Value>(constantEntityId, value.cast<std::string>().value());\
        return  constantEntityId;       \
    }(luaRef, name);        

#define ECS__FILE__TABLE__GET_NAMESPACE_ENTITY_IDS(entityId)\
    [this](ECS2::Entity::Id tableEntityId){\
		ASSERT(tableEntityId.IsValid());\
        std::vector<ECS2::Entity::Id> namespaces; \
        std::function<void(ECS2::Entity::Id)> processParent = \
        [this, \
        &namespaces, \
        &processParent](ECS2::Entity::Id parentEntityId) {\
            ECS2::ComponentsFilter parentTableCF = GetComponentsFilter(parentEntityId); \
            ASSERT(IsComponentExist<File::Table::Tag>(parentEntityId)); \
            ASSERT(IsComponentExist<File::Table::Namespace::Tag>(parentEntityId)); \
            ASSERT(IsComponentExist<File::Table::Name>(parentEntityId)); \
            ASSERT(IsComponentExist<File::Table::ChildTablesEntityIds>(parentEntityId)); \
            if (IsComponentExist<File::Table::ParentTableEntityId>(parentEntityId)) {\
                   processParent(GetComponent<File::Table::ParentTableEntityId>(parentEntityId)->id_); \
            }\
            namespaces.push_back(parentEntityId);\
        }; \
		if(IsComponentExist<File::Table::ParentTableEntityId>(tableEntityId)){\
			ECS2::Entity::Id parentTableEntityId = GetComponent<File::Table::ParentTableEntityId>(tableEntityId)->id_; \
			processParent(parentTableEntityId); \
		}\
        return namespaces;\
    }(entityId)

#define ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(entityId)\
	[this](ECS2::Entity::Id tableEntityId){\
		std::vector<ECS2::Entity::Id> namespaces = ECS__FILE__TABLE__GET_NAMESPACE_ENTITY_IDS(tableEntityId);\
		std::vector<std::string> namespaceNames;\
		for (ECS2::Entity::Id namespaceEntityId : namespaces) {\
			namespaceNames.push_back(GetComponent<ECS::File::Table::Name>(namespaceEntityId)->name_);\
		}\
		return namespaceNames;\
	}(entityId)

#define ECS__FILE__TABLE__GET_SPLITED_FULL_NAME(entityId)\
	[this](ECS2::Entity::Id tableEntityId){\
		std::vector<std::string> tableNames = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(tableEntityId);\
		tableNames.push_back(GetComponent<ECS::File::Table::Name>(tableEntityId)->name_);\
		return tableNames;\
	}(entityId)

#define ECS__FILE__TABLE__GET_FULL_NAMESPACE_STRING(entityId, delimiterString, isNamespaceUppercase)\
		[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
			ECS2::ComponentsFilter beginTableCF = GetComponentsFilter(tableEntityId);\
			std::vector<std::string> namespaceStrings = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(tableEntityId);\
			std::string resultNamespace;\
			for (Common::Index i = 0; i < namespaceStrings.size(); i++) {\
				std::string& string = namespaceStrings[i];\
				if (isUppercase) {\
					/*Change values in namespaceStrings, we will not use it later.*/\
					std::transform(string.begin(), string.end(), string.begin(),\
						[](unsigned char c) { return std::toupper(c); });\
				}\
				resultNamespace += string;\
				if (i != namespaceStrings.size() - 1) {\
					resultNamespace += delimiter;\
				}\
			}\
			return resultNamespace;\
			}(entityId,delimiterString,isNamespaceUppercase)

#define ECS__FILE__TABLE__GET_FULL_NAME(entityId, delimiterString, isNamespaceUppercase)\
			[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
				ASSERT(IsComponentExist<File::Table::Tag>(tableEntityId));\
				ASSERT(IsComponentExist<File::Table::Name>(tableEntityId));\
				std::string tableName = GetComponent<File::Table::Name>(tableEntityId)->name_;\
				if (isUppercase) {\
					std::transform(tableName.begin(), tableName.end(), tableName.begin(),\
						[](unsigned char c) { return std::toupper(c); });\
				}\
				const std::string tableNamespace = ECS__FILE__TABLE__GET_FULL_NAMESPACE_STRING(tableEntityId, delimiter, isUppercase);\
				return tableNamespace + delimiter + tableName;\
			}(entityId, delimiterString, isNamespaceUppercase)

	static inline bool IsValidName(const std::string& fullName) {
		std::regex pattern(R"(^(::)?([A-Za-z_][A-Za-z0-9_]*)(::[A-Za-z_][A-Za-z0-9_]*)*$)");
		return std::regex_match(fullName, pattern);
	}

	AI_GENERATED
		static inline std::vector<std::string> SplitName(const std::string& name) {
		std::vector<std::string> result;
		size_t start = 0;
		size_t pos;

		// Ищем разделители "::"
		while ((pos = name.find("::", start)) != std::string::npos) {
			// Извлекаем подстроку от start до pos
			std::string token = name.substr(start, pos - start);
			if (!token.empty()) {
				result.push_back(token);
			}
			start = pos + 2; // пропускаем "::"
		}

		// Последний фрагмент (после последнего "::")
		std::string last_token = name.substr(start);
		if (!last_token.empty()) {
			result.push_back(last_token);
		}
		ASSERT(result.size() >= 1);
		return result;
	}

	AI_GENERATED
		static inline std::string GetName(const std::string& fullName) {
		size_t pos = fullName.rfind("::");
		if (pos == std::string::npos) {
			return fullName;
		}
		return fullName.substr(pos + 2);
	}


}
