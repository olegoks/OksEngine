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
		inline CodeStructure::Code GenerateTypeImGuiInputVariable(const std::string& typeName, std::string variableName) {
			CodeStructure::Code code;
			const std::string& fieldTypeName = typeName;
			if (fieldTypeName == "std::string") {
				code.Add("static char " + variableName + "[1024] = \"\";");
				return code;
			}
			else if (
				fieldTypeName == "float" ||
				fieldTypeName == "double" ||
				fieldTypeName == "Common::Size" ||
				fieldTypeName == "Common::UInt64" ||
				fieldTypeName == "Common::UInt32" ||
				fieldTypeName == "Common::Index" ||
				fieldTypeName == "ECS2::Entity::Id" || 
				fieldTypeName == "std::vector<ECS2::Entity::Id>") {
				code.Add("static " + typeName + " " + variableName + ";");
				return code;
			}
			OS::NotImplemented();
			return {};
		}

		//// ImGui::InputScalar("Value", ImGuiDataType_Float, {outVariable});\n
		//// ImGui::InputText("Value", {outVariable}, IM_ARRAYSIZE({outVariable}));\n
		inline CodeStructure::Code GenerateImGuiInputTypeCode(const std::string& imguiVariableName, const std::string& typeName, std::string outVariable) {
			CodeStructure::Code code;
			if (typeName == "std::string") {
				code.Add("ImGui::InputText(\"" + imguiVariableName + "\", " + outVariable + ", IM_ARRAYSIZE(" + outVariable + "));");
				return code;
			}
			else if (
				typeName == "float" ||
				typeName == "double" ||
				typeName == "Common::Size" ||
				typeName == "Common::UInt64" ||
				typeName == "Common::UInt32" ||
				typeName == "Common::Index" ||
				typeName == "ECS2::Entity::Id") {
				code.Add("ImGui::InputScalar(\"" + imguiVariableName + "\", " + GetImGuiType(typeName) + ", " + outVariable + ");");
				return code;
			}
			else if (typeName == "std::vector<ECS2::Entity::Id>") {
				code.Add("static ECS2::Entity::Id refEntityId = ECS2::Entity::Id::invalid_;");
				code.Add("ImGui::InputScalar(\"refEntityId\", " + GetImGuiType("ECS2::Entity::Id") + ", &refEntityId);");
				code.Add("if(ImGui::Button(\"Add refEntity\")){{");
				code.Add("	");
				code.Add("}}");
				return code;
			}
			OS::NotImplemented();
			return code;
		}

		inline CodeStructure::Code GenerateTypeImGuiEditCode(
			const std::string& componentVariableName,			// position3D	//name
			const std::string& fieldVariableTypeName,			// float		//std::string
			const std::string& fieldVariableName,				// x			//value
			const std::string& fieldComponentVariableName) {	// x_			//value_
			CodeStructure::Code code;
			if (fieldVariableTypeName == "std::string") {
				const std::string stringArrayName = componentVariableName + '_' + fieldVariableName;
				code.Add(GenerateTypeImGuiInputVariable(fieldVariableTypeName, stringArrayName));
				code.Add("std::memcpy(" + stringArrayName + ", " + componentVariableName + "->" + fieldComponentVariableName + ".c_str(), " + componentVariableName + "->" + fieldVariableName + "_.size());");
				code.Add(GenerateImGuiInputTypeCode(stringArrayName, fieldVariableTypeName, stringArrayName));
				code.Add(componentVariableName + "->" + fieldComponentVariableName + " = std::string{ " + stringArrayName + " };");
				code.Add("std::memset({}, 0, 1024);", stringArrayName);
				return code;
			}
			else if (
				fieldVariableTypeName == "float" ||
				fieldVariableTypeName == "double" ||
				fieldVariableTypeName == "Common::Size" ||
				fieldVariableTypeName == "Common::UInt64" ||
				fieldVariableTypeName == "Common::UInt32" ||
				fieldVariableTypeName == "Common::Index") {
				code.Add(GenerateImGuiInputTypeCode(
					fieldVariableName,
					fieldVariableTypeName,
					"&" + componentVariableName + "->" + fieldComponentVariableName));
				return code;
			}
			else if (fieldVariableTypeName == "ECS2::Entity::Id") {
				code.Add("ImGui::Indent(20.f);");
				code.Add("EditEntity(ecsWorld, {}->{});", componentVariableName, fieldComponentVariableName);
				code.Add("ImGui::Unindent(20.0f);");
				return code;
			}
			else if (fieldVariableTypeName == "std::vector<ECS2::Entity::Id>") {
				code.Add("for(Common::Index i = 0; i < {}->{}.size(); i++){{", componentVariableName, fieldComponentVariableName);
				code.Add("ImGui::Indent(20.f);");
				code.Add("	EditEntity(ecsWorld, {}->{}[i]);", componentVariableName, fieldComponentVariableName);
				code.Add("ImGui::Unindent(20.0f);");
				code.Add("}");
				
				return code;
			}

			OS::NotImplemented();

			return {};
		}

		//PARSE FUNCTION
		std::shared_ptr<CodeStructure::Function> GenerateParseFunctionRealization(std::shared_ptr<ParsedComponent> component) {

			if (component->ci_.serializable_) {
				CodeStructure::Code realization;

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


				CodeStructure::Function::CreateInfo fci{
					.name_ = "Parse" + component->GetName(),
					.parameters_ = {
						{ "luabridge::LuaRef&", component->GetLowerName() + "Ref"}
					},
					.returnType_ = component->GetName(),
					.code_ = realization,
					.inlineModifier_ = true
				};

				auto parseFunuction = std::make_shared<CodeStructure::Function>(fci);

				return parseFunuction;
			}

			return nullptr;

		}
		
		std::shared_ptr<CodeStructure::Function> GenerateParseFunctionEmptyRealization(std::shared_ptr<ParsedComponent> component) {

			if (component->ci_.serializable_) {
				CodeStructure::Code realization;

				CodeStructure::Function::CreateInfo fci{
					.name_ = "Parse" + component->GetName(),
					.parameters_ = {
						{ "luabridge::LuaRef&", component->GetLowerName() + "Ref"}
					},
					.returnType_ = component->GetName(),
					.code_ = realization,
					.inlineModifier_ = false
				};

				auto parseFunuction = std::make_shared<CodeStructure::Function>(fci);

				return parseFunuction;
			}

			return nullptr;

		}

		std::shared_ptr<CodeStructure::Function> GenerateParseFunctionPrototype(std::shared_ptr<ParsedComponent> component) {

			if (component->ci_.serializable_) {

				CodeStructure::Function::CreateInfo fci{
					.name_ = "Parse" + component->GetName(),
					.parameters_ = {
						{ "luabridge::LuaRef&", component->GetLowerName() + "Ref"}
					},
					.returnType_ = component->GetName(),
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto parseFunuction = std::make_shared<CodeStructure::Function>(fci);

				return parseFunuction;
			}

			return nullptr;

		}

		//SERIALIZE FUNCTION
		std::shared_ptr<CodeStructure::Function> GenerateSerializeFunctionRealization(std::shared_ptr<ParsedComponent> component) {

			//Serialize function.
			if (component->ci_.serializable_) {

				CodeStructure::Code fieldsLuaCode;
				component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {

					CodeStructure::Code convertValueToStringCode;
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

				CodeStructure::Code luaScriptCode;
				luaScriptCode.Add("using namespace std::string_literals;");
				luaScriptCode.Add(std::format(
					"std::string luaScript = \"{} = {{"
					"{} }}\";",
					component->GetLowerName(), fieldsLuaCode.code_));

				luaScriptCode.Add("return luaScript;");

				CodeStructure::Function::CreateInfo fci{
					.name_ = "Serialize" + component->GetName(),
					.parameters_ = {
						{ "const " + component->GetName() + "*", component->GetLowerName()}
					},
					.returnType_ = "std::string",
					.code_ = luaScriptCode,
					.inlineModifier_ = true
				};

				auto serializeFunction = std::make_shared<CodeStructure::Function>(fci);

				return serializeFunction;
			}

			return nullptr;
		}

		std::shared_ptr<CodeStructure::Function> GenerateSerializeFunctionEmptyRealization(std::shared_ptr<ParsedComponent> component) {

			//Serialize function.
			if (component->ci_.serializable_) {

				CodeStructure::Code luaScriptCode;

				CodeStructure::Function::CreateInfo fci{
					.name_ = "Serialize" + component->GetName(),
					.parameters_ = {
						{ "const " + component->GetName() + "*", component->GetLowerName()}
					},
					.returnType_ = "std::string",
					.code_ = luaScriptCode,
					.inlineModifier_ = false
				};

				auto serializeFunction = std::make_shared<CodeStructure::Function>(fci);

				return serializeFunction;
			}

			return nullptr;
		}

		std::shared_ptr<CodeStructure::Function> GenerateSerializeFunctionPrototype(std::shared_ptr<ParsedComponent> component) {

			//Serialize function.
			if (component->ci_.serializable_) {

				CodeStructure::Function::CreateInfo fci{
					.name_ = "Serialize" + component->GetName(),
					.parameters_ = {
						{ "const " + component->GetName() + "*", component->GetLowerName()}
					},
					.returnType_ = "std::string",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto serializeFunction = std::make_shared<CodeStructure::Function>(fci);

				return serializeFunction;
			}

			return nullptr;
		}

		//EDIT FUNCTION
		std::shared_ptr<CodeStructure::Function> GenerateEditFunctionRealization(std::shared_ptr<ParsedComponent> component) {

			if (component->GetName() == "ScriptName") {
				Common::BreakPointLine();
			}
			CodeStructure::Code realization;
			realization.Add("ImGui::PushID({}::GetTypeId());", component->GetName());
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
			realization.Add("ImGui::PopID();");

			CodeStructure::Function::CreateInfo fci1{
				.name_ = "Edit" + component->GetName(), 
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "ecsWorld" },
					{ component->GetName() + "*", component->GetLowerName()}
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci1);

			return editFunction;

		}

		std::shared_ptr<CodeStructure::Function> GenerateEditFunctionEmptyRealization(std::shared_ptr<ParsedComponent> component) {

			if (component->GetName() == "ChildModelNodeEntities") {
				Common::BreakPointLine();
			}
			CodeStructure::Code realization;

			CodeStructure::Function::CreateInfo fci1{
				.name_ = "Edit" + component->GetName(),
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "ecsWorld" },
					{ component->GetName() + "*", component->GetLowerName()}
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = false
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci1);

			return editFunction;

		}

		std::shared_ptr<CodeStructure::Function> GenerateEditFunctionPrototype(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::Function::CreateInfo fci1{
				.name_ = "Edit" + component->GetName(),
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "ecsWorld" },
					{ component->GetName() + "*", component->GetLowerName()}
				},
				.returnType_ = "void",
				.isPrototype_ = true,
				.inlineModifier_ = false
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci1);

			return editFunction;

		}

		std::shared_ptr<CodeStructure::Function> GenerateTemplateEditFunction(std::shared_ptr<ParsedComponent> component) {

			//Template variant
			CodeStructure::Function::CreateInfo fci2{
				.name_ = "::OksEngine::Edit",//Exlicit OksEngine namespace to overload default Edit Function
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "ecsWorld" },
					{ component->GetName() + "*", component->GetLowerName()}
				},
				.returnType_ = "void",
				.code_ = std::format("Edit{}(ecsWorld,{});", component->GetName(), component->GetLowerName()),
				.inlineModifier_ = true,
				.templateParameters_ = {},
				.specializedTemplateParameters_ = { component->GetName() }
			};

			auto templateEditFunction = std::make_shared<CodeStructure::Function>(fci2);

			return templateEditFunction;

		}

		std::shared_ptr<CodeStructure::Function> GenerateTemplateEditFunctionWithNamespace(
			std::vector<std::string> currentNamespace, std::shared_ptr<ParsedComponent> component) {

			std::string forFunctionName;
			std::string forTypeName;
			for (Common::Index i = 0; i < currentNamespace.size(); i++) {
				forFunctionName += currentNamespace[i];
				forFunctionName += "_";

				forTypeName += currentNamespace[i];
				forTypeName += "::";
			}
			//Template variant
			CodeStructure::Function::CreateInfo fci2{
				.name_ = "Edit",//Exlicit OksEngine namespace to overload default Edit Function
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "ecsWorld" },
					{ forTypeName + component->GetName() + "*", component->GetLowerName()}
				},
				.returnType_ = "void",
				.code_ = std::format("{}Edit{}(ecsWorld,{});", forTypeName, component->GetName(), component->GetLowerName()),
				.inlineModifier_ = true,
				.templateParameters_ = {},
				.specializedTemplateParameters_ = { forTypeName + component->GetName() }
			};

			auto templateEditFunction = std::make_shared<CodeStructure::Function>(fci2);

			return templateEditFunction;

		}

		//BIND FUNCTION
		std::shared_ptr<CodeStructure::Function> GenerateBindFunctionRealization(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::Code realization;
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

			CodeStructure::Function::CreateInfo fci{
				.name_ = "Bind" + component->GetName(),
				.parameters_ = {
					{ "::Lua::Context&", "context" }
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci);

			return editFunction;

		}

		std::shared_ptr<CodeStructure::Function> GenerateBindFunctionEmptyRealization(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::Code realization;
			CodeStructure::Function::CreateInfo fci{
				.name_ = "Bind" + component->GetName(),
				.parameters_ = {
					{ "::Lua::Context&", "context" }
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = false
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci);

			return editFunction;

		}


		std::shared_ptr<CodeStructure::Function> GenerateBindFunctionPrototype(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::Function::CreateInfo fci{
				.name_ = "Bind" + component->GetName(),
				.parameters_ = {
					{ "::Lua::Context&", "context" }
				},
				.returnType_ = "void",
				.isPrototype_ = true,
				.inlineModifier_ = false
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci);

			return editFunction;

		}

		//ADD FUNCTION
		std::shared_ptr<CodeStructure::Function> GenerateAddFunctionRealization(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::Code realization;
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
						CodeStructure::Code addRealization =
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
					realization.Add("if (!ecsWorld->IsComponentExist<" + component->GetName() + ">(entityId)) {");
					realization.NewLine();

					realization.Add("ecsWorld->CreateComponent<" + component->GetName() + ">(entityId");
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
			CodeStructure::Function::CreateInfo fci{
				.name_ = "Add" + component->GetName(),
				.parameters_ = {
					{ "ECS2::World*", "ecsWorld" },
					{ "ECS2::Entity::Id", "entityId" }
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci);

			return editFunction;
		}

		std::shared_ptr<CodeStructure::Function> GenerateAddFunctionEmptyRealization(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::Code realization;

			CodeStructure::Function::CreateInfo fci{
				.name_ = "Add" + component->GetName(),
				.parameters_ = {
					{ "ECS2::World*", "ecsWorld" },
					{ "ECS2::Entity::Id", "entityId" }
				},
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = false
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci);

			return editFunction;
		}

		std::shared_ptr<CodeStructure::Function> GenerateAddFunctionPrototype(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::Function::CreateInfo fci{
				.name_ = "Add" + component->GetName(),
				.parameters_ = {
					{ "ECS2::World*", "ecsWorld" },
					{ "ECS2::Entity::Id", "entityId" }
				},
				.returnType_ = "void",
				.isPrototype_ = true,
				.inlineModifier_ = false
			};

			auto editFunction = std::make_shared<CodeStructure::Function>(fci);

			return editFunction;
		}


		std::shared_ptr<CodeStructure::Struct> GenerateComponentStruct(std::shared_ptr<ParsedComponent> component) {

			if (component->GetName() == "ECSMenu") {
				Common::BreakPointLine();
			}
			//GetTypeId function

			//HACK

			static Common::Index id = 0;

			//HACK
			CodeStructure::Function::CreateInfo tidfci{
				.name_ = "GetTypeId",
				.parameters_ = {},
				.returnType_ = "ECS2::ComponentTypeId",
				.code_ = "return " + std::to_string(++id) + ";",
				.inlineModifier_ = true,
				.staticModifier_ = true
			};

			auto getTypeIdMethod = std::make_shared<CodeStructure::Function>(tidfci);

			//GetName  function
			std::vector<CodeStructure::Struct::Field> fields;

			component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast)->bool {

				CodeStructure::Struct::Field field{
					.type_ = fieldInfo.GetTypeName(),
					.name_ = fieldInfo.GetName(),
					.alignment_ = fieldInfo.alignment_,
					.copyable_ = fieldInfo.copyable_,
					.defaultValue_ = fieldInfo.defaultValue_
				};

				fields.push_back(field);

				return true;
				});

			CodeStructure::Function::CreateInfo fci{
				.name_ = "GetName",
				.parameters_ = {},
				.returnType_ = "const char*",
				.code_ = "return \"" + component->GetFullName() + "\";",
				.inlineModifier_ = false,
				.staticModifier_ = true
			};

			auto getNameMethod = std::make_shared<CodeStructure::Function>(fci);

			CodeStructure::Struct::CreateInfo sci{
				.name_ = component->GetName(),
				//.parent_ = "OksEngine::IComponent<" + component->GetName() + ">",
				.alignment_ = component->ci_.alignment_,
				.fields_ = fields,
				.methods_ = { getNameMethod, getTypeIdMethod }
			};
			auto structObject = std::make_shared<CodeStructure::Struct>(sci);

			return structObject;
		}

		CodeStructure::File::Includes GenerateIncludes(std::shared_ptr<ParsedComponent> component) {

			CodeStructure::File::Includes includes{
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