#pragma once 

#include <Component/ECSGenerator2.ParsedComponent.hpp>

namespace ECSGenerator2 {


	class ComponentCodeStructureGenerator {
	public:

		struct CreateInfo {
			std::filesystem::path includeDirectory_;
		};

		ComponentCodeStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		//static char {variableName}[256] = "";
		//static Common::UInt64 {variableName};
		Code GenerateTypeImGuiInputVariable(const std::string& typeName, std::string variableName) {
			Code code;
			const std::string& fieldTypeName = typeName;
			if (fieldTypeName == "std::string") {
				code.Add("static char " + variableName + "[4096] = \"\";");
				return code;
			}
			else if (
				fieldTypeName == "float" ||
				fieldTypeName == "double" ||
				fieldTypeName == "Common::Size" ||
				fieldTypeName == "Common::UInt64" ||
				fieldTypeName == "Common::Index" ||
				fieldTypeName == "ECS2::Entity::Id") {
				code.Add("static " + typeName + " " + variableName + ";");
				return code;
			}
			OS::NotImplemented();
			return {};
		}

		//// ImGui::InputScalar("Value", ImGuiDataType_Float, {outVariable});\n
		//// ImGui::InputText("Value", {outVariable}, IM_ARRAYSIZE({outVariable}));\n
		Code GenerateImGuiInputTypeCode(const std::string& imguiVariableName, const std::string& typeName, std::string outVariable) {
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
				typeName == "ECS2::Entity::Id") {
				code.Add("ImGui::InputScalar(\"" + imguiVariableName + "\", " + GetImGuiType(typeName) + ", " + outVariable + ");");
				return code;
			}
			OS::NotImplemented();
			return code;
		}

		Code GenerateTypeImGuiEditCode(
			const std::string& componentVariableName,			
			const std::string& fieldVariableTypeName,			
			const std::string& fieldVariableName,				
			const std::string& fieldComponentVariableName) {	// x_
			Code code;
			if (fieldVariableTypeName == "std::string") {
				code.Add(GenerateTypeImGuiInputVariable(fieldVariableTypeName, fieldVariableName));
				code.NewLine();
				code.Add("std::memcpy(" + fieldVariableName + ", " + componentVariableName + "->" + fieldComponentVariableName + ".c_str(), " + componentVariableName + "->" + fieldVariableName + "_.size());");
				code.NewLine();
				code.Add(GenerateImGuiInputTypeCode(fieldVariableName, fieldVariableTypeName, fieldVariableName));
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
				fieldVariableTypeName == "Common::Index") {
				code.Add(GenerateImGuiInputTypeCode(
					fieldVariableName,
					fieldVariableTypeName,
					"&" + componentVariableName + "->" + fieldComponentVariableName));
				return code;
			}
			else if (fieldVariableTypeName == "ECS2::Entity::Id") {
				code.Add(GenerateImGuiInputTypeCode(
					fieldVariableName,
					fieldVariableTypeName,
					"&" + componentVariableName + "->" + fieldComponentVariableName));
				return code;
			}

			OS::NotImplemented();

			return {};
		}

		std::shared_ptr<Function> GenerateParseFunction(std::shared_ptr<ParsedComponent> component) {

			if (component->ci_.serializable_) {
				Code realization;

				realization.Add(std::format("{} {};",
					component->GetName(),
					component->GetLowerName()));

				component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {

					realization.Add(std::format("{}.{}_ = {}Ref[\"{}\"].cast<{}>().value();",
						component->GetLowerName(),
						fieldInfo.GetName(),
						component->GetLowerName(),
						fieldInfo.GetName(),


						(fieldInfo.GetTypeName() != "ECS2::Entity::Id") ? (fieldInfo.GetTypeName()) : ("Common::Index")));

					return true;
					});

				realization.Add(std::format("return {};", component->GetLowerName()));


				Function::CreateInfo fci{
					.name_ = "Parse" + component->GetName(),
					.parameters_ = {
						{ "luabridge::LuaRef&", component->GetLowerName() + "Ref"}
					},
					.returnType_ = component->GetName(),
					.code_ = realization,
					.inlineModifier_ = true
				};

				auto parseFunuction = std::make_shared<Function>(fci);

				return parseFunuction;
			}

			return nullptr;

		}

		std::shared_ptr<Function> GenerateSerializeFunction(std::shared_ptr<ParsedComponent> component) {

			//Serialize function.
			if (component->ci_.serializable_) {

				Code fieldsLuaCode;
				component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {

					Code convertValueToStringCode;
					if (fieldInfo.GetTypeName() == "std::string") {
						convertValueToStringCode.Add(std::format("\"\\\"\"s + {}->{}_ + \"\\\"\"",
							component->GetLowerName(),
							fieldInfo.GetName()));
					}
					else {
						convertValueToStringCode.Add(std::format("std::to_string({}->{}_)",
							component->GetLowerName(),
							fieldInfo.GetName()));
					}


					fieldsLuaCode.Add(std::format("{} =\" + {} + \"",
						fieldInfo.GetName(),
						convertValueToStringCode.code_));
					if (!isLast)
					{
						fieldsLuaCode.Add(",");
					}
					return true;
					});

				Code luaScriptCode;
				luaScriptCode.Add("using namespace std::string_literals;");
				luaScriptCode.Add(std::format(
					"std::string luaScript = \"{} = {{"
					"{} }}\";",
					component->GetLowerName(), fieldsLuaCode.code_));

				luaScriptCode.Add("return luaScript;");

				Function::CreateInfo fci{
					.name_ = "Serialize" + component->GetName(),
					.parameters_ = {
						{ "const " + component->GetName() + "*", component->GetLowerName()}
					},
					.returnType_ = "std::string",
					.code_ = luaScriptCode,
					.inlineModifier_ = true
				};

				auto serializeFunction = std::make_shared<Function>(fci);

				return serializeFunction;
			}

			return nullptr;
		}

		std::shared_ptr<Function> GenerateEditFunction(std::shared_ptr<ParsedComponent> component) {


			Code realization;
			component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {
				if (IsTypeCanBeEnteredFromImGui(fieldInfo.GetTypeName())) {
					realization.Add(GenerateTypeImGuiEditCode(
						component->GetLowerName(),
						fieldInfo.GetTypeName(),
						fieldInfo.GetName(),
						fieldInfo.GetName() + "_")
						.NewLine());
				}
				return true;
				});


			Function::CreateInfo fci1{
				.name_ = "Edit" + component->GetName(),
				.parameters_ = {
					{ component->GetName() + "*", component->GetLowerName()}
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true
			};

			auto editFunction = std::make_shared<Function>(fci1);

			return editFunction;

		}

		std::shared_ptr<Function> GenerateTemplateEditFunction(std::shared_ptr<ParsedComponent> component) {

			//Template variant
			Function::CreateInfo fci2{
				.name_ = "Edit",
				.parameters_ = {
					{ component->GetName() + "*", component->GetLowerName()}
				},
				.returnType_ = "void",
				.code_ = std::format("Edit{}({});", component->GetName(), component->GetLowerName()),
				.inlineModifier_ = true,
				.templateParameters_ = {},
				.specializedTemplateParameters_ = { component->GetName() }
			};

			auto templateEditFunction = std::make_shared<Function>(fci2);

			return templateEditFunction;

		}

		std::shared_ptr<Function> GenerateBindFunction(std::shared_ptr<ParsedComponent> component) {

			Code realization;
			realization.Add("context.GetGlobalNamespace()");
			realization.NewLine();
			realization.Add("\t.beginClass<" + component->GetName() + ">(\"" + component->GetName() + "\")");
			realization.NewLine();
			component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {
				if (IsBindableType(fieldInfo.GetTypeName())) {
					realization.Add("\t.addProperty (\"" + fieldInfo.GetName() + "\", &" + component->GetName() + "::" + fieldInfo.GetName() + "_)");
					realization.NewLine();
				}
				return true;
				});
			realization.Add("\t.endClass();");
			realization.NewLine();

			Function::CreateInfo fci{
				.name_ = "Bind" + component->GetName(),
				.parameters_ = {
					{ "::Lua::Context&", "context" }
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true
			};

			auto editFunction = std::make_shared<Function>(fci);

			return editFunction;

		}

		std::shared_ptr<Function> GenerateAddFunction(std::shared_ptr<ParsedComponent> component) {

			Code realization;
			if (component->CanBeCreatedFromImGui()) {
				component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {
					realization.Add(GenerateTypeImGuiInputVariable(
						fieldInfo.GetTypeName(),
						fieldInfo.GetName())
						.NewLine());
					return true;
					});
			}
			realization.NewLine();
			realization.Add("if (ImGui::CollapsingHeader(\"Create info\")) {");
			{
				realization.NewLine();

				if (component->CanBeCreatedFromImGui()) {
					component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {
						std::string variableName;
						if (!(fieldInfo.GetTypeName() == "std::string")) {
							variableName += "&"; // If string we dont need &, because array id is pointer.
						}
						variableName += fieldInfo.GetName();
						Code addRealization =
							GenerateImGuiInputTypeCode(
								fieldInfo.GetName(),
								fieldInfo.GetTypeName(),
								variableName);
						addRealization.ApplyTab();
						realization.Add(addRealization);
						realization.NewLine();
						return true;
						});
				}


				realization.Add("ImGui::Spacing();");

				realization.NewLine();
				realization.Add("\tif (ImGui::Button(\"Add component\")) {");
				{
					realization.NewLine();
					realization.Add("if (!world->IsComponentExist<" + component->GetName() + ">(entityId)) {");
					realization.NewLine();

					realization.Add("world->CreateComponent<" + component->GetName() + ">(entityId");
					if (component->CanBeCreatedFromImGui()) {
						if (component->AreThereFields()) {
							realization.Add(", ");
						}
						component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {
							realization.Add(fieldInfo.GetName());
							if (!isLast) {
								realization.Add(", ");
							}
							return true;
							});
					}
					realization.Add(");");
					realization.NewLine();


					realization.Add("}");
				}
				realization.NewLine();
				realization.NewLine();
				realization.Add("}");
			}
			realization.NewLine();
			realization.Add("}");
			Function::CreateInfo fci{
				.name_ = "Add" + component->GetName(),
				.parameters_ = {
					{ "ECS2::World*", "world" },
					{ "ECS2::Entity::Id", "entityId" }
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true
			};

			auto editFunction = std::make_shared<Function>(fci);

			return editFunction;
		}

		std::shared_ptr<Struct> GenerateComponentStruct(std::shared_ptr<ParsedComponent> component) {

			if (component->GetName() == "ECSMenu") {
				Common::BreakPointLine();
			}
			std::vector<Struct::Field> fields;

			component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast)->bool {

				Struct::Field field{
					.type_ = fieldInfo.GetTypeName(),
					.name_ = fieldInfo.GetName(),
					.copyable_ = fieldInfo.copyable_
				};

				fields.push_back(field);

				return true;
				});

			Function::CreateInfo fci{
				.name_ = "GetName",
				.parameters_ = {},
				.returnType_ = "const char*",
				.code_ = "return Common::TypeInfo<" + component->GetName() + ">::GetTypeName();",
				.inlineModifier_ = false,
				.staticModifier_ = true
			};

			auto getNameMethod = std::make_shared<Function>(fci);

			Struct::CreateInfo sci{
				.name_ = component->GetName(),
				.parent_ = "OksEngine::IComponent<" + component->GetName() + ">",
				.fields_ = fields,
				.methods_ = { getNameMethod }
			};
			auto structObject = std::make_shared<Struct>(sci);

			return structObject;
		}

		File::Includes GenerateIncludes(std::shared_ptr<ParsedComponent> component) {

			File::Includes includes{
				{	"imgui.h",
					"ECS2.World.hpp",
					"ECS2.Entity.hpp",
					"OksEngine.IComponent.hpp",
					"Lua.Context.hpp" }
			};

			component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {

				if (fieldInfo.GetTypeName() == "std::string") {
					includes.paths_.insert("string");
				}
				if (fieldInfo.GetTypeName().find("Common") != std::string::npos) {
					includes.paths_.insert("Common.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::chrono") != std::string::npos) {
					includes.paths_.insert("chrono");
				}
				if (fieldInfo.GetTypeName().find("PAL") != std::string::npos) {
					includes.paths_.insert("PAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm") != std::string::npos) {
					includes.paths_.insert("glm/glm.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::queue") != std::string::npos) {
					includes.paths_.insert("queue");
				}
				if (fieldInfo.GetTypeName().find("std::vector") != std::string::npos) {
					includes.paths_.insert("vector");
				}
				if (fieldInfo.GetTypeName().find("std::unordered_set") != std::string::npos) {
					includes.paths_.insert("unordered_set");
				}
				if (fieldInfo.GetTypeName().find("RAL::") != std::string::npos) {
					includes.paths_.insert("RAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("Geom:") != std::string::npos) {
					includes.paths_.insert("Geometry.hpp");
				}
				if (fieldInfo.GetTypeName().find("UIAL::Window") != std::string::npos) {
					includes.paths_.insert("UIAL.Window.hpp");
				}
				if (fieldInfo.GetTypeName().find("UI::Window") != std::string::npos) {
					includes.paths_.insert("UI.Window.hpp");
				}
				if (fieldInfo.GetTypeName().find("sf::") != std::string::npos) {
					includes.paths_.insert("SFML/Audio.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm::quat") != std::string::npos) {
					includes.paths_.insert("glm/gtc/quaternion.hpp");
				}
				if (fieldInfo.GetTypeName().find("PE") != std::string::npos) {
					includes.paths_.insert("PE.PhysicsEngine.hpp");
				}
				if (fieldInfo.GetTypeName().find("AsyncResourceSubsystem") != std::string::npos) {
					includes.paths_.insert("Resources/OksEngine.Resource.Subsystem.hpp");
				}
				if (fieldInfo.GetTypeName().find("ax::NodeEditor::") != std::string::npos) {
					includes.paths_.insert("imgui.h");
					includes.paths_.insert("imgui-node-editor/imgui_node_editor.h");
				}

				return true;
				});

			return includes;
		}

		CreateInfo ci_;
	};

}