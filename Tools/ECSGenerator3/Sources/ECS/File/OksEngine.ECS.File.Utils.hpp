#pragma once

#include <string>
#include <regex>
#include <luabridge3/LuaBridge/LuaBridge.h>

namespace OksEngine::ECS::File::Table {



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

#define ECS__FILE__TABLE__GET_NAME(entityId, isNamespaceUppercase)\
			[this](ECS2::Entity::Id tableEntityId, bool isUppercase) {\
				ASSERT(IsComponentExist<File::Table::Tag>(tableEntityId));\
				ASSERT(IsComponentExist<File::Table::Name>(tableEntityId));\
				std::string tableName = GetComponent<File::Table::Name>(tableEntityId)->name_;\
				if (isUppercase) {\
					std::transform(tableName.begin(), tableName.end(), tableName.begin(),\
						[](unsigned char c) { return std::toupper(c); });\
				}\
				return tableName;\
			}(entityId, isNamespaceUppercase)

#define ECS__FILE__TABLE__GET_FULL_NAME(entityId, delimiterString, isNamespaceUppercase)\
			[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
				std::string tableName = ECS__FILE__TABLE__GET_NAME(tableEntityId, isNamespaceUppercase);\
				const std::string tableNamespace = ECS__FILE__TABLE__GET_FULL_NAMESPACE_STRING(tableEntityId, delimiter, isUppercase);\
				return tableNamespace + delimiter + tableName;\
			}(entityId, delimiterString, isNamespaceUppercase)

#define ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(tableTag, scope, name)\
	[this]<typename TableTag>(ECS2::Entity::Id usageTableEntityId, const std::string& findingFullTableName) {\
		BEGIN_PROFILE("ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME");\
		ASSERT(OksEngine::ECS::File::Table::IsValidName(findingFullTableName));\
		const std::string findingComponentName = ECS::File::Table::GetName(findingFullTableName);\
		std::vector<std::string> systemNamespaceNames = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(usageTableEntityId);\
		systemNamespaceNames.insert(systemNamespaceNames.begin(), "Root");\
		std::vector<std::string> findingTableSplitedFullName = ECS::File::Table::SplitName(findingFullTableName);\
		if (findingFullTableName.starts_with("::")) {\
			findingTableSplitedFullName.insert(findingTableSplitedFullName.begin(), "Root");\
		}\
		const std::string findingTableName = findingTableSplitedFullName.back();\
		std::vector<std::string> findingTableSplitedNamespaces = findingTableSplitedFullName;\
		findingTableSplitedNamespaces.pop_back();\
		std::vector<std::pair<Common::Size, ECS2::Entity::Id>> applicants;\
		world_->ForEachEntity<ECS::File::Table::Tag>([&](ECS2::Entity::Id tableEntityId) {\
			const ECS2::ComponentsFilter tableCF = GetComponentsFilter(tableEntityId);\
			if (!tableCF.IsSet<TableTag>()) {\
				return;\
			}\
			const std::string tableName = GetComponent<ECS::File::Table::Name>(tableEntityId)->name_;\
			if (tableName != findingTableName) {\
				return;\
			}\
			std::vector<std::string> tableSplitedNamespaces = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(tableEntityId);\
			tableSplitedNamespaces.insert(tableSplitedNamespaces.begin(), "Root");\
			bool isFindingAndCurrentTablesHasCommonNamespace = [](const std::vector<std::string>& first, const std::vector<std::string>& second) {\
				if (second.empty()) {\
					return true;\
				}\
				for (auto rIt = first.rbegin(); rIt != first.rend(); ++rIt) {\
					if (*rIt == *second.begin()) {\
						const Common::UInt64 offset = std::distance(rIt, first.rend()) - 1;\
						if (first.size() - offset != second.size()) {\
							return false;\
						}\
						for (Common::Index j = offset; j < first.size(); j++) {\
							if (first[j] != second[j - offset]) {\
								return false;\
							}\
						}\
						return true;\
					}\
				}\
				return false;\
				}(tableSplitedNamespaces, findingTableSplitedNamespaces);\
			if (!isFindingAndCurrentTablesHasCommonNamespace) {\
				return;\
			}\
			for (auto rIt = systemNamespaceNames.rbegin(); rIt != systemNamespaceNames.rend(); ++rIt) {\
				if (*rIt == tableSplitedNamespaces.front()) {\
					applicants.push_back({ std::abs(std::distance(rIt, systemNamespaceNames.rbegin())), tableEntityId });\
				}\
			}\
			});\
		if (applicants.empty()) {\
			ASSERT_FAIL_FMSG("Can't find {} in context {}.", findingFullTableName, ECS__FILE__TABLE__GET_FULL_NAME(usageTableEntityId, "::", false));\
		}\
		std::sort(applicants.begin(), applicants.end(),\
			[](std::pair<Common::Size, ECS2::Entity::Id>& first, std::pair<Common::Size, ECS2::Entity::Id>& second) {\
				return	first.first < second.first;\
			});\
		END_PROFILE();\
		return applicants.front().second;\
	}.operator()<tableTag>(scope, name);\

#define ECS__FILE__TABLE__GET_TABLE_FILE_ENTITY(table)\
	[this](ECS2::Entity::Id tableEntityId){\
		return GetComponent<ECS::File::EntityId>(tableEntityId)->id_;\
	}(table)

#define ECS__FILE__GET_FILE_INCLUDE_PATH(file, projectFilePath, extension)\
	[this](ECS2::Entity::Id fileEntityId, std::filesystem::path projectFileFullPath, std::string extensionStr){\
		auto projectFilePathIt = projectFileFullPath.begin();\
		const std::filesystem::path componentFilePath = GetComponent<ECS::File::Path>(fileEntityId)->path_;\
		auto hppFilePathIt = componentFilePath.begin();\
		while (*projectFilePathIt == *hppFilePathIt) {\
			++projectFilePathIt;\
			++hppFilePathIt;\
		}\
		std::filesystem::path hppFileRelativePath;\
		ASSERT(*hppFilePathIt == "Sources");\
		++hppFilePathIt;\
		while (hppFilePathIt != componentFilePath.end()) {\
			hppFileRelativePath /= *hppFilePathIt;\
			++hppFilePathIt;\
		}\
		hppFileRelativePath = hppFileRelativePath.parent_path() / ("auto_" + hppFileRelativePath.stem().string() + extensionStr);\
		return hppFileRelativePath;\
	}(file, projectFilePath, extension)

#define ECS__FILE__TABLE__SYSTEM__GET_WRITE_COMPONENT_ENTITY_IDS(system)																											\
	[this](ECS2::Entity::Id systemEntityId){																																		\
		ASSERT(IsComponentExist<ECS::File::Table::System::Tag>(systemEntityId));																									\
		std::vector<ECS2::Entity::Id> writeComponents;																																\
		if (IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {																					\
			ECS2::Entity::Id updateMethodEntityId																																	\
			= GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;																					\
			ECS2::ComponentsFilter updateMethodCF = GetComponentsFilter(updateMethodEntityId);																						\
			if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {														\
				std::vector<ECS2::Entity::Id> processesEntities																														\
					= GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;											\
				for (ECS2::Entity::Id processesEntity : processesEntities) {																										\
					const auto& processComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processesEntity)->componentInfos_;	\
					for (auto& processComponentInfo : processComponentInfos) {																										\
						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(																			\
							ECS::File::Table::Component::Tag,																														\
							systemEntityId,																																			\
							processComponentInfo.name_);																															\
						if (!processComponentInfo.readonly_) {																														\
							writeComponents.push_back(componentEntityId);																											\
						}																																							\
					}																																								\
				}																																									\
			}																																										\
			if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)) {														\
				std::vector<ECS2::Entity::Id> accessesEntities																														\
					= GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)->entityIds_;											\
				for (ECS2::Entity::Id accessEntity : accessesEntities) {																											\
					const auto& accessComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)->components_;			\
					for (auto& accessComponentInfo : accessComponentInfos) {																										\
						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(																			\
							ECS::File::Table::Component::Tag,																														\
							systemEntityId,																																			\
							accessComponentInfo.name_);																																\
						if (!accessComponentInfo.readonly_) {																														\
							writeComponents.push_back(componentEntityId);																											\
						}																																							\
					}																																								\
				}																																									\
			}																																										\
		}																																											\
		return writeComponents;																																						\
	}(system)


#define ECS__FILE__TABLE__SYSTEM__GET_READ_COMPONENT_ENTITY_IDS(system)																												\
	[this](ECS2::Entity::Id systemEntityId){																																		\
		ASSERT(IsComponentExist<ECS::File::Table::System::Tag>(systemEntityId));																									\
		std::vector<ECS2::Entity::Id> readComponents;																																\
		if (IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {																					\
			ECS2::Entity::Id updateMethodEntityId																																	\
			= GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;																					\
			ECS2::ComponentsFilter updateMethodCF = GetComponentsFilter(updateMethodEntityId);																						\
			if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {														\
				std::vector<ECS2::Entity::Id> processesEntities																														\
					= GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;											\
				for (ECS2::Entity::Id processesEntity : processesEntities) {																										\
					const auto& processComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processesEntity)->componentInfos_;	\
					for (auto& processComponentInfo : processComponentInfos) {																										\
						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(																			\
							ECS::File::Table::Component::Tag,																														\
							systemEntityId,																																			\
							processComponentInfo.name_);																															\
						if (processComponentInfo.readonly_) {																														\
							readComponents.push_back(componentEntityId);																											\
						}																																							\
					}																																								\
				}																																									\
			}																																										\
			if (IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)) {														\
				std::vector<ECS2::Entity::Id> accessesEntities																														\
					= GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)->entityIds_;											\
				for (ECS2::Entity::Id accessEntity : accessesEntities) {																											\
					const auto& accessComponentInfos = GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)->components_;			\
					for (auto& accessComponentInfo : accessComponentInfos) {																										\
						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(																			\
							ECS::File::Table::Component::Tag,																														\
							systemEntityId,																																			\
							accessComponentInfo.name_);																																\
						if (accessComponentInfo.readonly_) {																														\
							readComponents.push_back(componentEntityId);																											\
						}																																							\
					}																																								\
				}																																									\
			}																																										\
		}																																											\
		return readComponents;																																						\
	}(system)

#define ECS__FILE__TABLE__SYSTEM__HAS_RUN_BEFORE_SYSTEMS(systemE)																									\
	[this](ECS2::Entity::Id systemEntityId) -> bool {																												\
		ECS2::Entity::Id callOrderEntityId = GetComponent<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)->id_;							\
		const ECS::File::Table::System::CallOrder::RunBefore* systemRunBefore = GetComponent<ECS::File::Table::System::CallOrder::RunBefore>(callOrderEntityId);	\
		return !systemRunBefore->systems_.empty();																													\
	}(systemE)

#define ECS__FILE__TABLE__SYSTEM__HAS_RUN_AFTER_SYSTEMS(systemE)																									\
	[this](ECS2::Entity::Id systemEntityId) -> bool {																												\
		ECS2::Entity::Id callOrderEntityId = GetComponent<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)->id_;							\
		const ECS::File::Table::System::CallOrder::RunAfter* systemRunAfter = GetComponent<ECS::File::Table::System::CallOrder::RunAfter>(callOrderEntityId);		\
		return !systemRunAfter->systems_.empty();																													\
	}(systemE)


#define ECS__FILE__TABLE__SYSTEM__ADD_RUN_BEFORE_SYSTEM(systemE, runBeforeSystem)																					\
	[this](ECS2::Entity::Id systemEntityId, ECS2::Entity::Id runBeforeSystemEntityId) {																				\
		if(IsComponentExist<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)){																\
			ECS2::Entity::Id callOrderEntityId = GetComponent<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)->id_;						\
			if (IsComponentExist<ECS::File::Table::System::CallOrder::RunBefore>(callOrderEntityId)) {																\
				ECS::File::Table::System::CallOrder::RunBefore* systemRunBefore = GetComponent<ECS::File::Table::System::CallOrder::RunBefore>(callOrderEntityId);	\
				auto it = std::find_if(systemRunBefore->systems_.begin(), systemRunBefore->systems_.end(), [runBeforeSystemEntityId](auto& systemInfo){				\
					ASSERT(systemInfo.id_.IsValid());																												\
					ASSERT(OksEngine::ECS::File::Table::IsValidName(systemInfo.name_));																				\
					return systemInfo.id_ == runBeforeSystemEntityId;																								\
				});																																					\
				if(it == systemRunBefore->systems_.end()){																											\
					systemRunBefore->systems_.push_back({ runBeforeSystemEntityId, ECS__FILE__TABLE__GET_FULL_NAME(runBeforeSystemEntityId, "::", false) });		\
				}																																					\
			}																																						\
		}																																							\
	}(systemE, runBeforeSystem)

#define ECS__FILE__TABLE__SYSTEM__ADD_RUN_AFTER_SYSTEM(systemE, runAfterSystem)																						\
	[this](ECS2::Entity::Id systemEntityId, ECS2::Entity::Id runAfterSystemEntityId) {																				\
		if(IsComponentExist<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)){																\
			ECS2::Entity::Id callOrderEntityId = GetComponent<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)->id_;						\
			if (IsComponentExist<ECS::File::Table::System::CallOrder::RunAfter>(callOrderEntityId)) {																\
				ECS::File::Table::System::CallOrder::RunAfter* systemRunAfter = GetComponent<ECS::File::Table::System::CallOrder::RunAfter>(callOrderEntityId);		\
				auto it = std::find_if(systemRunAfter->systems_.begin(), systemRunAfter->systems_.end(), [runAfterSystemEntityId](auto& systemInfo){				\
					ASSERT(systemInfo.id_.IsValid());																												\
					ASSERT(OksEngine::ECS::File::Table::IsValidName(systemInfo.name_));																				\
					return systemInfo.id_ == runAfterSystemEntityId;																								\
				});																																					\
				if(it == systemRunAfter->systems_.end()){																											\
					systemRunAfter->systems_.push_back({ runAfterSystemEntityId, ECS__FILE__TABLE__GET_FULL_NAME(runAfterSystemEntityId, "::", false) });			\
				}																																					\
			}																																						\
		}																																							\
	}(systemE, runAfterSystem)

}
