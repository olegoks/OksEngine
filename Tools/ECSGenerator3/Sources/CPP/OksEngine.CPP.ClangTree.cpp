#pragma once
#include <CPP\auto_OksEngine.CPP.ClangTree.hpp>
#include <OS.FileSystem.TextFile.hpp>
#include <format>
#include <memory>

#include <clang/Format/Format.h>

using namespace std::string_literals;

#define CPP_CONSTEXPR "constexpr"
#define CPP_CONSTEXPR_STR "constexpr"s

#define CPP_CONST "const"
#define CPP_CONST_STR "const"s

#define CPP_STATIC "static"
#define CPP_STATIC_STR "static"s

#define CPP_SPACE " "
#define CPP_SPACE_STR " "s

#define CPP_CLASS "class"
#define CPP_CLASS_STR "class"s

#define CPP_PUBLIC "public:"
#define CPP_PUBLIC_STR "public:"s

#define CPP_PRIVATE "private:"
#define CPP_PRIVATE_STR "private:"s

namespace OksEngine
{

	namespace CPP
	{
		namespace Tree
		{


			void GenerateCode::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::CommandLineParameters* commandLineParameters0,
				const OksEngine::ECS::ProjectFilePath* projectFilePath0,

				ECS2::Entity::Id entity1id,
				const OksEngine::CPP::File::Tag* cPP__File__Tag1) {

				const ECS2::Entity::Id& fileEntityId = entity1id;

				std::string code;

				const ECS2::ComponentsFilter cppfileCF = GetComponentsFilter(fileEntityId);

				if (cppfileCF.IsSet<CPP::File::Type::Hpp>()) {
					code += "#pragma once\n";
				}

				//INCLUDES
				if (cppfileCF.IsSet<CPP::File::Include::EntityIds>()) {

					const auto* includesEntityIds = GetComponent<CPP::File::Include::EntityIds>(fileEntityId);

					for (ECS2::Entity::Id includeEntityId : includesEntityIds->ids_) {

						ASSERT(IsComponentExist<CPP::File::Include::Tag>(includeEntityId));
						ASSERT(IsComponentExist<CPP::File::Include::Path>(includeEntityId));

						const std::string includePath = GetComponent<CPP::File::Include::Path>(includeEntityId)->path_;

						code += std::vformat("#include<{}>\n", std::make_format_args(includePath));
					}

				}

				std::function<void(ECS2::Entity::Id)> processNode =
					[&processNode, this, &code](ECS2::Entity::Id entityId) {

					const ECS2::ComponentsFilter nodeCF = GetComponentsFilter(entityId);

					ASSERT(nodeCF.IsSet<CPP::Tree::Node::Tag>());
					if (nodeCF.IsSet<CPP::Tree::Decl::Namespace_>()) {
						ASSERT(nodeCF.IsSet<CPP::Tree::Decl::Tag>());

						code += "namespace " + GetComponent<CPP::Tree::Node::Name>(entityId)->name_ + "{";
						if (nodeCF.IsSet<CPP::Tree::Node::ChildEntityIds>()) {
							const auto* childNodes = GetComponent<CPP::Tree::Node::ChildEntityIds>(entityId);
							for (ECS2::Entity::Id childNodeEntityId : childNodes->ids_) {
								processNode(childNodeEntityId);
							}
						}
						code += "}";
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Variable>()) {
						ASSERT(nodeCF.IsSet<CPP::Tree::Decl::Tag>());
						const auto* variable = GetComponent<CPP::Tree::Decl::Variable>(entityId);
						ASSERT(variable->initializer_ != ECS2::Entity::Id::invalid_);
						ASSERT(variable->type_ != ECS2::Entity::Id::invalid_);

						if (nodeCF.IsSet<CPP::Tree::StorageClass::Static_>()) {
							code += CPP_STATIC_STR + CPP_SPACE_STR;
						}

						if (nodeCF.IsSet<CPP::Tree::Constexpr_>()) {
							code += CPP_CONSTEXPR_STR + CPP_SPACE_STR;
						}
						if (nodeCF.IsSet<CPP::Tree::Const_>()) {
							code += CPP_CONST_STR + CPP_SPACE_STR;
						}
						//Type.
						ASSERT(IsComponentExist<CPP::Tree::Type::Tag>(variable->type_));
						code += GetComponent<CPP::Tree::Type::Name>(variable->type_)->name_ + CPP_SPACE_STR;
						//Name.
						code += GetComponent<CPP::Tree::Node::Name>(entityId)->name_;
						
						//Initializer.
						code += "{" + GetComponent<CPP::Tree::Expr::Literal>(variable->initializer_)->value_ + "}";

						code += ";";

					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Class_>()) {
						ASSERT(nodeCF.IsSet<CPP::Tree::Decl::Tag>());
						;
						code += CPP_CLASS_STR + CPP_SPACE_STR + GetComponent<CPP::Tree::Node::Name>(entityId)->name_ + "{";

						for (ECS2::Entity::Id classChildEntityId : GetComponent<CPP::Tree::Node::ChildEntityIds>(entityId)->ids_) {
							ECS2::ComponentsFilter childComponentsFilter = GetComponentsFilter(classChildEntityId);
							if (childComponentsFilter.IsSet<CPP::Tree::Decl::Variable>()) {
								//Access.
								if (childComponentsFilter.IsSet<CPP::Tree::Access::Public_>()) {
									code += CPP_PUBLIC_STR;
								}
								if (childComponentsFilter.IsSet<CPP::Tree::Access::Private_>()) {
									code += CPP_PRIVATE_STR;
								}
								auto* variable = GetComponent<CPP::Tree::Decl::Variable>(classChildEntityId);
								ASSERT(IsComponentExist<CPP::Tree::Type::Tag>(variable->type_));
								code += GetComponent<CPP::Tree::Type::Name>(variable->type_)->name_ + CPP_SPACE_STR;
								code += GetComponent<CPP::Tree::Node::Name>(classChildEntityId)->name_ + "_";

								if (variable->initializer_ != ECS2::Entity::Id::invalid_) {
									ASSERT(IsComponentExist<CPP::Tree::Expr::Tag>(variable->initializer_));
									code += "{" + GetComponent<CPP::Tree::Expr::Literal>(variable->initializer_)->value_ + "}";
								}


								code += ";";
							}
							else if (childComponentsFilter.IsSet<CPP::Tree::Decl::Function>()) {
								if (childComponentsFilter.IsSet<CPP::Tree::Decl::Constructor>()) {
									code += GetComponent<CPP::Tree::Node::Name>(entityId)->name_;
									code += "(";
									if (IsComponentExist<CPP::Tree::Node::ChildEntityIds>(classChildEntityId)) {
										//Parameters.
										bool isFirst = true;
										for (ECS2::Entity::Id maybeConstructorParameter : GetComponent<CPP::Tree::Node::ChildEntityIds>(classChildEntityId)->ids_) {
											if (IsComponentExist<CPP::Tree::Decl::Parameter>(maybeConstructorParameter)) {
												ECS2::Entity::Id& constructorParameter = maybeConstructorParameter;
												ASSERT(IsComponentExist<CPP::Tree::Node::Tag>(constructorParameter));
												ASSERT(IsComponentExist<CPP::Tree::Decl::Tag>(constructorParameter));
												const auto* nameTypeInitializer = GetComponent<CPP::Tree::Decl::Parameter>(constructorParameter);
												ASSERT(nameTypeInitializer->type_.IsValid());
												ASSERT(IsComponentExist<CPP::Tree::Type::Tag>(nameTypeInitializer->type_));

												if (!isFirst) {
													code += ",";
												}

												code += GetComponent<CPP::Tree::Type::Name>(nameTypeInitializer->type_)->name_;
												code += CPP_SPACE_STR;
												code += nameTypeInitializer->name_;

												isFirst = false;

											}
										}
									}
									code += ")";
									if (IsComponentExist<CPP::Tree::ConstructorInitList>(classChildEntityId)) {
										const auto* initList = GetComponent<CPP::Tree::ConstructorInitList>(classChildEntityId);
										code += ":";
										bool isFirst = true;
										for (ECS2::Entity::Id initializer : initList->initializers_) {
											ECS2::Entity::Id memberEntityId = GetComponent<CPP::Tree::ConstructorInitializer>(initializer)->member_;
											code += GetComponent<CPP::Tree::Node::Name>(memberEntityId)->name_;
											
											if (!isFirst) {
												code += ",";
											}
											
											code += "{";
											ECS2::Entity::Id exprEntityId = GetComponent<CPP::Tree::ConstructorInitializer>(initializer)->initExpr_;

											if (IsComponentExist<CPP::Tree::Expr::Identifier>(exprEntityId)) {
												ASSERT(IsComponentExist<CPP::Tree::Node::ResolvedRef>(exprEntityId));
												code += GetComponent<CPP::Tree::Expr::Identifier>(exprEntityId)->name_;
											} else  if (IsComponentExist<CPP::Tree::Node::ResolvedRef>(exprEntityId)) {
												ECS2::Entity::Id parameterEntityId = GetComponent<CPP::Tree::Node::ResolvedRef>(exprEntityId)->targetDecl_;
												code += GetComponent<CPP::Tree::Decl::Parameter>(parameterEntityId)->name_;
											}
											else if (IsComponentExist<CPP::Tree::Expr::Literal>(exprEntityId)) {
												code += GetComponent<CPP::Tree::Expr::Literal>(exprEntityId)->value_;
												//TODO: add types validation.
											}
											else {
												NOT_IMPLEMENTED();
											}
											code += "}";
											isFirst = false;
										}

									}
									//Empty body.
									code += "{}";

								}

							}

						}


						code += "};";
					}


					};

				const auto* cppFileRootNodes = GetComponent<CPP::Tree::Node::ChildEntityIds>(fileEntityId);

				for (ECS2::Entity::Id rootNodeEntityId : cppFileRootNodes->ids_) {
					processNode(rootNodeEntityId);
				}

				//FORMATTIG
				{
					clang::format::FormatStyle style = clang::format::getMicrosoftStyle(clang::format::FormatStyle::LanguageKind::LK_Cpp);
					{
						style.AlignEscapedNewlines = clang::format::FormatStyle::EscapedNewlineAlignmentStyle::ENAS_DontAlign;
						//style.MacroBlockBegin
						//style.AlignConsecutiveMacros 
						//style.ColumnLimit
					}

					llvm::StringRef codeToFormat = code.c_str();  // ������ ������ ����

					// ���������� �������� ��� ����� ����
					clang::tooling::Replacements replacements = clang::format::reformat(style, codeToFormat, { clang::tooling::Range(0, codeToFormat.size()) });

					auto formattedCode = clang::tooling::applyAllReplacements(codeToFormat, replacements);
					if (!formattedCode) {
						llvm::errs() << "Error formatting the code.\n";
					}
					code = *formattedCode;
				}

				//CREATE FILE.
				const auto* path = GetComponent<CPP::File::Path>(fileEntityId);
				const auto* name = GetComponent<CPP::File::Name>(fileEntityId);

				auto osFile = std::make_shared<OS::TextFile>(path->path_ + "/auto_" + name->name_ + std::string{ (cppfileCF.IsSet<CPP::File::Type::Hpp>()) ? (".hpp") : (".cpp") });
				osFile->Create();
				*osFile << code;


			}
		}

	} // namespace CPP

} // namespace OksEngine