#pragma once

#include <ECSGenerator2.Common.hpp>
#include <ECSGenerator2.CodeFile.hpp>

#include <clang/Format/Format.h>

namespace ECSGenerator2 {


	class CodeGenerator {
	public:

		static CodeStructure::Code GenerateInclude(const std::filesystem::path& path) {
			CodeStructure::Code code{ "#include <" + path.string() + ">" };

			return code;
		}

		CodeStructure::Code GenerateECSCXXFilesStructure(std::shared_ptr<CodeStructure::Struct> structObject) {
			CodeStructure::Code code;

			if (structObject->ci_.forwardDeclaration_) {
				code.Add("struct "); 
				code.Add(structObject->GetName() + ";");
				return code;
			}

			code.Add("struct ");
			if (structObject->ci_.alignment_ != Common::Limits<Common::Size>::Max()) {
				code.Add(" alignas({})  ", structObject->ci_.alignment_);
			}
			code.Add(structObject->GetName() + " ");

			if (structObject->HasParent()) {
				code.Add(": " + structObject->GetParentName() + " ");
			}
			code.Add("{");

			CodeStructure::Code structRealization;
			{
				if (structObject->ci_.defaultConstructor_) {
					//Default constructor.
					structRealization.Add(structObject->GetName() + "() { }");
					structRealization.NewLine();
				}

				if (structObject->AreThereFields()) {


					//Fields
					structObject->ForEachField([&](const CodeStructure::Struct::Field& field, bool isLast) {

						if (field.alignment_ != Common::Limits<Common::Size>::Max()) {
							structRealization.Add("alignas({})", field.alignment_);
						}

						structRealization.Add(field.type_ + " " + field.name_ + "_;");
						structRealization.NewLine();
						});

					structRealization.NewLine();

					if (structObject->ci_.constructor_) {
						//Main constructor.
						structRealization.Add(structObject->GetName() + "(");
						structObject->ForEachField([&](const CodeStructure::Struct::Field& field, bool isLast) {
							structRealization.Add(field.type_ + ((!field.copyable_) ? ("&& ") : (" ")) + field.name_);
							if (!isLast) {
								structRealization.Add(", ");
							}
							return true;
							});
						structRealization.Add(") : ");
						structRealization.NewLine();
						structRealization.NewLine();

						structObject->ForEachField([&](const CodeStructure::Struct::Field& field, bool isLast) {
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
					CodeStructure::Code code;
					code.Add(GenerateECSCXXFilesStructure(functionObject));
					structRealization.Add(code);
				}
				structRealization.ApplyTab();
				code.Add(structRealization);
			}
			code.Add("};");



			return code;
		}

		CodeStructure::Code GenerateECSCXXFilesStructure(std::shared_ptr<CodeStructure::Namespace> namespaceObject) {
			CodeStructure::Code code;

			code.Add("namespace " + namespaceObject->GetName() + " {");


			for (Common::Index i = 0; i < namespaceObject->base_.size(); i++) {
				auto base = namespaceObject->base_[i];
				CodeStructure::Code namespaceCode = GenerateECSCXXFilesStructure(base);
				namespaceCode.ApplyTab();
				namespaceCode.NewLine();
				namespaceCode.NewLine();
				code.Add(namespaceCode);
				if (i != namespaceObject->base_.size() - 1) {
					namespaceCode.NewLine();
					namespaceCode.NewLine();
				}
			}

			code.Add("}");

			return code;
		}

		CodeStructure::Code GenerateECSCXXFilesStructure(std::shared_ptr<CodeStructure::Variable> variableObject) {
			CodeStructure::Code code;
			code.Add(
				variableObject->ci_.type_ +
				" " +
				variableObject->ci_.name_ +
				"{" + variableObject->ci_.initValue_ + "};");
			return code;
		}

		CodeStructure::Code GenerateECSCXXFilesStructure(std::shared_ptr<CodeStructure::CodeBlock> codeBlockObject) {
			CodeStructure::Code code;
			code.Add(codeBlockObject->code_);
			return code;
		}

		CodeStructure::Code GenerateECSCXXFilesStructure(std::shared_ptr<CodeStructure::Function> functionObject) {
			CodeStructure::Code code;

			if (functionObject->ci_.descriptionComment_ != "") {
				code.Add("//" + functionObject->ci_.descriptionComment_ + "\n");
			}

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
					const CodeStructure::Function::Parameter& parameter = functionObject->ci_.parameters_[i];
					code.Add(parameter.inputType_ + " " + parameter.valueName_);
					if (i < functionObject->ci_.parameters_.size() - 1) {
						code.Add(", ");
					}
				}
				code.Add("){");

				CodeStructure::Code realization = functionObject->ci_.code_;
				realization.ApplyTab();
				code.Add(realization);

				code.Add("};");

			}
			else {
				code.Add((functionObject->ci_.inlineModifier_) ? ("inline") : ("") + functionObject->ci_.returnType_ + " " + functionObject->ci_.name_ + "(");

				for (Common::Index i = 0; i < functionObject->ci_.parameters_.size(); i++) {
					const CodeStructure::Function::Parameter& paramter = functionObject->ci_.parameters_[i];
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

		CodeStructure::Code GenerateECSCXXFilesStructure(std::shared_ptr<CodeStructure::Base> base) {
			CodeStructure::Code code;
			if (base == nullptr) {
				return code;
			}
			if (base->GetType() == CodeStructure::Base::Type::Namespace) {
				auto namespaceObject = std::dynamic_pointer_cast<CodeStructure::Namespace>(base);
				code = GenerateECSCXXFilesStructure(namespaceObject);

			}
			else if (base->GetType() == CodeStructure::Base::Type::Struct) {
				auto structObject = std::dynamic_pointer_cast<CodeStructure::Struct>(base);
				code = GenerateECSCXXFilesStructure(structObject);
			}
			else if (base->GetType() == CodeStructure::Base::Type::Function) {
				auto functionObject = std::dynamic_pointer_cast<CodeStructure::Function>(base);
				code = GenerateECSCXXFilesStructure(functionObject);
			}
			else if (base->GetType() == CodeStructure::Base::Type::Variable) {
				auto functionObject = std::dynamic_pointer_cast<CodeStructure::Variable>(base);
				code = GenerateECSCXXFilesStructure(functionObject);
			}
			else if (base->GetType() == CodeStructure::Base::Type::CodeBlock) {
				auto functionObject = std::dynamic_pointer_cast<CodeStructure::CodeBlock>(base);
				code = GenerateECSCXXFilesStructure(functionObject);
			}

			return code;
		}

		std::shared_ptr<CodeStructure::CodeFile> GenerateCode(std::shared_ptr<CodeStructure::File> fileStructure) {
			return GenerateECSCXXFilesStructure(fileStructure);
		}

		std::shared_ptr<CodeStructure::CodeFile> GenerateECSCXXFilesStructure(std::shared_ptr<CodeStructure::File> fileStructure) {

			auto codeFile = std::make_shared<CodeStructure::CodeFile>();

			if (fileStructure->IsHpp()) {
				CodeStructure::Code code;
				code.Add("#pragma once");
				code.NewLine();

				codeFile->AddCode(code);
			}
			if (fileStructure->createInfo_.includes_.paths_.size() == 0) {
				//__debugbreak();
			}
			//Includes.
			{
				CodeStructure::Code includes;
				fileStructure->ForEachInclude([&](const std::filesystem::path& path) {
					includes.Add(GenerateInclude(path));
					includes.NewLine();
					});
				includes.NewLine();
				codeFile->AddCode(includes);
			}
			CodeStructure::Code code = GenerateECSCXXFilesStructure(fileStructure->GetBase());
			//codeFile->path_ = fileStructure->cr

			if (fileStructure->createInfo_.isFormat_) {
				clang::format::FormatStyle style = clang::format::getMicrosoftStyle(clang::format::FormatStyle::LanguageKind::LK_Cpp);
				llvm::StringRef codeToFormat = code.code_.c_str();  // ������ ������ ����

				// ���������� �������� ��� ����� ����
				clang::tooling::Replacements replacements = clang::format::reformat(style, codeToFormat, { clang::tooling::Range(0, codeToFormat.size()) });

				auto formattedCode = clang::tooling::applyAllReplacements(codeToFormat, replacements);
				if (!formattedCode) {
					llvm::errs() << "Error formatting the code.\n";
				}
				code.code_ = *formattedCode;
			}


			codeFile->AddCode(code);
			return codeFile;
		}

	};

}