#pragma once

#include <string>
#include <regex>
#include <string_view>
#include <algorithm>
#include <vector>
#include <luabridge3/LuaBridge/LuaBridge.h>

namespace OksEngine::ECS::File::Table {

	AI_GENERATED
		namespace Tags {

		// Разделитель иерархии
		constexpr char SEPARATOR = '.';

		// Разбить тег на сегменты: "ECS.Inspector.Physics" -> {"ECS", "Inspector", "Physics"}
		inline ::std::vector<::std::string> Split(const ::std::string& tag) {
			::std::vector<::std::string> parts;
			size_t start = 0;
			while (true) {
				size_t pos = tag.find(SEPARATOR, start);
				if (pos == ::std::string::npos) {
					parts.push_back(tag.substr(start));
					break;
				}
				parts.push_back(tag.substr(start, pos - start));
				start = pos + 1;
			}
			return parts;
		}

		// Собрать тег из сегментов: {"ECS","Inspector"} -> "ECS.Inspector"
		inline ::std::string Join(const ::std::vector<::std::string>& parts) {
			::std::string result;
			for (size_t i = 0; i < parts.size(); ++i) {
				if (i > 0) result += SEPARATOR;
				result += parts[i];
			}
			return result;
		}

		// Родительский тег: "ECS.Inspector.Physics" -> "ECS.Inspector"
		// Возвращает пустую строку, если родителя нет.
		inline ::std::string Parent(const ::std::string& tag) {
			size_t pos = tag.rfind(SEPARATOR);
			if (pos == ::std::string::npos) return {};
			return tag.substr(0, pos);
		}

		// Последний сегмент: "ECS.Inspector.Physics" -> "Physics"
		inline ::std::string Leaf(const ::std::string& tag) {
			size_t pos = tag.rfind(SEPARATOR);
			if (pos == ::std::string::npos) return tag;
			return tag.substr(pos + 1);
		}

		// Количество сегментов
		inline size_t Depth(const ::std::string& tag) {
			if (tag.empty()) return 0;
			return ::std::count(tag.begin(), tag.end(), SEPARATOR) + 1;
		}

		// Является ли `ancestor` предком `tag` (или равным ему)?
		// "ECS.Inspector" включает "ECS.Inspector.Physics" -> true
		// "ECS.Inspector" включает "ECS.Inspector"          -> true
		// "ECS.Inspector" включает "ECS.Other"              -> false
		// "ECS.Ins"       включает "ECS.Inspector"          -> false (не по границе сегмента)
		inline bool IsAncestorOf(const ::std::string& ancestor, const ::std::string& tag) {
			if (ancestor.empty()) return true; // пустой тег = "всё"
			if (tag.size() < ancestor.size()) return false;
			// Проверяем префикс
			if (tag.compare(0, ancestor.size(), ancestor) != 0) return false;
			// Граница: либо tag == ancestor, либо следующий символ — разделитель
			if (tag.size() == ancestor.size()) return true;
			return tag[ancestor.size()] == SEPARATOR;
		}

		// Эквивалентно: tag включает filter (то есть filter является предком или равен tag)
		// Удобный алиас с прямым порядком аргументов.
		inline bool Matches(const ::std::string& tag, const ::std::string& filter) {
			return IsAncestorOf(filter, tag);
		}

		// Проверить, что тег соответствует хотя бы одному фильтру из списка
		inline bool MatchesAny(const ::std::string& tag,
			const ::std::vector<::std::string>& filters) {
			for (const auto& f : filters) {
				if (Matches(tag, f)) return true;
			}
			return false;
		}

		// Проверить, что тег соответствует всем фильтрам из списка
		inline bool MatchesAll(const ::std::string& tag,
			const ::std::vector<::std::string>& filters) {
			for (const auto& f : filters) {
				if (!Matches(tag, f)) return false;
			}
			return true;
		}

		// Проверить, что система (у неё может быть несколько тегов) попадает хотя бы под один фильтр
		inline bool SystemMatchesAny(const ::std::vector<::std::string>& systemTags,
			const ::std::vector<::std::string>& filters) {
			for (const auto& t : systemTags) {
				if (MatchesAny(t, filters)) return true;
			}
			return false;
		}

		// Проверить, что у системы есть тег, совпадающий с любым из фильтров в списке
		inline bool HasAnyTag(const ::std::vector<::std::string>& systemTags,
			const ::std::vector<::std::string>& tags) {
			for (const auto& t : systemTags) {
				if (::std::find(tags.begin(), tags.end(), t) != tags.end()) return true;
			}
			return false;
		}


		// Является ли тег валидным (не пустой, без двойных точек и т.п.)
		inline bool IsValid(const ::std::string& tag) {
			if (tag.empty()) return false;
			if (tag.front() == SEPARATOR || tag.back() == SEPARATOR) return false;
			if (tag.find("..") != ::std::string::npos) return false;
			return true;
		}

		// Все предки тега: "ECS.Inspector.Physics" -> {"ECS", "ECS.Inspector", "ECS.Inspector.Physics"}
		inline ::std::vector<::std::string> AllAncestors(const ::std::string& tag) {
			::std::vector<::std::string> result;
			auto parts = Split(tag);
			::std::string current;
			for (size_t i = 0; i < parts.size(); ++i) {
				if (i > 0) current += SEPARATOR;
				current += parts[i];
				result.push_back(current);
			}
			return result;
		}

		// Являются ли теги в одном поддереве (есть общий предок, кроме корня)?
		inline bool SameSubtree(const ::std::string& a, const ::std::string& b) {
			return IsAncestorOf(a, b) || IsAncestorOf(b, a);
		}

	} // namespace ECS::Tags


	static inline bool IsValidName(const ::std::string& fullName) {
		::std::regex pattern(R"(^(::)?([A-Za-z_][A-Za-z0-9_]*)(::[A-Za-z_][A-Za-z0-9_]*)*$)");
		return ::std::regex_match(fullName, pattern);
	}

	AI_GENERATED
		static inline ::std::vector<::std::string> SplitName(const ::std::string& name) {
		::std::vector<::std::string> result;
		size_t start = 0;
		size_t pos;

		// Ищем разделители "::"
		while ((pos = name.find("::", start)) != ::std::string::npos) {
			// Извлекаем подстроку от start до pos
			::std::string token = name.substr(start, pos - start);
			if (!token.empty()) {
				result.push_back(token);
			}
			start = pos + 2; // пропускаем "::"
		}

		// Последний фрагмент (после последнего "::")
		::std::string last_token = name.substr(start);
		if (!last_token.empty()) {
			result.push_back(last_token);
		}
		ASSERT(result.size() >= 1);
		return result;
	}

	AI_GENERATED
		static inline ::std::string GetName(const ::std::string& fullName) {
		size_t pos = fullName.rfind("::");
		if (pos == ::std::string::npos) {
			return fullName;
		}
		return fullName.substr(pos + 2);
	}


#define ECS__FILE__TABLE__PARSE_CONSTANT(luaRef, name)\
    [this](luabridge::LuaRef constant,\
    const ::std::string& constantName){                               \
        const ECS2::Entity::Id constantEntityId = CreateEntity<ECS__FILE__TABLE__CONSTANT__CONSTANT>();   \
        luabridge::LuaRef type = constant["type"];                  \
        luabridge::LuaRef value = constant["value"];                \
        CreateComponent<ECS::File::Table::Tag>(constantEntityId);\
        CreateComponent<ECS::File::Table::Constant::Tag>(constantEntityId);\
        CreateComponent<ECS::File::Table::Name>(constantEntityId, constantName);\
        CreateComponent<Constant::TypeName>(constantEntityId, type.cast<::std::string>().value());\
        CreateComponent<Constant::Value>(constantEntityId, value.cast<::std::string>().value());\
        return  constantEntityId;       \
    }(luaRef, name);        
	//
	//#define ECS__FILE__TABLE__GET_NAMESPACE_ENTITY_IDS(entityId)\
	//    [this](ECS2::Entity::Id tableEntityId){\
	//		ASSERT(tableEntityId.IsValid());\
	//        ::std::vector<ECS2::Entity::Id> namespaces; \
	//        ::std::function<void(ECS2::Entity::Id)> processParent = \
	//        [this, \
	//        &namespaces, \
	//        &processParent](ECS2::Entity::Id parentEntityId) {\
	//            ECS2::ComponentsFilter parentTableCF = GetComponentsFilter(parentEntityId); \
	//            ASSERT(IsComponentExist<File::Table::Tag>(parentEntityId)); \
	//            ASSERT(IsComponentExist<File::Table::Namespace::Tag>(parentEntityId)); \
	//            ASSERT(IsComponentExist<File::Table::Name>(parentEntityId)); \
	//            ASSERT(IsComponentExist<File::Table::ChildTablesEntityIds>(parentEntityId)); \
	//            if (IsComponentExist<File::Table::ParentTableEntityId>(parentEntityId)) {\
	//                   processParent(GetComponent<File::Table::ParentTableEntityId>(parentEntityId)->id_); \
	//            }\
	//            namespaces.push_back(parentEntityId);\
	//        }; \
	//		if(IsComponentExist<File::Table::ParentTableEntityId>(tableEntityId)){\
	//			ECS2::Entity::Id parentTableEntityId = GetComponent<File::Table::ParentTableEntityId>(tableEntityId)->id_; \
	//			processParent(parentTableEntityId); \
	//		}\
	//        return namespaces;\
	//    }(entityId)


	template<class System>
	::std::vector<ECS2::Entity::Id> ECS__FILE__TABLE__GET_NAMESPACE_ENTITY_IDS(System* system, ECS2::Entity::Id tableEntityId) {
		ASSERT(tableEntityId.IsValid());
		::std::vector<ECS2::Entity::Id> namespaces;
		::std::function<void(ECS2::Entity::Id)> processParent =
			[system,
			&namespaces,
			&processParent](ECS2::Entity::Id parentEntityId) {
			ECS2::ComponentsFilter parentTableCF = system->GetComponentsFilter(parentEntityId);
			ASSERT(system->template IsComponentExist<File::Table::Tag>(parentEntityId));
			ASSERT(system->template IsComponentExist<File::Table::Namespace::Tag>(parentEntityId));
			ASSERT(system->template IsComponentExist<File::Table::Name>(parentEntityId));
			ASSERT(system->template IsComponentExist<File::Table::ChildTablesEntityIds>(parentEntityId));
			if (system->template IsComponentExist<File::Table::ParentTableEntityId>(parentEntityId)) {
				processParent(system->template GetComponent<File::Table::ParentTableEntityId>(parentEntityId)->id_);
			}
			namespaces.push_back(parentEntityId);
			};
		if (system->template IsComponentExist<File::Table::ParentTableEntityId>(tableEntityId)) {
			ECS2::Entity::Id parentTableEntityId = system->template GetComponent<File::Table::ParentTableEntityId>(tableEntityId)->id_;
			processParent(parentTableEntityId);
		}
		return namespaces;
	}

	template<class System>
	::std::vector<::std::string> ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(System* system, ECS2::Entity::Id tableEntityId) {
		::std::vector<ECS2::Entity::Id> namespaces = ECS__FILE__TABLE__GET_NAMESPACE_ENTITY_IDS(system, tableEntityId);
		::std::vector<::std::string> namespaceNames;
		for (ECS2::Entity::Id namespaceEntityId : namespaces) {
			namespaceNames.push_back(system->template GetComponent<ECS::File::Table::Name>(namespaceEntityId)->name_);
		}
		return namespaceNames;
	}

	template<class System>
	::std::string ECS__FILE__TABLE__GET_SPLITED_FULL_NAME(System* system, ECS2::Entity::Id tableEntityId) {
		::std::vector<::std::string> tableNames = File::Table::ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(system, tableEntityId);
		tableNames.push_back(system->template GetComponent<ECS::File::Table::Name>(tableEntityId)->name_);
		return tableNames;
	}

	//#define ECS__FILE__TABLE__GET_FULL_NAMESPACE_STRING(entityId, delimiterString, isNamespaceUppercase)\
	//		[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
	//			ECS2::ComponentsFilter beginTableCF = GetComponentsFilter(tableEntityId);\
	//			::std::vector<::std::string> namespaceStrings = ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(tableEntityId);\
	//			::std::string resultNamespace;\
	//			for (Common::Index i = 0; i < namespaceStrings.size(); i++) {\
	//				::std::string& string = namespaceStrings[i];\
	//				if (isUppercase) {\
	//					/*Change values in namespaceStrings, we will not use it later.*/\
	//					::std::transform(string.begin(), string.end(), string.begin(),\
	//						[](unsigned char c) { return ::std::toupper(c); });\
	//				}\
	//				resultNamespace += string;\
	//				if (i != namespaceStrings.size() - 1) {\
	//					resultNamespace += delimiter;\
	//				}\
	//			}\
	//			return resultNamespace;\
	//			}(entityId,delimiterString,isNamespaceUppercase)

	template<class System>
	::std::string ECS__FILE__TABLE__GET_FULL_NAMESPACE_STRING(System* system, ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {
		ECS2::ComponentsFilter beginTableCF = system->GetComponentsFilter(tableEntityId);
		::std::vector<::std::string> namespaceStrings = File::Table::ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(system, tableEntityId);
		::std::string resultNamespace;
		for (Common::Index i = 0; i < namespaceStrings.size(); i++) {
			::std::string& string = namespaceStrings[i];
			if (isUppercase) {
				/*Change values in namespaceStrings, we will not use it later.*/
				::std::transform(string.begin(), string.end(), string.begin(),
					[](unsigned char c) { return ::std::toupper(c); });
			}
			resultNamespace += string;
			if (i != namespaceStrings.size() - 1) {
				resultNamespace += delimiter;
			}
		}
		return resultNamespace;
	}

	//#define ECS__FILE__TABLE__GET_NAME(entityId, isNamespaceUppercase)\
	//			[this](ECS2::Entity::Id tableEntityId, bool isUppercase) {\
	//				ASSERT(IsComponentExist<File::Table::Tag>(tableEntityId));\
	//				ASSERT(IsComponentExist<File::Table::Name>(tableEntityId));\
	//				::std::string tableName = GetComponent<File::Table::Name>(tableEntityId)->name_;\
	//				if (isUppercase) {\
	//					::std::transform(tableName.begin(), tableName.end(), tableName.begin(),\
	//						[](unsigned char c) { return ::std::toupper(c); });\
	//				}\
	//				return tableName;\
	//			}(entityId, isNamespaceUppercase)

	template<class System>
	::std::string ECS__FILE__TABLE__GET_NAME(System* system, ECS2::Entity::Id tableEntityId, bool isUppercase) {
		ASSERT(system->template IsComponentExist<File::Table::Tag>(tableEntityId));
		ASSERT(system->template IsComponentExist<File::Table::Name>(tableEntityId));
		::std::string tableName = system->template GetComponent<File::Table::Name>(tableEntityId)->name_;
		if (isUppercase) {
			::std::transform(tableName.begin(), tableName.end(), tableName.begin(),
				[](unsigned char c) { return ::std::toupper(c); });
		}
		return tableName;
	}

	//#define ECS__FILE__TABLE__GET_FULL_NAME(entityId, delimiterString, isNamespaceUppercase)\
	//			[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
	//				::std::string tableName = ECS__FILE__TABLE__GET_NAME(tableEntityId, isNamespaceUppercase);\
	//				const ::std::string tableNamespace = ECS__FILE__TABLE__GET_FULL_NAMESPACE_STRING(tableEntityId, delimiter, isUppercase);\
	//				return tableNamespace + delimiter + tableName;\
	//			}(entityId, delimiterString, isNamespaceUppercase)

	template<class System>
	::std::string ECS__FILE__TABLE__GET_FULL_NAME(System* system, ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {
		::std::string tableName = ECS__FILE__TABLE__GET_NAME(system, tableEntityId, isUppercase);
		const ::std::string tableNamespace = ECS__FILE__TABLE__GET_FULL_NAMESPACE_STRING(system, tableEntityId, delimiter, isUppercase);
		return tableNamespace + delimiter + tableName;
	}

#define ECS__FILE__TABLE__GET_ALL_COMPONENT_ENTITY_IDS()																			\
		[this]() {																													\
			const Common::Size allComponentsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__COMPONENT__COMPONENT>();			\
			const auto componentComponents = GetComponents<ECS__FILE__TABLE__COMPONENT__COMPONENT>();								\
			const ECS2::Entity::Id* componentEntityIdsPtr = ::std::get<ECS2::Entity::Id*>(componentComponents);						\
			::std::vector<ECS2::Entity::Id> componentEntityIds{ componentEntityIdsPtr, componentEntityIdsPtr + allComponentsNumber };	\
			return componentEntityIds;																								\
		}()

	inline ::std::vector<::std::string> GetCommonNamespace(
		const ::std::vector<::std::string>& first,
		const ::std::vector<::std::string>& second) {
		
		::std::vector<::std::string> commonNamespace;

		auto firstBegin = first.begin(), firstEnd = first.end();
		auto secondBegin = second.begin(), secondEnd = second.end();
		std::size_t n = 0;
		for (; firstBegin != firstEnd && secondBegin != secondEnd; ++firstBegin, ++secondBegin) {
			if (*firstBegin != *secondBegin) break;
			commonNamespace.push_back(*firstBegin);
		}

		return commonNamespace;
	}

	inline bool HasCommonNamespace(
		const ::std::vector<::std::string>& first,
		const ::std::vector<::std::string>& second) {
		if (second.empty()) {
			return true;
		}
		for (auto rIt = first.rbegin(); rIt != first.rend(); ++rIt) {
			if (*rIt == *second.begin()) {
				const Common::UInt64 offset = ::std::distance(rIt, first.rend()) - 1;
				if (first.size() - offset != second.size()) {
					return false;
				}
				for (Common::Index j = offset; j < first.size(); j++) {
					if (first[j] != second[j - offset]) {
						return false;
					}
				}
				return true;
			}
		}
		return false;
	}

	template<class System, class TableTag>
	ECS2::Entity::Id ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME_OR_INVALID_ID(System* system, ECS2::Entity::Id usageTableEntityId, const ::std::string& findingFullTableName) {
		BEGIN_PROFILE("ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME");
		ASSERT(OksEngine::ECS::File::Table::IsValidName(findingFullTableName));
		::std::vector<::std::string> systemNamespaceNames = File::Table::ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(system, usageTableEntityId);
		systemNamespaceNames.insert(systemNamespaceNames.begin(), "Root");

		::std::vector<::std::string> findingTableSplitedFullName = ECS::File::Table::SplitName(findingFullTableName);
		if (findingFullTableName.starts_with("::")) {
			findingTableSplitedFullName.insert(findingTableSplitedFullName.begin(), "Root");
		}
		const ::std::string findingTableName = findingTableSplitedFullName.back();

		::std::vector<::std::string> findingTableSplitedNamespaces = findingTableSplitedFullName;
		findingTableSplitedNamespaces.pop_back();

		//At first try to find need table in the context.
		{

		}


		::std::vector<::std::tuple<Common::Size, ECS2::Entity::Id, std::vector<std::string>>> applicants; // Distance(namespace path/depth) and table id
		bool foundApplicantInContext = false;
		system->world_->ForEachEntity<ECS::File::Table::Tag>([&](ECS2::Entity::Id tableEntityId) {
			if (foundApplicantInContext) {
				return;
			}
			const ECS2::ComponentsFilter tableCF = system->GetComponentsFilter(tableEntityId);
			if (!tableCF.IsSet<TableTag>()) {
				return;
			}
			const ::std::string tableName = system->template GetComponent<ECS::File::Table::Name>(tableEntityId)->name_;
			if (tableName != findingTableName) {
				return;
			}
			::std::vector<::std::string> tableSplitedNamespaces = File::Table::ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(system, tableEntityId);
			tableSplitedNamespaces.insert(tableSplitedNamespaces.begin(), "Root");
			bool isFindingAndCurrentTablesHasCommonNamespace = HasCommonNamespace(tableSplitedNamespaces, findingTableSplitedNamespaces);
			if (!isFindingAndCurrentTablesHasCommonNamespace) {
				return;
			}
			
			//if (HasCommonNamespace(systemNamespaceNames, tableSplitedNamespaces)) {
				applicants.push_back({ GetCommonNamespace(systemNamespaceNames, tableSplitedNamespaces).size(), tableEntityId, tableSplitedNamespaces });
			//}

			});
		if (applicants.empty()) {
			return ECS2::Entity::Id::invalid_;
		}

		::std::sort(applicants.begin(), applicants.end(),
			[](::std::tuple<Common::Size, ECS2::Entity::Id, std::vector<std::string>>& first, ::std::tuple<Common::Size, ECS2::Entity::Id, std::vector<std::string>>& second) {
				return	std::get<0>(first) > std::get<0>(second);
			});



		//At first need to find table in the namespace of context.
		//for (const auto& [distance, tableEntityId, tableNamespace] : applicants) {
		//	::std::vector<::std::string> tableSplitedNamespaces = File::Table::ECS__FILE__TABLE__GET_SPLITED_NAMESPACE_NAMES(system, tableEntityId);


		//}



		END_PROFILE();
		return std::get<1>(applicants.front());
	}

	template<class System, class TableTag>
	ECS2::Entity::Id ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(System* system, ECS2::Entity::Id usageTableEntityId, const ::std::string& findingFullTableName) {
		const ECS2::Entity::Id tableEntityId = File::Table::ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME_OR_INVALID_ID<System, TableTag>(system, usageTableEntityId, findingFullTableName);
		ASSERT_FMSG(
			tableEntityId.IsValid(), 
			"Can't find {} in context {}.", findingFullTableName, ECS__FILE__TABLE__GET_FULL_NAME(system, usageTableEntityId, "::", false));
		return tableEntityId;

	}

	template<class System>
	ECS2::Entity::Id ECS__FILE__TABLE__GET_TABLE_FILE_ENTITY(System* system, ECS2::Entity::Id tableEntityId) {
		return system->template GetComponent<ECS::File::EntityId>(tableEntityId)->id_;
	}

	template<class System>
	::std::filesystem::path ECS__FILE__GET_FILE_INCLUDE_PATH(System* system, ECS2::Entity::Id fileEntityId, ::std::filesystem::path projectFileFullPath, ::std::string extensionStr) {
		auto projectFilePathIt = projectFileFullPath.begin();
		::std::filesystem::path ecsFilePath = system->template GetComponent<ECS::File::Path>(fileEntityId)->path_;
		if(ecsFilePath.is_absolute()){
			::std::filesystem::path relative =ecsFilePath.lexically_relative(projectFileFullPath); 
			return relative.parent_path() / ("auto_" + relative.stem().string() + extensionStr);
		}
		auto hppFilePathIt = ecsFilePath.begin();
		while (*projectFilePathIt == *hppFilePathIt) {
			++projectFilePathIt;
			++hppFilePathIt;
		}
		::std::filesystem::path hppFileRelativePath;
		ASSERT(*hppFilePathIt == "Sources");
		++hppFilePathIt;
		while (hppFilePathIt != ecsFilePath.end()) {
			hppFileRelativePath /= *hppFilePathIt;
			++hppFilePathIt;
		}
		hppFileRelativePath = hppFileRelativePath.parent_path() / ("auto_" + hppFileRelativePath.stem().string() + extensionStr);
		return hppFileRelativePath;
	}

	template<class System>
	::std::vector<ECS2::Entity::Id> ECS__FILE__TABLE__SYSTEM__GET_WRITE_COMPONENT_ENTITY_IDS(System* system, ECS2::Entity::Id systemEntityId) {
		ASSERT(system->template IsComponentExist<ECS::File::Table::System::Tag>(systemEntityId));
		::std::vector<ECS2::Entity::Id> writeComponents;
		if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {
			ECS2::Entity::Id updateMethodEntityId
				= system->template GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;
			ECS2::ComponentsFilter updateMethodCF = system->GetComponentsFilter(updateMethodEntityId);
			if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {
				::std::vector<ECS2::Entity::Id> processesEntities
					= system->template GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;
				for (ECS2::Entity::Id processesEntity : processesEntities) {
					const bool allCombinations = system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessAllCombinations>(processesEntity);
					if (allCombinations) continue;
					const auto& processComponentInfos = system->template GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processesEntity)->componentInfos_;
					for (auto& processComponentInfo : processComponentInfos) {
						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME<System, ECS::File::Table::Component::Tag>(
							system,
							systemEntityId,
							processComponentInfo.name_);
						if (!processComponentInfo.readonly_) {
							writeComponents.push_back(componentEntityId);
						}
					}
				}
			}
			if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)) {
				::std::vector<ECS2::Entity::Id> accessesEntities
					= system->template GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)->entityIds_;
				for (ECS2::Entity::Id accessEntity : accessesEntities) {
					if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)) {
						const auto& accessComponentInfos = system->template GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)->components_;
						for (auto& accessComponentInfo : accessComponentInfos) {
							ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME<System, ECS::File::Table::Component::Tag>(
								system,
								systemEntityId,
								accessComponentInfo.name_);
							if (!accessComponentInfo.readonly_) {
								writeComponents.push_back(componentEntityId);
							}
						}
					}
					//archetype
					if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::Archetype>(accessEntity)) {
						const std::string archetypeLocalName = system->template  GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::Archetype>(accessEntity)->name_;
						ECS2::Entity::Id archetypeEntityId = File::Table::ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME<System, ECS::File::Table::Archetype::Tag>(system, systemEntityId, archetypeLocalName);
						//TODO: Get archetype components

					}
				}
			}
		}
		return writeComponents;
	}

	template<class System>
	::std::vector<ECS2::Entity::Id> ECS__FILE__TABLE__SYSTEM__GET_READ_COMPONENT_ENTITY_IDS(System* system, ECS2::Entity::Id systemEntityId) {
		ASSERT(system->template IsComponentExist<ECS::File::Table::System::Tag>(systemEntityId));
		::std::vector<ECS2::Entity::Id> readComponents;
		if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {
			ECS2::Entity::Id updateMethodEntityId
				= system->template GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;
			ECS2::ComponentsFilter updateMethodCF = system->GetComponentsFilter(updateMethodEntityId);
			if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {
				::std::vector<ECS2::Entity::Id> processesEntities
					= system->template GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;
				for (ECS2::Entity::Id processesEntity : processesEntities) {
					const bool allCombinations = system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessAllCombinations>(processesEntity);
					if (allCombinations) continue;

					
					if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::Archetype>(processesEntity)) {
						const auto* archetype = system->template GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::Archetype>(processesEntity);
						
					}
					const auto& processComponentInfos = system->template GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processesEntity)->componentInfos_;
					for (auto& processComponentInfo : processComponentInfos) {
						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME<System, ECS::File::Table::Component::Tag >(
							system,
							systemEntityId,
							processComponentInfo.name_);
						if (processComponentInfo.readonly_) {
							readComponents.push_back(componentEntityId);
						}
					}
				}
			}
			if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)) {
				::std::vector<ECS2::Entity::Id> accessesEntities
					= system->template GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)->entityIds_;
				for (ECS2::Entity::Id accessEntity : accessesEntities) {
					if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)) {
						const auto& accessComponentInfos = system->template GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)->components_;
						for (auto& accessComponentInfo : accessComponentInfos) {
							ECS2::Entity::Id componentEntityId = File::Table::ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME<System, ECS::File::Table::Component::Tag>(
								system,
								systemEntityId,
								accessComponentInfo.name_);
							if (accessComponentInfo.readonly_) {
								readComponents.push_back(componentEntityId);
							}
						}
					}
				}
			}
		}
		return readComponents;
	}

	//template<class System>
	//::std::vector<ECS2::Entity::Id> ECS__FILE__TABLE__SYSTEM__GET_ARCHETYPES_USED(System* system, ECS2::Entity::Id systemEntityId) {
	//	ASSERT(system->template IsComponentExist<ECS::File::Table::System::Tag>(systemEntityId));
	//	::std::vector<ECS2::Entity::Id> readComponents;
	//	if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)) {
	//		ECS2::Entity::Id updateMethodEntityId
	//			= system->template GetComponent<ECS::File::Table::System::UpdateMethod::EntityId>(systemEntityId)->id_;
	//		ECS2::ComponentsFilter updateMethodCF = system->GetComponentsFilter(updateMethodEntityId);
	//		if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)) {
	//			::std::vector<ECS2::Entity::Id> processesEntities
	//				= system->template GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::EntityIds>(updateMethodEntityId)->entityIds_;
	//			for (ECS2::Entity::Id processesEntity : processesEntities) {
	//				const bool allCombinations = system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessAllCombinations>(processesEntity);
	//				if (allCombinations) continue;
	//				const auto& processComponentInfos = system->template GetComponent<ECS::File::Table::System::UpdateMethod::Process::Entity::ProcessComponents>(processesEntity)->componentInfos_;
	//				for (auto& processComponentInfo : processComponentInfos) {
	//					ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(
	//						system,
	//						ECS::File::Table::Component::Tag,
	//						systemEntityId,
	//						processComponentInfo.name_);
	//					if (processComponentInfo.readonly_) {
	//						readComponents.push_back(componentEntityId);
	//					}
	//				}
	//			}
	//		}
	//		if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)) {
	//			::std::vector<ECS2::Entity::Id> accessesEntities
	//				= system->template GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::EntityIds>(updateMethodEntityId)->entityIds_;
	//			for (ECS2::Entity::Id accessEntity : accessesEntities) {
	//				if (system->template IsComponentExist<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)) {
	//					const auto& accessComponentInfos = system->template GetComponent<ECS::File::Table::System::UpdateMethod::Access::Entity::AccessComponents>(accessEntity)->components_;
	//					for (auto& accessComponentInfo : accessComponentInfos) {
	//						ECS2::Entity::Id componentEntityId = ECS__FILE__TABLE__GET_TABLE_ENTITY_ID_BY_NAME(
	//							system,
	//							ECS::File::Table::Component::Tag,
	//							systemEntityId,
	//							accessComponentInfo.name_);
	//						if (accessComponentInfo.readonly_) {
	//							readComponents.push_back(componentEntityId);
	//						}
	//					}
	//				}
	//			}
	//		}
	//		return readComponents;
	//	}
	//}

	//#define  ECS__FILE__TABLE__SYSTEM__GET_COMPONENTS_ENTITY_IDS(system)																												\
	//	[this](ECS2::Entity::Id systemEntityId){																																		\
	//		const auto readComponents = ECS__FILE__TABLE__SYSTEM__GET_READ_COMPONENT_ENTITY_IDS(systemEntityId);																		\
	//		const auto writeComponents = ECS__FILE__TABLE__SYSTEM__GET_WRITE_COMPONENT_ENTITY_IDS(systemEntityId);																		\
	//		::std::vector<ECS2::Entity::Id> allComponents = readComponents;																												\
	//		allComponents.insert(allComponents.end(), writeComponents.begin(), writeComponents.end());																					\
	//		::std::sort(allComponents.begin(), allComponents.end());																														\
	//		allComponents.erase(::std::unique(allComponents.begin(), allComponents.end()), allComponents.end());																			\
	//		return allComponents;																																						\
	//	}(system)

	template<class System>
	::std::vector<ECS2::Entity::Id> ECS__FILE__TABLE__SYSTEM__GET_COMPONENTS_ENTITY_IDS(System* system, ECS2::Entity::Id systemEntityId) {
		const auto readComponents = ECS__FILE__TABLE__SYSTEM__GET_READ_COMPONENT_ENTITY_IDS(system, systemEntityId);
		const auto writeComponents = ECS__FILE__TABLE__SYSTEM__GET_WRITE_COMPONENT_ENTITY_IDS(system, systemEntityId);
		::std::vector<ECS2::Entity::Id> allComponents = readComponents;
		allComponents.insert(allComponents.end(), writeComponents.begin(), writeComponents.end());
		::std::sort(allComponents.begin(), allComponents.end());
		allComponents.erase(::std::unique(allComponents.begin(), allComponents.end()), allComponents.end());
		return allComponents;
	}

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


	template<class System>
	void ECS__FILE__TABLE__SYSTEM__ADD_RUN_BEFORE_SYSTEM(System* system, ECS2::Entity::Id systemEntityId, ECS2::Entity::Id runBeforeSystemEntityId) {
		if (system->template IsComponentExist<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)) {
			ECS2::Entity::Id callOrderEntityId = system->template GetComponent<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)->id_;
			if (system->template IsComponentExist<ECS::File::Table::System::CallOrder::RunBefore>(callOrderEntityId)) {
				ECS::File::Table::System::CallOrder::RunBefore* systemRunBefore = system->template GetComponent<ECS::File::Table::System::CallOrder::RunBefore>(callOrderEntityId);
				auto it = ::std::find_if(systemRunBefore->systems_.begin(), systemRunBefore->systems_.end(), [runBeforeSystemEntityId](auto& systemInfo) {
					/*Can be invalid.*/
					/*ASSERT(systemInfo.id_.IsValid());*/
					ASSERT(OksEngine::ECS::File::Table::IsValidName(systemInfo.name_));
					return systemInfo.id_ == runBeforeSystemEntityId;
					});
				if (it == systemRunBefore->systems_.end()) {
					systemRunBefore->systems_.push_back({ runBeforeSystemEntityId, ECS__FILE__TABLE__GET_FULL_NAME(system, runBeforeSystemEntityId, "::", false) });
				}
			}
		}
	}

	template<class System>
	void ECS__FILE__TABLE__SYSTEM__ADD_RUN_AFTER_SYSTEM(System* system, ECS2::Entity::Id systemEntityId, ECS2::Entity::Id runAfterSystemEntityId) {
		if (system->template IsComponentExist<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)) {
			ECS2::Entity::Id callOrderEntityId = system->template GetComponent<OksEngine::ECS::File::Table::System::CallOrder::EntityId>(systemEntityId)->id_;
			if (system->template IsComponentExist<ECS::File::Table::System::CallOrder::RunAfter>(callOrderEntityId)) {
				ECS::File::Table::System::CallOrder::RunAfter* systemRunAfter = system->template GetComponent<ECS::File::Table::System::CallOrder::RunAfter>(callOrderEntityId);
				auto it = ::std::find_if(systemRunAfter->systems_.begin(), systemRunAfter->systems_.end(), [runAfterSystemEntityId](auto& systemInfo) {
					/*Can be invalid.*/
					/*ASSERT(systemInfo.id_.IsValid());*/
					ASSERT(OksEngine::ECS::File::Table::IsValidName(systemInfo.name_));
					return systemInfo.id_ == runAfterSystemEntityId;
					});
				if (it == systemRunAfter->systems_.end()) {
					systemRunAfter->systems_.push_back({ runAfterSystemEntityId, ECS__FILE__TABLE__GET_FULL_NAME(system, runAfterSystemEntityId, "::", false) });
				}
			}
		}
	}

#define ECS__FILE__TABLE__SYSTEM__IS_DISABLED(system)										\
	[this](ECS2::Entity::Id systemEntityId){										\
		return IsComponentExist<ECS::File::Table::System::Disabled>(systemEntityId);\
	}(system)

	template<class System>
	bool IsSystemDisabled(System* system, ECS2::Entity::Id systemEntityId) {
		return system->template IsComponentExist<ECS::File::Table::System::Disabled>(systemEntityId);
	}
}
