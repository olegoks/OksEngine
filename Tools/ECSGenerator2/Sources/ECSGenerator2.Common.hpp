#pragma once 

#include <filesystem>

#include <Lua.Context.hpp>

#include <OS.CommandLineParameters.hpp>
#include <Resource.hpp>
#include <unordered_set>

namespace ECSGenerator2{


	enum class Type {
		Std_string,
		Float,
		Int,
		Common_UInt64,
		Size,
		Index,
		ECS_Entity_Id,
		std_vector_ECS_Entity_Id
	};

	inline Type TypeStringToEnum(const std::string typeString) {
		static std::map<std::string, Type> strToEnum{
			{ "std::string", Type::Std_string },
			{ "float", Type::Float },
			{ "Common::UInt64", Type::Common_UInt64 },
			{ "Size", Type::Size },
			{ "Index", Type::Index },
			{ "ECS2::Entity::Id", Type::ECS_Entity_Id },
			{ "std::vector<ECS2::Entity::Id>", Type::std_vector_ECS_Entity_Id }
		};
		OS::AssertMessage(strToEnum.contains(typeString), "");
		return strToEnum[typeString];
	}


	inline bool IsTypeCanBeEnteredFromImGui(const std::string& typeName) {
		static std::unordered_set<std::string> bindableType{
			"std::string",
			"float",
			"int",
			"Common::UInt64",
			"Common::UInt32",
			"Common::Size",
			"Common::Index",
			"ECS2::Entity::Id",
			"std::vector<ECS2::Entity::Id>"
		};
		return bindableType.contains(typeName);
	}


	inline bool IsBindableType(const std::string& typeName) {
		static std::unordered_set<std::string> bindableType{
			"std::string",
			"float",
			"int",
			"Common::UInt64",
			"Common::Size",
			"Common::Index",
			"ECS2::Entity::Id"
		};
		return bindableType.contains(typeName);
	}


	inline std::string GetImGuiType(const std::string& fieldType) {
		if (fieldType == "float") {
			return "ImGuiDataType_Float";
		}
		else if (fieldType == "Common::UInt64") {
			return "ImGuiDataType_U64";
		}
		else if (fieldType == "Common::UInt32") {
			return "ImGuiDataType_U32";
		}
		else if (fieldType == "Common::Size") {
			return "ImGuiDataType_U64";
		}
		else if (fieldType == "Common::Index") {
			return "ImGuiDataType_U64";
		}
		else if (fieldType == "ECS2::Entity::Id") {
			return "ImGuiDataType_U64";
		}
		OS::AssertFail();
		return "";
	}


	inline void FormatPath(std::filesystem::path& path) {
		std::string pathString = path.string();
		std::replace(pathString.begin(), pathString.end(), '\\', '/');
		path = pathString;
	}

	//// Пользовательский литерал _fmt
	//template <typename... Args>
	//std::string operator""_fmt(const char* str, size_t, Args&&... args) {
	//	return std::format(str, std::forward<Args>(args)...);
	//}


	enum class SearchDirection {
		FromEndToBegin,
		FromBeginToEnd,
		Undefined
	};

	enum class ResultRange {
		FromStartFolderToEnd,
		FromStartToStartFolder,
		Undefined
	};

	inline std::filesystem::path GetSubPath(
		const std::filesystem::path& path,
		const std::string& startFolder,
		ResultRange resultRange,
		SearchDirection startFolderSearchAlgo,
		bool skipStartFolder = true) {

		std::filesystem::path result;
		std::filesystem::path::iterator startFolderIt;

		if (startFolderSearchAlgo == SearchDirection::FromBeginToEnd) {
			for (auto it = path.begin(); it != path.end(); ++it) {
				if (*it == startFolder) {
					startFolderIt = it;
					break;
				}
			}
		}
		else if (startFolderSearchAlgo == SearchDirection::FromEndToBegin) {
			for (auto it = path.end(); it != path.begin(); --it) {
				if (*it == startFolder) {
					startFolderIt = it;
					break;
				}
			}
		}
		//
		//if (skipStartFolder) {
		//	if (resultRange == ResultRange::FromStartFolderToEnd) {
		//		++startFolderIt;
		//	}
		//	else if (resultRange == ResultRange::FromStartToStartFolder) {
		//		--startFolderIt;
		//	}
		//}

		if (resultRange == ResultRange::FromStartFolderToEnd) {
#pragma region Assert
			OS::AssertMessage(startFolderIt != path.end(), "");
#pragma endregion

			for (auto it = (skipStartFolder) ? (++startFolderIt) : (startFolderIt) ;
				it != path.end();
				++it) {
				result /= *it;
			}
		}
		else if (resultRange == ResultRange::FromStartToStartFolder) {
			for (auto it = path.begin();
				it != startFolderIt;
				++it) {
				result /= *it;
			}
			if (!skipStartFolder) {
				result /= *startFolderIt;
			}
		}
		FormatPath(result);
		return result;
	}



}