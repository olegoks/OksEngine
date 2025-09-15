#pragma once 

#include <ECSGenerator2.Common.hpp>
#include <ECSGenerator2.CodeFile.hpp>

#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <random>  
#include <format>

#include <ECSGenerator2.ParsedTable.hpp>

namespace ECSGenerator2 {

	class ParsedComponent : public ParsedTable {
	public:

		struct FieldInfo {
			Type type_;
			std::string typeName_;
			std::string name_;
			bool copyable_ = true;

			const std::string& GetName() const {
				return name_;
			}
			const std::string& GetTypeName() const {
				return typeName_;
			}
		};

		struct CreateInfo {
			std::string name_;
			bool serializable_ = true;		
			bool manualEditFunction_ = false;
			bool manualBindFunction_ = false;
			bool manualAddFunction_ = false;
			bool manualParseFunction_ = false;
			bool manualSerializeFunction_ = false;
			std::vector<FieldInfo> fields_;
		};

		ParsedComponent(const CreateInfo& createInfo) :
			ci_{ createInfo } { }

		virtual ParsedTable::Type GetType() const noexcept override {
			return ParsedTable::Type::Component;
		}

		virtual std::shared_ptr<ParsedTable> Clone() const noexcept override {
			return std::make_shared<ParsedComponent>(*this);
		}

		bool AreThereFields() const {
			return !ci_.fields_.empty();
		}

		[[nodiscard]]
		bool IsNeedToImplementHelpFunction() const {
			return 
				ci_.manualEditFunction_ 
				|| ci_.manualBindFunction_ 
				|| ci_.manualAddFunction_ 
				|| ci_.manualParseFunction_ 
				|| ci_.manualSerializeFunction_;
		}

		using ProcessField = std::function<bool(const FieldInfo& fieldInfo, bool isLast)>;

		void ForEachField(ProcessField&& processField) const {
			for (Common::Index i = 0; i < ci_.fields_.size(); i++) {
				const FieldInfo info = ci_.fields_[i];
				if (!processField(info, (i == ci_.fields_.size() - 1))) {
					break;
				}
			}
		}

		[[nodiscard]]
		bool IsContainsFieldWithType(std::string typeName) const {

			bool isContainsFieldWithType = false;

			ForEachField([&](const FieldInfo& fieldInfo, bool isLast) {
				
				if (fieldInfo.typeName_ == typeName) {
					isContainsFieldWithType = true;
					return false;
				}

				return true;
				});

			return isContainsFieldWithType;
		}


		bool CanBeCreatedFromImGui() const {
			bool canCreateFromImgui = true;
			ForEachField([&canCreateFromImgui](const FieldInfo& fieldInfo, bool isLast) {
				if (!IsTypeCanBeEnteredFromImGui(fieldInfo.GetTypeName())) {
					canCreateFromImgui = false;
					return false;
				}
				return true;
				});
			return canCreateFromImgui;
		}

		[[nodiscard]]
		virtual const std::string& GetName() const noexcept override {
			return ci_.name_;
		}

		[[nodiscard]]
		std::string GetLowerName() {
			return std::string{ static_cast<char>(std::tolower(GetName()[0])) } + GetName().substr(1);
		}

		CreateInfo ci_;

	};

	using ParsedComponentPtr = std::shared_ptr<ParsedComponent>;

	inline std::vector<std::string> GetComponentNamespace(ParsedComponentPtr parsedComponent) {

		std::vector<std::string> componentNamespace;

		parsedComponent->ForEachParentTable([&](ParsedTablePtr parsedTable) {

			componentNamespace.insert(componentNamespace.begin(), parsedTable->GetName());

			return true;

			});

		return componentNamespace;
	}

	inline std::shared_ptr<ParsedComponent> ParseComponent(
		luabridge::LuaRef component,
		const std::string& componentName) {

		bool serializable = true;
		luabridge::LuaRef serializableRef = component["serializable"];
		if (!serializableRef.isNil()) {
			serializable = serializableRef.cast<bool>().value();
		}
		luabridge::LuaRef fields = component["fields"];

		std::vector<ParsedComponent::FieldInfo> parsedFields;

		if (!fields.isNil()) {
			for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
				luabridge::LuaRef field = it.value();
				ParsedComponent::FieldInfo fieldInfo{
					.typeName_ = field["type"].cast<std::string>().value(),
					.name_ = field["name"].cast<std::string>().value()
				};

				if (!field["copyable"].isNil()) {
					fieldInfo.copyable_ = field["copyable"].cast<bool>().value();
				}

				parsedFields.push_back(fieldInfo);
			}
		}
		ParsedComponent::CreateInfo ci{
			.name_ = componentName,
			.serializable_ = serializable,
			.fields_ = parsedFields
		};
		auto parsedComponentFile = std::make_shared<ParsedComponent>(ci);
		ASSERT_FMSG(std::isupper(parsedComponentFile->GetName()[0]), "First component name symbol must be uppercase.");
		return parsedComponentFile;
	}


	//static char {variableName}[256] = "";
	//static Common::UInt64 {variableName};
	//CodeStructure::Code GenerateTypeImGuiInputVariable(const std::string& typeName, std::string variableName) {
	//	CodeStructure::Code code;
	//	const std::string& fieldTypeName = typeName;
	//	if (fieldTypeName == "std::string") {
	//		code.Add("static char " + variableName + "[4096] = \"\";");
	//		return code;
	//	}
	//	else if (
	//		fieldTypeName == "float" ||
	//		fieldTypeName == "double" ||
	//		fieldTypeName == "Common::Size" ||
	//		fieldTypeName == "Common::UInt64" ||
	//		fieldTypeName == "Common::Index" ||
	//		fieldTypeName == "ECS2::Entity::Id") {
	//		code.Add("static " + typeName + " " + variableName + ";");
	//		return code;
	//	}
	//	OS::NotImplemented();
	//	return {};
	//}

	//// ImGui::InputScalar("Value", ImGuiDataType_Float, {outVariable});\n
	//// ImGui::InputText("Value", {outVariable}, IM_ARRAYSIZE({outVariable}));\n
	//CodeStructure::Code GenerateImGuiInputTypeCode(const std::string& imguiVariableName, const std::string& typeName, std::string outVariable) {
	//	CodeStructure::Code code;
	//	if (typeName == "std::string") {
	//		code.Add("ImGui::InputText(\"" + imguiVariableName + "\", " + outVariable + ", IM_ARRAYSIZE(" + outVariable + "));");
	//		return code;
	//	}
	//	else if (
	//		typeName == "float" ||
	//		typeName == "double" ||
	//		typeName == "Common::Size" ||
	//		typeName == "Common::UInt64" ||
	//		typeName == "Common::Index" ||
	//		typeName == "ECS2::Entity::Id") {
	//		code.Add("ImGui::InputScalar(\"" + imguiVariableName + "\", " + GetImGuiType(typeName) + ", " + outVariable + ");");
	//		return code;
	//	}
	//	OS::NotImplemented();
	//	return code;
	//}

	//CodeStructure::Code GenerateTypeImGuiEditCode(
	//	const std::string& componentVariableName,			// position3D
	//	const std::string& fieldVariableTypeName,			// float
	//	const std::string& fieldVariableName,				// x
	//	const std::string& fieldComponentVariableName) {	// x_
	//	CodeStructure::Code code;
	//	if (fieldVariableTypeName == "std::string") {
	//		code.Add(GenerateTypeImGuiInputVariable(fieldVariableTypeName, fieldVariableName));
	//		code.NewLine();
	//		code.Add("std::memcpy(" + fieldVariableName + ", " + componentVariableName + "->" + fieldComponentVariableName + ".c_str(), " + componentVariableName + "->" + fieldVariableName + "_.size());");
	//		code.NewLine();
	//		code.Add(GenerateImGuiInputTypeCode(fieldVariableName, fieldVariableTypeName, fieldVariableName));
	//		code.NewLine();
	//		code.Add(componentVariableName + "->" + fieldComponentVariableName + " = std::string{ " + fieldVariableName + " };");
	//		code.NewLine();
	//		return code;
	//	}
	//	else if (
	//		fieldVariableTypeName == "float" ||
	//		fieldVariableTypeName == "double" ||
	//		fieldVariableTypeName == "Common::Size" ||
	//		fieldVariableTypeName == "Common::UInt64" ||
	//		fieldVariableTypeName == "Common::Index") {
	//		code.Add(GenerateImGuiInputTypeCode(
	//			fieldVariableName,
	//			fieldVariableTypeName,
	//			"&" + componentVariableName + "->" + fieldComponentVariableName));
	//		return code;
	//	}
	//	else if (fieldVariableTypeName == "ECS2::Entity::Id") {
	//		code.Add(GenerateImGuiInputTypeCode(
	//			fieldVariableName,
	//			fieldVariableTypeName,
	//			"&" + componentVariableName + "->" + fieldComponentVariableName));
	//		return code;
	//	}

	//	OS::NotImplemented();

	//	return {};
	//}



}