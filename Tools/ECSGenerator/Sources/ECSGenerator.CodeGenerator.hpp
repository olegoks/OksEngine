#pragma once

#include <ECSGenerator.Common.hpp>
#include <ECSGenerator.ProjectContext.hpp>
#include <ECSGenerator.CodeFile.hpp>


namespace ECSGenerator {


	class CodeGenerator {
	public:

		static Code GenerateInclude(const std::filesystem::path& path) {
			Code code{ "#include <" + path.string() + ">" };

			return code;
		}

		Code GenerateECSCXXFilesStructure(std::shared_ptr<Struct> structObject) {
			Code code;
			code.Add("struct " + structObject->GetName() + " ");
			if (structObject->HasParent()) {
				code.Add(": " + structObject->GetParentName() + " ");
			}
			code.Add("{");



			Code structRealization;
			{
				if (structObject->ci_.defaultConstructor_) {
					//Default constructor.
					structRealization.Add(structObject->GetName() + "() { }");
					structRealization.NewLine();
				}

				if (structObject->AreThereFields()) {


					//Fields
					structObject->ForEachField([&](const Struct::Field& field, bool isLast) {
						structRealization.Add(field.type_ + " " + field.name_ + "_;");
						structRealization.NewLine();
						});

					structRealization.NewLine();

					if (structObject->ci_.constructor_) {
						//Main constructor.
						structRealization.Add(structObject->GetName() + "(");
						structObject->ForEachField([&](const Struct::Field& field, bool isLast) {
							structRealization.Add(field.type_ + ((!field.copyable_) ? ("&& ") : (" ")) + field.name_);
							if (!isLast) {
								structRealization.Add(", ");
							}
							return true;
							});
						structRealization.Add(") : ");
						structRealization.NewLine();
						structRealization.NewLine();

						structObject->ForEachField([&](const Struct::Field& field, bool isLast) {
							structRealization.Add(field.name_ + "_{ "
								+ ((!field.copyable_) ? ("std::move(") : ("")) +
								field.name_
								+ ((!field.copyable_) ? (")") : ("")) + " }");
							if (!isLast) {
								structRealization.Add(",");
								structRealization.NewLine();
							}
							return true;
							});
						structRealization.Add("{ }");
						structRealization.NewLine();
						structRealization.NewLine();
					}



				}

				for (auto functionObject : structObject->ci_.methods_) {
					Code code;
					code.Add(GenerateECSCXXFilesStructure(functionObject));
					structRealization.Add(code);
				}
				structRealization.ApplyTab();
				code.Add(structRealization);
			}
			code.Add("};");



			return code;
		}

		Code GenerateECSCXXFilesStructure(std::shared_ptr<Namespace> namespaceObject) {
			Code code;

			code.Add("namespace " + namespaceObject->GetName() + " {");


			for (auto base : namespaceObject->base_) {
				Code namespaceCode = GenerateECSCXXFilesStructure(base);
				namespaceCode.ApplyTab();
				code.Add(namespaceCode);
			}

			code.Add("}");

			return code;
		}

		Code GenerateECSCXXFilesStructure(std::shared_ptr<Variable> variableObject) {
			Code code;
			code.Add(
				variableObject->ci_.type_ +
				" " +
				variableObject->ci_.name_ +
				"{" + variableObject->ci_.initValue_ + "};");
			return code;
		}

		Code GenerateECSCXXFilesStructure(std::shared_ptr<Function> functionObject) {
			Code code;

			if (!functionObject->ci_.templateParameters_.empty() ||
				!functionObject->ci_.specializedTemplateParameters_.empty()) {
				code.Add("template<");
				for (Common::Index i = 0; i < functionObject->ci_.templateParameters_.size(); ++i) {
					code.Add("class " + functionObject->ci_.templateParameters_[i]);
					if (i != functionObject->ci_.templateParameters_.size() - 1) {
						code.Add(", ");
					}
				}
				code.Add(">");

			}

			if (!functionObject->ci_.isPrototype_) {
				code.Add((functionObject->ci_.staticModifier_) ? ("static ") : (""));
				code.Add((functionObject->ci_.inlineModifier_) ? ("inline ") : (""));
				code.Add(functionObject->ci_.returnType_ + " " + functionObject->ci_.name_);
				if (!functionObject->ci_.specializedTemplateParameters_.empty()) {
					code.Add("<");
					for (Common::Index i = 0;
						i < functionObject->ci_.specializedTemplateParameters_.size();
						i++) {
						code.Add(functionObject->ci_.specializedTemplateParameters_[i]);
						if (i != functionObject->ci_.specializedTemplateParameters_.size() - 1) {
							code.Add(", ");
						}
					}
					code.Add(">");
				}
				code.Add("(");
				for (Common::Index i = 0; i < functionObject->ci_.parameters_.size(); i++) {
					const Function::Parameter& parameter = functionObject->ci_.parameters_[i];
					code.Add(parameter.inputType_ + " " + parameter.valueName_);
					if (i < functionObject->ci_.parameters_.size() - 1) {
						code.Add(", ");
					}
				}
				code.Add("){");

				Code realization = functionObject->ci_.code_;
				realization.ApplyTab();
				code.Add(realization);

				code.Add("};");

			}
			else {
				code.Add((functionObject->ci_.inlineModifier_) ? ("inline") : ("") + functionObject->ci_.returnType_ + " " + functionObject->ci_.name_ + "(");

				for (Common::Index i = 0; i < functionObject->ci_.parameters_.size(); i++) {
					const Function::Parameter& paramter = functionObject->ci_.parameters_[i];
					code.Add(paramter.inputType_ + " " + paramter.valueName_);
					if (i < functionObject->ci_.parameters_.size() - 1) {
						code.Add(", ");
					}

				}
				code.Add(");");

				return code;
			}
			return code;
		}

		Code GenerateECSCXXFilesStructure(std::shared_ptr<Base> base) {
			Code code;
			if (base == nullptr) {
				return code;
			}
			if (base->GetType() == Base::Type::Namespace) {
				auto namespaceObject = Common::pointer_cast<Namespace>(base);
				code = GenerateECSCXXFilesStructure(namespaceObject);

			}
			else if (base->GetType() == Base::Type::Struct) {
				auto structObject = Common::pointer_cast<Struct>(base);
				code = GenerateECSCXXFilesStructure(structObject);
			}
			else if (base->GetType() == Base::Type::Function) {
				auto functionObject = Common::pointer_cast<Function>(base);
				code = GenerateECSCXXFilesStructure(functionObject);
			}
			else if (base->GetType() == Base::Type::Variable) {
				auto functionObject = Common::pointer_cast<Variable>(base);
				code = GenerateECSCXXFilesStructure(functionObject);
			}
			clang::format::FormatStyle style = clang::format::getGoogleStyle(clang::format::FormatStyle::LanguageKind::LK_Cpp);
			llvm::StringRef codeToFormat = code.code_.c_str();  // ������ ������ ����

			// ���������� �������� ��� ����� ����
			clang::tooling::Replacements replacements = clang::format::reformat(style, codeToFormat, { clang::tooling::Range(0, codeToFormat.size()) });

			auto formattedCode = clang::tooling::applyAllReplacements(codeToFormat, replacements);
			if (!formattedCode) {
				llvm::errs() << "Error formatting the code.\n";
			}
			code.code_ = *formattedCode;
			return code;
		}

		std::shared_ptr<CodeFile> GenerateCode(std::shared_ptr<File> fileStructure) {
			return GenerateECSCXXFilesStructure(fileStructure);
		}

		std::shared_ptr<CodeFile> GenerateECSCXXFilesStructure(std::shared_ptr<File> fileStructure) {

			auto codeFile = std::make_shared<CodeFile>();

			if (fileStructure->IsHpp()) {
				Code code;
				code.Add("#pragma once");
				code.NewLine();

				codeFile->AddCode(code);
			}
			if (fileStructure->createInfo_.includes_.paths_.size() == 0) {
				//__debugbreak();
			}
			//Includes.
			{
				Code includes;
				fileStructure->ForEachInclude([&](const std::filesystem::path& path) {
					includes.Add(GenerateInclude(path));
					includes.NewLine();
					});
				includes.NewLine();
				codeFile->AddCode(includes);
			}
			const Code code = GenerateECSCXXFilesStructure(fileStructure->GetBase());
			//codeFile->path_ = fileStructure->cr
			codeFile->AddCode(code);
			return codeFile;
		}

		std::shared_ptr<CodeFile> GenerateComponentCodeFile() {


			//std::string cxxECSCode = "//THIS FILE IS AUTOGENERATED!!!\n";
	//cxxECSCode += "#pragma once\n";
	//cxxECSCode += "\n";
	//cxxECSCode += "#include <OksEngine.ECS.Component.hpp>\n";
	//cxxECSCode += "#include <imgui.h>\n";
	//cxxECSCode += "namespace OksEngine{\n\n";

				//std::string name = componentInfo.GetName();
				//std::string lowerName = componentInfo.GetLowerName();

				//cxxECSCode += GenerateComponentStructDefinition(componentInfo);

				//cxxECSCode += "\n";

				////Generate Edit function. 
				//cxxECSCode += GenerateImGuiEditFunction(componentInfo);

				////Generate Bind function. 
				//cxxECSCode += "template<>\n";
				//cxxECSCode += "inline void Bind<" + name + ">(::Lua::Context & context) {\n";
				//cxxECSCode += "\tcontext.GetGlobalNamespace()\n";
				//cxxECSCode += "\t\t.beginClass<" + name + ">(\"" + name + "\")\n";
				//for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
				//	luabridge::LuaRef field = it.value();
				//	std::string type = field["type"].cast<std::string>().value();
				//	std::string fieldName = field["name"].cast<std::string>().value();
				//	if (IsBindableType(type)) {
				//		cxxECSCode += "\t\t.addProperty (\"" + fieldName + "\", &" + name + "::" + fieldName + "_)\n";
				//	}
				//}
				//cxxECSCode += "\t\t.endClass();\n";
				//cxxECSCode += "};\n\n";

				//cxxECSCode += GenerateAddFunction(componentInfo);


				//std::make_shared<ComponentInfo>(componentInfo);
				////Namespace OksEngine end.
				//cxxECSCode += "\n};\n";
				//return cxxECSCode;
			return nullptr;
		}



	};

}