#pragma once

#include <CPP/OksEngine.CPP.Tree.Utils.hpp>
#include <OS.FileSystem.TextFile.hpp>
#include <format>
#include <memory>

#include <clang/Format/Format.h>


namespace OksEngine
{

	namespace CPP
	{
		namespace Tree
		{


			void GenerateCode::Update(
				ECS2::Entity::Id entity1id,
				const OksEngine::CPP::File::Tag* cPP__File__Tag1) {

				const ECS2::Entity::Id& fileEntityId = entity1id;

				std::string code;

				const ECS2::ComponentsFilter cppfileCF = GetComponentsFilter(fileEntityId);

				

				std::function<void(ECS2::Entity::Id)> processNode =
					[&processNode, this, &code](ECS2::Entity::Id entityId) {

					const ECS2::ComponentsFilter nodeCF = GetComponentsFilter(entityId);
					if (nodeCF.IsSet<CPP::Tree::Preprocessor::Tag>()) {
						code += "#";
						if (nodeCF.IsSet<CPP::Tree::Preprocessor::Pragma_>()) {
							code += "pragma" + CPP_SPACE_STR + GetComponent<CPP::Tree::Preprocessor::Pragma_>(entityId)->text_ + CPP_NEWLINE_STR;
						}
						else if (nodeCF.IsSet<CPP::Tree::Preprocessor::Include_>()) {
							code += "include<" + GetComponent<CPP::Tree::Preprocessor::Include_>(entityId)->path_ + ">" + CPP_NEWLINE_STR;
						}
						else if (nodeCF.IsSet<CPP::Tree::Preprocessor::Define_>()) {
							auto* define = GetComponent<CPP::Tree::Preprocessor::Define_>(entityId);
							code += CPP_DEFINE;
							code += CPP_SPACE_STR;
							code += define->name_;
							if (!define->params_.empty()) {
								code += "(";
								bool isFirst = true;
								for (std::string paramName : define->params_) {
									if (!isFirst) {
										code += CPP_COMMA_STR;
									}
									isFirst = false;
									code += paramName;
								}
								code += ")";
							}
							if (define->bodyEntity_.IsValid()) {
								processNode(define->bodyEntity_);
							}
							else {
								code += CPP_SPACE_STR;
								code += define->rawBody_;
							}
							code += CPP_NEWLINE_STR;
						}
						else {
							NOT_IMPLEMENTED();
						}
					}
					else if (nodeCF.IsSet<CPP::Tree::Comment>()) {
						auto* comment = GetComponent<CPP::Tree::Comment>(entityId);
						if (comment->type_ == CPP::Tree::CommentType::OneLine) {
							code += "\n//" + comment->text_ + CPP_NEWLINE;
						}
						else if (comment->type_ == CPP::Tree::CommentType::Multiline) {
							code += "/*" + comment->text_ + "*/";
						}
						else {
							NOT_IMPLEMENTED();
						}
						
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Namespace_>()) {
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

						ASSERT(variable->type_ != ECS2::Entity::Id::invalid_);

						if (nodeCF.IsSet<CPP::Tree::Access::Public_>()) {
							code += CPP_PUBLIC_STR + CPP_SPACE_STR;
						}
						if (nodeCF.IsSet<CPP::Tree::Access::Private_>()) {
							code += CPP_PRIVATE_STR + CPP_SPACE_STR;
						}
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
						processNode(variable->type_);
						code += CPP_SPACE_STR;
						code += GetComponent<CPP::Tree::Node::Name>(entityId)->name_;

						//Initializer.
						if (variable->initializer_.IsValid()) {
							if (IsComponentExist<CPP::Tree::Expr::InitList>(variable->initializer_)) {
								processNode(variable->initializer_);
							}
							else {
								ASSERT(IsComponentExist<CPP::Tree::Expr::Tag>(variable->initializer_));
								code += "=";
								processNode(variable->initializer_);
							}
						}
						code += ";";

					}
					else if (nodeCF.IsSet<CPP::Tree::Type::Tag>()) {
						if (IsComponentExist<CPP::Tree::Type::ConstType>(entityId)) {
							code += CPP_CONST_STR;
							code += CPP_SPACE_STR;
						}
						if (nodeCF.IsSet<CPP::Tree::Type::Name>()) {
							code += GetComponent<CPP::Tree::Type::Name>(entityId)->name_;
						}
						else if (nodeCF.IsSet<CPP::Tree::Type::TemplateSpecializationType>()) {
							const auto* templateSpecializationType = GetComponent<CPP::Tree::Type::TemplateSpecializationType>(entityId);
							processNode(templateSpecializationType->baseType_);
							code += "<";
							bool isFirst = true;
							for (ECS2::Entity::Id templateArg : templateSpecializationType->templateArgs_) {
								if (!isFirst) {
									code += ",";
								}
								processNode(templateArg);
								isFirst = false;
							}
							code += ">";
						}
						else if (nodeCF.IsSet<CPP::Tree::Type::BuiltinType>()) {
							const auto* builtinType = GetComponent<CPP::Tree::Type::BuiltinType>(entityId);
							if (builtinType->type_ == CPP::Tree::Type::BuiltinTypes::Void) {
								code += CPP_VOID_STR;
							}
							else if (builtinType->type_ == CPP::Tree::Type::BuiltinTypes::Char) {
								code += CPP_CHAR_STR;
							}
							else if (builtinType->type_ == CPP::Tree::Type::BuiltinTypes::Bool) {
								code += CPP_BOOL_STR;
							}
							else if (builtinType->type_ == CPP::Tree::Type::BuiltinTypes::Float) {
								code += CPP_FLOAT_STR;
							}
							else {
								NOT_IMPLEMENTED();
							}
						}
						else if (nodeCF.IsSet<CPP::Tree::Type::PointerType>()) {
							const auto* pointerType = GetComponent<CPP::Tree::Type::PointerType>(entityId);
							processNode(pointerType->pointer_);
							code += "*";
						}
						else if (nodeCF.IsSet<CPP::Tree::Type::ReferenceType>()) {
							const auto* referenceType = GetComponent<CPP::Tree::Type::ReferenceType>(entityId);
							processNode(referenceType->id_);
							code += "&";
						}
						else {
							NOT_IMPLEMENTED();
						}
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Function>()) {

						if (nodeCF.IsSet<CPP::Tree::Access::Public_>()) {
							code += CPP_PUBLIC_STR + CPP_SPACE_STR;
						}
						if (nodeCF.IsSet<CPP::Tree::Access::Private_>()) {
							code += CPP_PRIVATE_STR + CPP_SPACE_STR;
						}

						if (IsComponentExist<CPP::Tree::Decl::TemplateDecl>(entityId) || IsComponentExist<CPP::Tree::Decl::ExplicitTemplateArgs>(entityId)) {
							ASSERT(!nodeCF.IsSet<CPP::Tree::Decl::Constructor>());
							std::vector<ECS2::Entity::Id> templateParamEntityIds;
							if (IsComponentExist<CPP::Tree::Decl::TemplateDecl>(entityId)) {
								templateParamEntityIds = GetComponent<CPP::Tree::Decl::TemplateDecl>(entityId)->templateParams_;
							}
	
							if (!templateParamEntityIds.empty() || IsComponentExist<CPP::Tree::Decl::ExplicitTemplateArgs>(entityId)) {
								code += "template";
								code += "<";
								bool isFirst = true;
								for (ECS2::Entity::Id templateParamEntityId : templateParamEntityIds) {
									if (!isFirst) {
										code += ",";
									}
									isFirst = false;
									const auto* templateTypeParam = GetComponent<CPP::Tree::Decl::TemplateTypeParam>(templateParamEntityId);
									code += CPP_CLASS_STR;
									if (templateTypeParam->isParameterPack_) {
										code += "...";
										ASSERT(templateTypeParam->defaultType_.IsInvalid());
									}
									code += CPP_SPACE_STR;
									processNode(templateParamEntityId);
									if (templateTypeParam->defaultType_.IsValid()) {
										code += "=";
										code += CPP_SPACE_STR;
										processNode(templateTypeParam->defaultType_);
									}
								}
								code += ">";
							}
						}


						if (nodeCF.IsSet<CPP::Tree::StorageClass::Static_>()) {
							code += CPP_STATIC_STR + CPP_SPACE_STR;
						}

						if (nodeCF.IsSet<CPP::Tree::Inline_>()) {
							code += CPP_INLINE_STR + CPP_SPACE_STR;
						}

						//For constructor returnType_ will be ECS2::Entity::Id::invalid_.
						if (GetComponent<CPP::Tree::Decl::Function>(entityId)->returnType_ != ECS2::Entity::Id::invalid_) {
							ECS2::Entity::Id returnTypeEntityId = GetComponent<CPP::Tree::Decl::Function>(entityId)->returnType_;
							ASSERT(IsComponentExist<CPP::Tree::Type::Tag>(returnTypeEntityId));
							processNode(returnTypeEntityId);//TODO: replace with PointerType
							code += CPP_SPACE_STR;
						}

						code += GetComponent<CPP::Tree::Node::Name>(entityId)->name_;


						if (IsComponentExist<CPP::Tree::Decl::ExplicitTemplateArgs>(entityId)) {
							ASSERT(!nodeCF.IsSet<CPP::Tree::Decl::Constructor>());
							std::vector<ECS2::Entity::Id> explicitTemplateParamEntityIds = GetComponent<CPP::Tree::Decl::ExplicitTemplateArgs>(entityId)->templateArgs_;
							if (!explicitTemplateParamEntityIds.empty()) {
								code += "<";
								bool isFirst = true;
								for (ECS2::Entity::Id explicitTemplateParamEntityId : explicitTemplateParamEntityIds) {
									if (!isFirst) {
										code += ",";
										code += CPP_SPACE_STR;
									}
									isFirst = false;
									processNode(explicitTemplateParamEntityId);
								}
								code += ">";
							}
						}


						code += "(";
						if (IsComponentExist<CPP::Tree::Node::ChildEntityIds>(entityId)) {
							//Parameters.
							bool isFirst = true;
							for (ECS2::Entity::Id maybeConstructorParameter : GetComponent<CPP::Tree::Node::ChildEntityIds>(entityId)->ids_) {
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
									processNode(maybeConstructorParameter);
									isFirst = false;

								}
							}
						}
						code += ")";
						if (nodeCF.IsSet<CPP::Tree::Decl::Constructor>()) {
							if (IsComponentExist<CPP::Tree::ConstructorInitList>(entityId)) {
								const auto* initList = GetComponent<CPP::Tree::ConstructorInitList>(entityId);
								code += ":";
								bool isFirst = true;
								for (ECS2::Entity::Id initializer : initList->initializers_) {
									ECS2::Entity::Id memberEntityId = GetComponent<CPP::Tree::ConstructorInitializer>(initializer)->member_;


									if (!isFirst) {
										code += ",";
									}
									isFirst = false;
									code += GetComponent<CPP::Tree::Node::Name>(memberEntityId)->name_;

									code += "{";
									ECS2::Entity::Id exprEntityId = GetComponent<CPP::Tree::ConstructorInitializer>(initializer)->initExpr_;

									if (IsComponentExist<CPP::Tree::Expr::Identifier>(exprEntityId)) {
										code += GetComponent<CPP::Tree::Expr::Identifier>(exprEntityId)->name_;
									}
									else  if (IsComponentExist<CPP::Tree::Node::ResolvedRef>(exprEntityId)) {
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

								}

							}
						}
						if (IsComponentExist<CPP::Tree::Node::ChildEntityIds>(entityId)) {
							//Parameters.
							bool isFirst = true;
							bool openBracket = false;
							for (ECS2::Entity::Id maybeBody : GetComponent<CPP::Tree::Node::ChildEntityIds>(entityId)->ids_) {
								if (IsComponentExist<CPP::Tree::Stmt::Tag>(maybeBody)) {
									if (!openBracket) {
										code += "{";
										openBracket = true;
									}
									processNode(maybeBody);
									
								}
							}
							if (openBracket) {
								code += "}";
							}
						}
						code += ";";
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::TemplateTypeParam>()) {
						const auto* templateTypeParam = GetComponent<CPP::Tree::Decl::TemplateTypeParam>(entityId);
						code += templateTypeParam->name_;
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::UsingDirective>()) {
						const auto* usingDirective = GetComponent<CPP::Tree::Decl::UsingDirective>(entityId);
						code += "using namespace" + CPP_SPACE_STR + usingDirective->namespaceName_ + ";";
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Class_>()) {
						ASSERT(nodeCF.IsSet<CPP::Tree::Decl::Tag>());
						code += CPP_CLASS_STR + CPP_SPACE_STR + GetComponent<CPP::Tree::Node::Name>(entityId)->name_ + "{";

						auto classChilds = GetComponent<CPP::Tree::Node::ChildEntityIds>(entityId)->ids_;

						for (ECS2::Entity::Id classChildEntityId : classChilds) {
							ECS2::ComponentsFilter childComponentsFilter = GetComponentsFilter(classChildEntityId);
							if (childComponentsFilter.IsSet<CPP::Tree::Decl::Variable>()) {
								processNode(classChildEntityId);
							}
							else if (childComponentsFilter.IsSet<CPP::Tree::Decl::Function>()) {
								processNode(classChildEntityId);
							}

						}
						code += "};";
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Enum_>()) {
						ASSERT(nodeCF.IsSet<CPP::Tree::Decl::Tag>());
						code += CPP_ENUM_STR + CPP_SPACE_STR + CPP_CLASS_STR + CPP_SPACE_STR + GetComponent<CPP::Tree::Node::Name>(entityId)->name_;
						code += "{";
						bool isFirst = true;
						for (ECS2::Entity::Id enumeratorEntityId : GetComponent<CPP::Tree::Node::ChildEntityIds>(entityId)->ids_) {
							if (!isFirst) {
								code += ",";
							}
							processNode(enumeratorEntityId);
							isFirst = false;
						}
						code += "};";
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Enumerator>()) {
						ASSERT(nodeCF.IsSet<CPP::Tree::Decl::Tag>());
						const auto* enumerator = GetComponent<CPP::Tree::Decl::Enumerator>(entityId);
						code += enumerator->name_;
						if (enumerator->valueExpr_.IsValid()) {
							NOT_IMPLEMENTED();
						}
					}
					else if (nodeCF.IsSet<CPP::Tree::Stmt::Tag>()) {
						if (nodeCF.IsSet<CPP::Tree::Stmt::CompoundStmt>()) {
							for (ECS2::Entity::Id childStmtEntityId : GetComponent<CPP::Tree::Node::ChildEntityIds>(entityId)->ids_) {
								processNode(childStmtEntityId);
							}
						}
						else if (nodeCF.IsSet<CPP::Tree::Stmt::ReturnStmt>()) {
							code += CPP_RETURN_STR + CPP_SPACE_STR;
							ECS2::Entity::Id returnExpr = GetComponent<CPP::Tree::Stmt::ReturnStmt>(entityId)->expr_;
							processNode(returnExpr);
							code += ";";
						}
						else if (nodeCF.IsSet<CPP::Tree::Stmt::ExpressionStmt>()) {
							ECS2::Entity::Id exprEntityId = GetComponent<CPP::Tree::Stmt::ExpressionStmt>(entityId)->expr_;
							ASSERT(exprEntityId.IsValid());
							processNode(exprEntityId);
							code += ";";
						}
						else {
							NOT_IMPLEMENTED();
						}
					}
					else if (nodeCF.IsSet<CPP::Tree::Expr::Tag>()) {
						if (nodeCF.IsSet<CPP::Tree::Expr::Literal>()) {
							code += GetComponent<CPP::Tree::Expr::Literal>(entityId)->value_;
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::Identifier>()) {
							code += GetComponent<CPP::Tree::Expr::Identifier>(entityId)->name_;
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::Lambda>()) {
							const auto* lambda = GetComponent<CPP::Tree::Expr::Lambda>(entityId);
							code += "[";
							{//Capture list
								if (lambda->captureType_ != CPP::Tree::Expr::CaptureType::Undefined) {
									if (lambda->captureType_ == CPP::Tree::Expr::CaptureType::ByReference) {
										code += "&";
									}
									else if (lambda->captureType_ == CPP::Tree::Expr::CaptureType::ByValue) {
										code += "=";
									}
									if (!lambda->captureList_.empty()) {
										code += ",";
									}
								}
								bool isFirst = true;
								for (const auto& capture : lambda->captureList_) {
									if (!isFirst) {
										code += ",";
									}
									isFirst = false;
									if (capture.captureType_ == CPP::Tree::Expr::CaptureType::ByReference) {
										code += "&";
									}
									else if (capture.captureType_ == CPP::Tree::Expr::CaptureType::ByValue) {
										//Do nothing.
									}
									else {
										ASSERT_FAIL();
									}
									processNode(capture.decl_);
								}
							}
							code += "]";
							code += "(";

							{//Parameters
								bool isFirst = true;
								for (const auto& parameterEntityId : lambda->parameters_) {
									if (!isFirst) {
										code += ",";
									}
									isFirst = false;

									processNode(parameterEntityId);
								}
							}
							code += ")";
							code += "{";
							{
								processNode(lambda->body_);
							}
							code += "}";
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::CallExpr>()) {
							const auto* callExpr = GetComponent<CPP::Tree::Expr::CallExpr>(entityId);
							processNode(callExpr->callee_);
							if (!callExpr->templateArgs_.empty()) {
								code += "<";
								bool isFirst = true;
								for (ECS2::Entity::Id typeEntityId : callExpr->templateArgs_) {
									if (!isFirst) {
										code += ",";
									}
									isFirst = false;
									processNode(typeEntityId);
									if (IsComponentExist<CPP::Tree::Decl::TemplateTypeParam>(typeEntityId)) {
										const auto* templateTypeParam = GetComponent<CPP::Tree::Decl::TemplateTypeParam>(typeEntityId);
										if (templateTypeParam->isParameterPack_) {
											code += "...";
										}
									}
								}
								code += ">";
							}
							code += "(";
							bool isFirst = true;
							for (ECS2::Entity::Id argEntityId : callExpr->arguments_) {
								if (!isFirst) {
									code += ",";
								}
								isFirst = false;
								processNode(argEntityId);
							}
							code += ")";
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::MemberAccess>()) {
							const auto* memberAccess = GetComponent<CPP::Tree::Expr::MemberAccess>(entityId);
							processNode(memberAccess->object_);
							if (memberAccess->isArrow_) {
								code += "->";
							}
							else {
								code += ".";
							}
							code += memberAccess->memberName_;
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::MacroInvocation>()) {
							const auto* invocation = GetComponent<CPP::Tree::Expr::MacroInvocation>(entityId);
							code += invocation->name_;
							code += "(";
							bool isFirst = true;
							for (auto& argument : invocation->arguments_) {
								if (!isFirst) {
									code += ", ";
								}
								isFirst = false;
								processNode(argument);
							}
							code += ")";
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::TemplateIdExpr>()) {
							const auto* templateId = GetComponent<CPP::Tree::Expr::TemplateIdExpr>(entityId);
							code += templateId->name_;
							code += "<";
							bool isFirst = true;
							for (auto& argument : templateId->templateArgs_) {
								if (!isFirst) {
									code += ", ";
								}
								isFirst = false;
								processNode(argument);
							}
							code += ">";
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::UnaryOp>()) {
							const auto* unaryOp = GetComponent<CPP::Tree::Expr::UnaryOp>(entityId);
							if (unaryOp->operation_ == CPP::Tree::Expr::UnaryOpType::AddressOf) {
								code += "&";
							}
							else {
								NOT_IMPLEMENTED();
							}
							processNode(unaryOp->operand_);
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::BinaryOp>()) {
							const auto* binaryOp = GetComponent<CPP::Tree::Expr::BinaryOp>(entityId);
							ASSERT(binaryOp->left_.IsValid());
							ASSERT(binaryOp->right_.IsValid());
							if (binaryOp->operation_ == CPP::Tree::Expr::BinaryOpType::Plus) {
								processNode(binaryOp->left_);
								code += "+";
								processNode(binaryOp->right_);
							}
							else if (binaryOp->operation_ == CPP::Tree::Expr::BinaryOpType::PlusAssign) {
								processNode(binaryOp->left_);
								code += "+=";
								processNode(binaryOp->right_);
							}
							else if (binaryOp->operation_ == CPP::Tree::Expr::BinaryOpType::Assign) {
								processNode(binaryOp->left_);
								code += "=";
								processNode(binaryOp->right_);
							}
							else {
								NOT_IMPLEMENTED();
							}
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::ArraySubscript>()) {
							const auto* arraySubscript = GetComponent<CPP::Tree::Expr::ArraySubscript>(entityId);
							processNode(arraySubscript->array_);
							code += "[";
							processNode(arraySubscript->index_);
							code += "]";
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::InitList>()) {
							code += "{";
							const auto* initList = GetComponent<CPP::Tree::Expr::InitList>(entityId);
							bool isFirst = true;
							for (auto element : initList->elements_) {
								if (!isFirst) {
									code += ",";
								}
								isFirst = false;
								processNode(element);
							}
							code += "}";
						}
						else if (nodeCF.IsSet<CPP::Tree::Expr::MemberPtrExpr>()) {

							auto* memberPtrExpr = GetComponent<CPP::Tree::Expr::MemberPtrExpr>(entityId);
							code += "&";
							processNode(memberPtrExpr->classType_);
							code += "::";
							code += memberPtrExpr->memberName_;
						}
						else {
							NOT_IMPLEMENTED();
						}
					}
					else if (nodeCF.IsSet<CPP::Tree::Decl::Parameter>()) {
						const auto* nameTypeInitializer = GetComponent<CPP::Tree::Decl::Parameter>(entityId);
						processNode(nameTypeInitializer->type_);
						code += CPP_SPACE_STR;
						code += nameTypeInitializer->name_;
					}
					else {
						NOT_IMPLEMENTED();
					}


					};

				if (IsComponentExist<CPP::Tree::Node::ChildEntityIds>(fileEntityId)) {
					const auto* fileRootNodes = GetComponent<CPP::Tree::Node::ChildEntityIds>(fileEntityId);

					for (ECS2::Entity::Id rootNodeEntityId : fileRootNodes->ids_) {
						processNode(rootNodeEntityId);
					}

					//FORMATTIG
					{
						clang::format::FormatStyle style = clang::format::getMicrosoftStyle(clang::format::FormatStyle::LanguageKind::LK_Cpp);
						{
							style.AlignEscapedNewlines = clang::format::FormatStyle::EscapedNewlineAlignmentStyle::ENAS_DontAlign;
							style.NamespaceIndentation = clang::format::FormatStyle::NI_All;
							style.BreakBeforeBraces = clang::format::FormatStyle::BS_Custom;
							style.BraceWrapping.AfterNamespace = false; // Скобка namespace на той же строке
							style.BraceWrapping.AfterClass = false;     // Скобка class на той же строке
							style.BraceWrapping.AfterStruct = false;    // Скобка struct на той же строке
							style.BraceWrapping.AfterEnum = false;      // Скобка enum на той же строке
							style.BraceWrapping.AfterFunction = false;
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
				}

				//CREATE FILE.
				const auto* path = GetComponent<CPP::File::Path>(fileEntityId);
				const auto* name = GetComponent<CPP::File::Name>(fileEntityId);

				auto osFile = std::make_shared<OS::TextFile>(path->path_ / std::filesystem::path(name->name_ + std::string{ (cppfileCF.IsSet<CPP::File::Type::Hpp>()) ? (".hpp") : (".cpp") }));
				osFile->Create();
				*osFile << code;


			}
		}

	} // namespace CPP

} // namespace OksEngine