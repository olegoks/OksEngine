#pragma once 

#include <ECSGenerator.Common.hpp>

#include <ECSGenerator.ParsedEcsFile.hpp>


namespace ECSGenerator {

	class ParsedComponentECSFile : public ParsedECSFile {
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
			std::filesystem::path path_;
			std::vector<FieldInfo> fields_;
		};

		ParsedComponentECSFile(const CreateInfo& createInfo)
			: ParsedECSFile{ createInfo.path_, createInfo.name_ },
			fields_{ createInfo.fields_ } { }


		std::vector<FieldInfo> fields_;

		bool AreThereFields() const {
			return !fields_.empty();
		}

		using ProcessField = std::function<bool(const FieldInfo& fieldInfo, bool isLast)>;

		void ForEachField(ProcessField&& processField) const {
			for (Common::Index i = 0; i < fields_.size(); i++) {
				const FieldInfo info = fields_[i];
				if (!processField(info, (i == fields_.size() - 1))) {
					break;
				}
			}
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
		std::string GetLowerName() {
			return std::string{ static_cast<char>(std::tolower(GetName()[0])) } + GetName().substr(1);
		}

		Type GetType() const noexcept override {
			return Type::Component;
		}

	};


	inline std::shared_ptr<ParsedComponentECSFile> ParseComponentEcsFile(const std::filesystem::path& path, ::Lua::Context& context) {

		luabridge::LuaRef component = context.GetGlobalAsRef("component");
		luabridge::LuaRef fields = component["fields"];

		std::vector<ParsedComponentECSFile::FieldInfo> parsedFields;

		for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
			luabridge::LuaRef field = it.value();
			ParsedComponentECSFile::FieldInfo fieldInfo{
				.typeName_ = field["type"].cast<std::string>().value(),
				.name_ = field["name"].cast<std::string>().value()
			};

			if (!field["copyable"].isNil()) {
				fieldInfo.copyable_ = field["copyable"].cast<bool>().value();
			}

			parsedFields.push_back(fieldInfo);
		}

		ParsedComponentECSFile::CreateInfo ci{
			.name_ = component["name"].cast<std::string>().value(),
			.path_ = path,
			.fields_ = parsedFields
		};
		auto parsedComponentFile = std::make_shared<ParsedComponentECSFile>(ci);
		OS::AssertMessage(std::isupper(parsedComponentFile->GetName()[0]), "First component name symbol must be uppercase.");
		return parsedComponentFile;
	}


	//static char {variableName}[256] = "";
	//static Common::UInt64 {variableName};
	Code GenerateTypeImGuiInputVariable(const std::string& typeName, std::string variableName) {
		Code code;
		const std::string& fieldTypeName = typeName;
		if (fieldTypeName == "std::string") {
			code.Add("static char " + variableName + "[256] = \"\";");
			return code;
		}
		else if (
			fieldTypeName == "float" || 
			fieldTypeName == "double" || 
			fieldTypeName == "Common::Size" ||
			fieldTypeName == "Common::UInt64" || 
			fieldTypeName == "Common::Index" || 
			fieldTypeName == "ECS::Entity::Id") {
			code.Add("static " + typeName + " " + variableName + ";");
			return code;
		}
		OS::NotImplemented();
		return {};
	}

	//// ImGui::InputScalar("Value", ImGuiDataType_Float, {outVariable});\n
	//// ImGui::InputText("Value", {outVariable}, IM_ARRAYSIZE({outVariable}));\n
	Code GenerateImGuiInputTypeCode(const std::string& imguiVariableName, const std::string& typeName,  std::string outVariable) {
		Code code;
		if (typeName == "std::string") {
			code.Add("ImGui::InputText(\"" + imguiVariableName + "\", " + outVariable + ", IM_ARRAYSIZE(" + outVariable + "));");
			return code;
		}
		else if (
			typeName == "float" ||
			typeName == "double" ||
			typeName == "Common::Size" ||
			typeName == "Common::UInt64" ||
			typeName == "Common::Index" ||
			typeName == "ECS::Entity::Id") {
			code.Add("ImGui::InputScalar(\"" + imguiVariableName + "\", " + GetImGuiType(typeName) + ", " + outVariable + ");");
			return code;
		}
		OS::NotImplemented();
		return code;
	}

	Code GenerateTypeImGuiEditCode(
		const std::string& componentVariableName,			// position3D
		const std::string& fieldVariableTypeName,			// float
		const std::string& fieldVariableName,				// x
		const std::string& fieldComponentVariableName) {	// x_
		Code code;
		if (fieldVariableTypeName == "std::string") {
			code.Add(GenerateTypeImGuiInputVariable(fieldVariableTypeName, fieldVariableName));
			code.NewLine();
			code.Add("std::memcpy(" + fieldVariableName + ", " + componentVariableName + "->" + fieldComponentVariableName + ".c_str(), " + componentVariableName + "->" + fieldVariableName + "_.size());");
			code.NewLine();
			code.Add(GenerateImGuiInputTypeCode(fieldVariableTypeName, fieldVariableTypeName, fieldVariableName));
			code.NewLine();
			code.Add(componentVariableName + "->" + fieldComponentVariableName + " = std::string{ " + fieldVariableName + " };");
			code.NewLine();
			return code;
		}
		else if (
			fieldVariableTypeName == "float" || 
			fieldVariableTypeName == "double" || 
			fieldVariableTypeName == "Common::Size" || 
			fieldVariableTypeName == "Common::UInt64" || 
			fieldVariableTypeName == "Common::Index" || 
			fieldVariableTypeName == "ECS::Entity::Id") {
			code.Add(GenerateImGuiInputTypeCode(
				fieldVariableName,
				fieldVariableTypeName,
				"&" + componentVariableName + "->" + fieldComponentVariableName));
			return code;
		}

		OS::NotImplemented();

		return {};
	}

	//std::string GenerateImGuiEditFunction(const ComponentInfo& component) {

	//	std::string code = "\n";
	//	code += "template<>\n";
	//	code += "inline void Edit<" + component.GetName() + ">(" + component.GetName() + "* " + component.GetLowerName() + ") {\n";
	//	component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
	//		if (IsTypeCanBeEnteredFromImGui(field.GetTypeName())) {
	//			code += GenerateTypeImGuiEditCode(component, field);
	//		}
	//		return true;
	//		});

	//	code += "};\n\n";


	//	return code;
	//}


	//std::string GenerateComponentStructDefinition(const ComponentInfo& component) {
	//	std::string code = "\n";
	//	code += "struct " + component.GetName() + ": public ECSComponent<" + component.GetName() + "> " + "{\n";

	//	//Default constructor.
	//	code += "\t" + component.GetName() + "()\n\t : ECSComponent{ nullptr } {}\n";

	//	//Main constructor.
	//	if (component.AreThereFields()) {
	//		//Main constructor.
	//		code += "\t" + component.GetName() + "(";
	//		component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
	//			code += field.GetTypeName() + " " + field.GetName();
	//			if (!isLast) {
	//				code += ", ";
	//			}
	//			return true;
	//			});
	//		code += ") : \n";
	//		code += "\t\tECSComponent{ nullptr }, \n";

	//		component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
	//			code += "\t\t" + field.GetName() + "_{ " + field.GetName() + " }";
	//			if (!isLast) {
	//				code += ",\n";
	//			}
	//			return true;
	//			});
	//		code += "{ }\n";

	//		/*[[nodiscard]]
	//		static const char* GetName() noexcept {
	//			return Common::TypeInfo<Type>::GetTypeName();
	//		}*/

	//		code += "\n";

	//		code += "\t\tstatic const char* GetName() noexcept {\n"
	//			"\t\t\treturn Common::TypeInfo<" + component.GetName() + ">::GetTypeName();\n"
	//			"\t\t}\n";

	//		/*[[nodiscard]]
	//		static std::size_t GetSize() noexcept { return sizeof(Type); }*/

	//		code += "\t\t[[nodiscard]]\n"
	//			"\t\tstatic std::size_t GetSize() noexcept { return sizeof(" + component.GetName() + "); }\n";

	//	}

	//	component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
	//		code += "\t" + field.GetTypeName() + " " + field.GetName() + "_;\n";
	//		return true;
	//		});


	//	code += "};\n";
	//	return code;
	//}


	//std::string GenerateAddFunction(const ComponentInfo& component) {
	//	//Generate Add function. 
	//	std::string code;
	//	code += "template<>\n";
	//	code += "inline void Add<" + component.GetName() + ">(ECS::World * world, ECS::Entity::Id entityId) {\n";
	//	code += "\n";
	//	if (component.CanBeCreatedFromImGui()) {
	//		component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
	//			code += "\t" + GenerateTypeImGuiInputVariable(component, field, field.GetName()) + "\n";
	//			return true;
	//			});
	//	}
	//	code += "\tif (ImGui::CollapsingHeader(\"Create info\")) {\n";

	//	if (component.CanBeCreatedFromImGui()) {
	//		component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
	//			std::string variableName;
	//			if (!(field.GetTypeName() == "std::string")) {
	//				variableName += "&"; // If string we dont need &, because array id is pointer.
	//			}
	//			variableName += field.GetName();
	//			code += "\t\t" + GenerateImGuiInputTypeCode(component, field, variableName) + "\n";
	//			return true;
	//			});
	//	}
	//	code += "\t\tImGui::Spacing();\n";
	//	code += "\t}\n";
	//	code += "\tif (ImGui::Button(\"Add component\")) {\n";
	//	code += "\t\tif (!world->IsComponentExist<" + component.GetName() + ">(entityId)) {\n";
	//	code += "\t\t\tworld->CreateComponent<" + component.GetName() + ">(entityId";
	//	if (component.CanBeCreatedFromImGui()) {
	//		if (component.AreThereFields()) {
	//			code += ", ";
	//		}
	//		component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
	//			code += field.GetName();
	//			if (!isLast) {
	//				code += ", ";
	//			}
	//			return true;
	//			});
	//	}
	//	code += ");\n";

	//	code += "\t\t}\n";
	//	code += "\t}\n";
	//	code += "};\n\n";

	//	return code;
	//}




}