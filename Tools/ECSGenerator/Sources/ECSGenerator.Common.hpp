#pragma once 


#include <Lua.Context.hpp>

#include <OS.CommandLineParameters.hpp>
#include <Resource.hpp>
#include <ECS.Entity.hpp>


namespace ECSGenerator{


	enum class Type {
		Std_string,
		Float,
		Int,
		Common_UInt64,
		Size,
		Index,
		ECS_Entity_Id
	};

	Type TypeStringToEnum(const std::string typeString) {
		static std::map<std::string, Type> strToEnum{
			{ "std::string", Type::Std_string },
			{ "float", Type::Float },
			{ "Common::UInt64", Type::Common_UInt64 },
			{ "Size", Type::Size },
			{ "Index", Type::Index },
			{ "ECS::Entity::Id", Type::ECS_Entity_Id }
		};
		OS::AssertMessage(strToEnum.contains(typeString), "");
		return strToEnum[typeString];
	}


	bool IsTypeCanBeEnteredFromImGui(const std::string& typeName) {
		static std::unordered_set<std::string> bindableType{
			"std::string",
			"float",
			"int",
			"Common::UInt64",
			"Common::Size",
			"Common::Index",
			"ECS::Entity::Id"
		};
		return bindableType.contains(typeName);
	}


	bool IsBindableType(const std::string& typeName) {
		static std::unordered_set<std::string> bindableType{
			"std::string",
			"float",
			"int",
			"Common::UInt64",
			"Common::Size",
			"Common::Index",
			"ECS::Entity::Id"
		};
		return bindableType.contains(typeName);
	}


	std::string GetImGuiType(const std::string& fieldType) {
		if (fieldType == "float") {
			return "ImGuiDataType_Float";
		}
		else if (fieldType == "Common::UInt64") {
			return "ImGuiDataType_U64";
		}
		else if (fieldType == "Common::Size") {
			return "ImGuiDataType_U64";
		}
		else if (fieldType == "Common::Index") {
			return "ImGuiDataType_U64";
		}
		else if (fieldType == "ECS::Entity::Id") {
			return "ImGuiDataType_U64";
		}
		OS::AssertFail();
		return "";
	}


}