#pragma once 

#include <string>
#include <CPP\auto_OksEngine.CPP.ClangTree.hpp>

using namespace std::string_literals;

#define CPP_CONSTEXPR "constexpr"
#define CPP_CONSTEXPR_STR "constexpr"s

#define CPP_CONST "const"
#define CPP_CONST_STR "const"s

#define CPP_VOID "void"
#define CPP_VOID_STR "void"s

#define CPP_CHAR "char"
#define CPP_CHAR_STR "char"s

#define CPP_BOOL "bool"
#define CPP_BOOL_STR "bool"s

#define CPP_FLOAT "float"
#define CPP_FLOAT_STR "float"s

#define CPP_ENUM "enum"
#define CPP_ENUM_STR "enum"s

#define CPP_RETURN "return"
#define CPP_RETURN_STR "return"s

#define CPP_STATIC "static"
#define CPP_STATIC_STR "static"s

#define CPP_INLINE "inline"
#define CPP_INLINE_STR "inline"s

#define CPP_SPACE " "
#define CPP_SPACE_STR " "s
#define CPP_COMMA ","
#define CPP_COMMA_STR ","s

#define CPP_NEWLINE "\n"
#define CPP_NEWLINE_STR "\n"s

#define CPP_CLASS "class"
#define CPP_CLASS_STR "class"s

#define CPP_PUBLIC "public:"
#define CPP_PUBLIC_STR "public:"s

#define CPP_PRIVATE "private:"
#define CPP_PRIVATE_STR "private:"s

#define CPP_DEFINE "define"
#define CPP_DEFINE_STR "define"s


#define CPP__TREE__CREATE_ENTITIES_VECTOR(...)std::vector<ECS2::Entity::Id>{__VA_ARGS__}

/*					COMMENTS						*/
#define CPP__TREE__COMMENT__CREATE_ONE_LINE_COMMENT(str)\
	[this](const std::string& text){\
		ECS2::Entity::Id comment = CreateEntity();\
		CreateComponent<CPP::Tree::Comment>(comment, text, CPP::Tree::CommentType::OneLine);\
		return comment;\
	}(str)

/*					PREPROCESSOR					*/
#define CPP__TREE__PREPROCESSOR__CREATE_PRAGMA(str)\
	[this](const std::string& text){\
		ECS2::Entity::Id pragma = CreateEntity();\
		CreateComponent<CPP::Tree::Preprocessor::Tag>(pragma);\
		CreateComponent<CPP::Tree::Preprocessor::Pragma_>(pragma, text);\
		return pragma;\
	}(str)

#define CPP__TREE__PREPROCESSOR__CREATE_PRAGMA_OPTIMIZE()\
	[this](){\
		ECS2::Entity::Id pragmaOptimize = CreateEntity();\
		CreateComponent<CPP::Tree::Preprocessor::Tag>(pragmaOptimize);\
		CreateComponent<CPP::Tree::Preprocessor::Pragma_>(pragmaOptimize, "optimize");\
		return pragmaOptimize;\
	}()

#define CPP__TREE__PREPROCESSOR__CREATE_PRAGMA_ONCE()\
	[this](){\
		ECS2::Entity::Id pragmaOptimize = CreateEntity();\
		CreateComponent<CPP::Tree::Preprocessor::Tag>(pragmaOptimize);\
		CreateComponent<CPP::Tree::Preprocessor::Pragma_>(pragmaOptimize, "once");\
		return pragmaOptimize;\
	}()

#define CPP__TREE__PREPROCESSOR__CREATE_INCLUDE(str)\
	[this](const std::string& text){\
		ECS2::Entity::Id includeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Preprocessor::Tag>(includeEntityId);\
		CreateComponent<CPP::Tree::Preprocessor::Include_>(includeEntityId, text, false);\
		return includeEntityId;\
	}(str)

#define CPP__TREE__PREPROCESSOR__CREATE_DEFINE(name, params, bodyStr, bodyEntity, isFunction)\
	[this](const std::string& nameStr, const std::vector<std::string>& parameters, const std::string& bodyString, ECS2::Entity::Id bodyEntityId, bool isFunctionLike){\
		ECS2::Entity::Id macrosEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Preprocessor::Tag>(macrosEntityId);\
		CreateComponent<CPP::Tree::Preprocessor::Define_>(macrosEntityId,nameStr, parameters, bodyString, bodyEntityId, isFunctionLike);\
		return macrosEntityId;\
	}(name, params, bodyStr, bodyEntity, isFunction)


/*						VARIABLE DECLARATION						*/
#define CPP__TREE__DECL__CREATE_VARIABLE_COMPONENTS(variable, type, name, initialize, parent)\
	[this](ECS2::Entity::Id variableEntityId, ECS2::Entity::Id typeEntityId, const std::string& variableName, ECS2::Entity::Id initializeEntityId, ECS2::Entity::Id parentEntityId){\
			CreateComponent<CPP::Tree::Node::Tag>(variableEntityId);\
			CreateComponent<CPP::Tree::Decl::Tag>(variableEntityId);\
			CreateComponent<CPP::Tree::Decl::Variable>(variableEntityId, typeEntityId, initializeEntityId);\
			CreateComponent<CPP::Tree::Node::Name>(variableEntityId, variableName);\
			if(parentEntityId.IsValid()){\
				CreateComponent<CPP::Tree::Node::ParentEntityId>(variableEntityId, parentEntityId); \
			}\
			return variableEntityId;\
			}(variable, type, name, initialize, parent)

#define CPP__TREE__DECL__CREATE_VARIABLE(type, name, initialize, parent)\
	[this](\
		ECS2::Entity::Id typeEntityId,\
		const std::string& variableName,\
		ECS2::Entity::Id initializeEntityId,\
		ECS2::Entity::Id parentEntityId){\
			ECS2::Entity::Id variableEntityId = CreateEntity();\
			CPP__TREE__DECL__CREATE_VARIABLE_COMPONENTS(\
				variableEntityId,\
				typeEntityId,\
				variableName,\
				initializeEntityId,\
				parentEntityId);\
			return variableEntityId;\
			}(type, name, initialize, parent)

#define CPP__TREE__DECL__CREATE_STATIC_VARIABLE(type, name, initialize, parent)\
	[this](\
		ECS2::Entity::Id typeEntityId,\
		const std::string& variableName,\
		ECS2::Entity::Id initializeEntityId,\
		ECS2::Entity::Id parentEntityId){\
			ECS2::Entity::Id variableEntityId = CPP__TREE__DECL__CREATE_VARIABLE(\
				typeEntityId,\
				variableName,\
				initializeEntityId,\
				parentEntityId);\
			CreateComponent<CPP::Tree::StorageClass::Static_>(variableEntityId);\
			return variableEntityId;\
	}(type, name, initialize, parent)


#define CPP__TREE__DECL__CREATE_STATIC_CONSTEXPR_VARIABLE(type, name, initialize, parent)\
	[this](ECS2::Entity::Id typeEntityId, const std::string& variableName, ECS2::Entity::Id initializeEntityId, ECS2::Entity::Id parentEntityId){\
			const ECS2::Entity::Id variableEntityId = CPP__TREE__DECL__CREATE_VARIABLE(typeEntityId, variableName, initializeEntityId, parentEntityId);\
			CreateComponent<CPP::Tree::StorageClass::Static_>(variableEntityId);\
			CreateComponent<CPP::Tree::Constexpr_>(variableEntityId);\
			return variableEntityId;\
			}(type, name, initialize, parent)


/*							NAMESPACE DECLARATION										*/
#define CPP__TREE__DECL__CREATE_NAMESPACE(namespaceName)\
	[this](const std::string& name){\
			const ECS2::Entity::Id namespaceEntityId = CreateEntity();\
			CreateComponent<CPP::Tree::Node::Tag>(namespaceEntityId);\
			CreateComponent<CPP::Tree::Decl::Tag>(namespaceEntityId);\
			CreateComponent<CPP::Tree::Decl::Namespace_>(namespaceEntityId);\
			CreateComponent<CPP::Tree::Node::Name>(namespaceEntityId, name);\
			return namespaceEntityId;\
			}(namespaceName)


//If cppTableEntity id has only one parent namespace firstNamespaceEntityId == lastNamespaceEntityId == valid ECS2::Entity::Id
//If cppTableEntity has no parents firstNamespaceEntityId == lastNamespaceEntityId == ECS2::Entity::Id::invalid_
#define CPP__TREE__CREATE_CPP_NAMESPACES(parentEntityIds, entityId)\
		[this](const std::vector<ECS2::Entity::Id>& parentTableEntityIds, std::vector<ECS2::Entity::Id> cppTableEntityId) ->std::pair<ECS2::Entity::Id, ECS2::Entity::Id> {\
			ECS2::Entity::Id firstNamespaceEntityId = ECS2::Entity::Id::invalid_;\
			ECS2::Entity::Id lastNamespaceEntityId = ECS2::Entity::Id::invalid_;\
			std::vector<ECS2::Entity::Id> cppNamespaceEntityIds;\
			for (Common::Index i = 0; i < parentTableEntityIds.size(); i++) {\
				ECS2::Entity::Id parentTableEntityId = parentTableEntityIds[i]; \
				const ECS2::Entity::Id namespaceEntityId = CPP__TREE__DECL__CREATE_NAMESPACE(GetComponent<File::Table::Name>(parentTableEntityId)->name_);\
				cppNamespaceEntityIds.push_back(namespaceEntityId);\
			}\
			for (Common::Index i = 0; i < parentTableEntityIds.size(); i++) {\
				ECS2::Entity::Id parentTableEntityId = parentTableEntityIds[i];\
				const ECS2::ComponentsFilter tableCF = GetComponentsFilter(parentTableEntityId);\
				ASSERT(tableCF.IsSet<File::Table::Namespace::Tag>());\
				ASSERT(tableCF.IsSet<File::Table::ChildTablesEntityIds>());\
				const ECS2::Entity::Id namespaceEntityId = cppNamespaceEntityIds[i];\
				if (i != parentTableEntityIds.size() - 1) {\
					CreateComponent<CPP::Tree::Node::ChildEntityIds>(namespaceEntityId, std::vector{ cppNamespaceEntityIds[i + 1] });\
				}\
				else {\
					if(!cppTableEntityId.empty()){\
						CreateComponent<CPP::Tree::Node::ChildEntityIds>(namespaceEntityId, cppTableEntityId);\
					}\
					lastNamespaceEntityId = namespaceEntityId; \
				}\
				if (i != 0) {\
					CreateComponent<CPP::Tree::Node::ParentEntityId>(namespaceEntityId, cppNamespaceEntityIds[i - 1]);\
				}\
				else {\
					firstNamespaceEntityId = namespaceEntityId;\
				}\
			}\
			return { firstNamespaceEntityId, lastNamespaceEntityId };\
		}(parentEntityIds, entityId)


/*						TYPES							*/
//Example: 
//std::shared_ptr
//ECS2::World
#define CPP__TREE__TYPE__CREATE_NAMED_TYPE(typeName)\
	[this](const std::string& name){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::Name>(typeEntityId, name);\
		return typeEntityId;\
	}(typeName)

#define CPP__TREE__TYPE__CREATE_AUTO_TYPE()\
	[this](){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::Name>(typeEntityId, "auto");\
		return typeEntityId;\
	}()

//returns std::vector<ECS2::Entity::Id>
#define CPP__TREE__TYPE__CREATE_NAMED_TYPE_ENTITIES_VECTOR(...)\
	[this](const std::vector<std::string> names){\
	std::vector<ECS2::Entity::Id> typeEntityIds;\
	for(std::string name : names){\
		typeEntityIds.push_back(CPP__TREE__TYPE__CREATE_NAMED_TYPE(name));\
	}\
	return typeEntityIds;\
	}(std::vector<std::string>{__VA_ARGS__})

//Example: 
//std::shared_ptr
//ECS2::World
#define CPP__TREE__TYPE__CREATE_VOID_TYPE()\
	[this](){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::BuiltinType>(typeEntityId, CPP::Tree::Type::BuiltinTypes::Void);\
		return typeEntityId;\
	}()

#define CPP__TREE__TYPE__CREATE_BOOL_TYPE()\
	[this](){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::BuiltinType>(typeEntityId, CPP::Tree::Type::BuiltinTypes::Bool);\
		return typeEntityId;\
	}()

#define CPP__TREE__TYPE__CREATE_FLOAT_TYPE()\
	[this](){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::BuiltinType>(typeEntityId, CPP::Tree::Type::BuiltinTypes::Float);\
		return typeEntityId;\
	}()

#define CPP__TREE__TYPE__CREATE_CHAR_TYPE()\
	[this](){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::BuiltinType>(typeEntityId, CPP::Tree::Type::BuiltinTypes::Char);\
		return typeEntityId;\
	}()

#define CPP__TREE__TYPE__CREATE_CONST_CHAR_TYPE()\
	[this](){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::BuiltinType>(typeEntityId, CPP::Tree::Type::BuiltinTypes::Char);\
		CreateComponent<CPP::Tree::Type::ConstType>(typeEntityId);\
		return typeEntityId;\
	}()

#define CPP__TREE__TYPE__CREATE_POINTER_TYPE(type)\
	[this](ECS2::Entity::Id pointerToTypeEntityId){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::PointerType>(typeEntityId, pointerToTypeEntityId);\
		return typeEntityId;\
	}(type)

#define CPP__TREE__TYPE__CREATE_CONST_CHAR_POINTER_TYPE()\
	[this](){\
		return CPP__TREE__TYPE__CREATE_POINTER_TYPE(\
			CPP__TREE__TYPE__CREATE_CONST_CHAR_TYPE()\
		);\
	}()\

#define CPP__TREE__TYPE__CREATE_REFERENCE_TYPE(type, rvalue)\
	[this](ECS2::Entity::Id referenceToTypeEntityId, bool isRvalue){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::ReferenceType>(typeEntityId, referenceToTypeEntityId, isRvalue);\
		return typeEntityId;\
	}(type, rvalue)

//Example: std::shared_ptr<ECS2::World>
// baseType<templateArgs>
#define CPP__TREE__TYPE__CREATE_TEMPLATE_SPECIALIZATION_TYPE(base, args)\
	[this](ECS2::Entity::Id baseType, std::vector<ECS2::Entity::Id> templateArgs){\
		ECS2::Entity::Id typeEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Type::Tag>(typeEntityId);\
		CreateComponent<CPP::Tree::Type::TemplateSpecializationType>(typeEntityId, baseType, templateArgs);\
		return typeEntityId;\
	}(base, args)

/*						STATEMENTS						*/

#define CPP__TREE__STMT__CREATE_EXPRESSION_STATEMENT(expr)\
	[this](ECS2::Entity::Id exprEntityId){\
		ECS2::Entity::Id stmtEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Stmt::Tag>(stmtEntityId);\
		CreateComponent<CPP::Tree::Stmt::ExpressionStmt>(stmtEntityId, exprEntityId);\
		return stmtEntityId;\
	}(expr)

#define CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT_COMPONENTS(entity, childs)\
	[this](ECS2::Entity::Id entityId, std::vector<ECS2::Entity::Id> childEntityIds){\
		CreateComponent<CPP::Tree::Node::Tag>(entityId);\
		CreateComponent<CPP::Tree::Stmt::Tag>(entityId);\
		CreateComponent<CPP::Tree::Stmt::CompoundStmt>(entityId);\
		CreateComponent<CPP::Tree::Node::ChildEntityIds>(entityId, childEntityIds);\
		return entityId;\
		}(entity, childs)

#define CPP__TREE__STMT__CREATE_EMPTY_COMPOUND_STATEMENT()\
	[this](){\
		ECS2::Entity::Id emptyStatementEntityId = CreateEntity();\
		CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT_COMPONENTS(emptyStatementEntityId, std::vector<ECS2::Entity::Id>{});\
		return emptyStatementEntityId;\
		}()

#define CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT(childs)\
	[this](std::vector<ECS2::Entity::Id> childEntityIds){\
		ECS2::Entity::Id emptyStatementEntityId = CreateEntity();\
		CPP__TREE__STMT__CREATE_COMPOUND_STATEMENT_COMPONENTS(emptyStatementEntityId, childEntityIds);\
		return emptyStatementEntityId;\
		}(childs)

#define CPP__TREE__STMT__CREATE_RETURN_STATEMENT(entityId)\
	[this](ECS2::Entity::Id returnExprEntityId){\
		ECS2::Entity::Id returnStmtEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Stmt::Tag>(returnStmtEntityId);\
		CreateComponent<CPP::Tree::Stmt::ReturnStmt>(returnStmtEntityId, returnExprEntityId);\
		return returnStmtEntityId;\
		}(entityId)

/*						PARAMETER DECLARATION								*/
#define CPP__TREE__DECL__CREATE_PARAMETER(type, name)\
	[this](ECS2::Entity::Id typeEntityId, std::string parameterName){\
		ECS2::Entity::Id parameterEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Node::Tag>(parameterEntityId);\
		CreateComponent<CPP::Tree::Decl::Tag>(parameterEntityId);\
		CreateComponent<CPP::Tree::Decl::Parameter>(parameterEntityId,\
			parameterName,\
			typeEntityId,\
			ECS2::Entity::Id::invalid_\
		);\
		return parameterEntityId;\
	}(type, name)

/*						EXPRESSION							*/

#define CPP__TREE__EXPR__CREATE_MACRO_INVOCATION(name, args, macro)\
	[this](const std::string& macroName, std::vector<ECS2::Entity::Id> arguments, ECS2::Entity::Id macroEntityId){\
		ECS2::Entity::Id macroInvocationEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(macroInvocationEntityId);\
		CreateComponent<CPP::Tree::Expr::MacroInvocation>(macroInvocationEntityId, macroName, arguments, macroEntityId);\
		return macroInvocationEntityId;\
	}(name, args, macro)

#define CPP__TREE__EXPR__CREATE_LITERAL_EXPR(text, suffix)\
	[this](const std::string& literal, const std::string& suffixStr){\
		ECS2::Entity::Id literalEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(literalEntityId);\
		CreateComponent<CPP::Tree::Expr::Literal>(literalEntityId,\
			Common::Limits<decltype(CPP::Tree::Expr::Literal::kind_)>::Max(),\
			literal, suffixStr);\
		return literalEntityId;\
	}(text, suffix)

#define CPP__TREE__EXPR__CREATE_MEMBER_PTR_EXPR(type, member)\
	[this](ECS2::Entity::Id typeEntityId, const std::string& memberName){\
		ECS2::Entity::Id memberPtrExprEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(memberPtrExprEntityId);\
		CreateComponent<CPP::Tree::Expr::MemberPtrExpr>(memberPtrExprEntityId, typeEntityId, memberName);\
		return memberPtrExprEntityId;\
	}(type, member)

#define CPP__TREE__EXPR__CREATE_IDENTIFIER_EXPR(identifier)\
	[this](const std::string& identifierName){\
		ECS2::Entity::Id identifierExprEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(identifierExprEntityId);\
		CreateComponent<CPP::Tree::Expr::Identifier>(identifierExprEntityId, identifierName);\
		return identifierExprEntityId;\
	}(identifier)

#define CPP__TREE__EXPR__CREATE_UNARY_OPERATION(operation, operand)\
	[this](CPP::Tree::Expr::UnaryOpType operationType, ECS2::Entity::Id operantEntityId){\
		ECS2::Entity::Id unaryOpExprEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(unaryOpExprEntityId);\
		CreateComponent<CPP::Tree::Expr::UnaryOp>(unaryOpExprEntityId,operationType,operantEntityId);\
		return unaryOpExprEntityId;\
	}(operation, operand)

#define CPP__TREE__EXPR__CREATE_BINARY_OPERATION(operation, left, right)\
	[this](CPP::Tree::Expr::BinaryOpType operationType, ECS2::Entity::Id leftEntityId,ECS2::Entity::Id rightEntityId){\
		ECS2::Entity::Id binaryOpExprEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(binaryOpExprEntityId);\
		CreateComponent<CPP::Tree::Expr::BinaryOp>(binaryOpExprEntityId, operationType, leftEntityId, rightEntityId);\
		return binaryOpExprEntityId;\
	}(operation, left, right)

#define CPP__TREE__EXPR__CREATE_LAMBDA_COMPONENTS(lambda, captureTypeEnum, lambdaCaptures, parameters, body)\
	[this](	ECS2::Entity::Id lambdaEntityId, CPP::Tree::Expr::CaptureType captureType, std::vector<CPP::Tree::Expr::LambdaCapture> captures,\
			std::vector<ECS2::Entity::Id> parameterEntityIds,\
			ECS2::Entity::Id bodyEntityId){\
			CreateComponent<CPP::Tree::Expr::Tag>(lambdaEntityId);\
			CreateComponent<CPP::Tree::Expr::Lambda>(lambdaEntityId,\
				captureType,\
				captures,\
				parameterEntityIds,\
				bodyEntityId);\
	}(lambda,captureTypeEnum, lambdaCaptures, parameters, body)	

#define CPP__TREE__EXPR__CREATE_LAMBDA(captureTypeEnum, lambdaCaptures, parameters, body)\
	[this](CPP::Tree::Expr::CaptureType captureType, std::vector<CPP::Tree::Expr::LambdaCapture> captures,\
			std::vector<ECS2::Entity::Id> parameterEntityIds,\
			ECS2::Entity::Id bodyEntityId){\
		ECS2::Entity::Id lambdaEntityId = CreateEntity();\
		CPP__TREE__EXPR__CREATE_LAMBDA_COMPONENTS(lambdaEntityId, captureType, captures, parameterEntityIds, bodyEntityId);\
		return lambdaEntityId;\
	}(captureTypeEnum, lambdaCaptures, parameters, body)

#define CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR_COMPONENTS(expr, object, member, arrow)\
		[this](ECS2::Entity::Id exprEntityId, ECS2::Entity::Id objectEntityId, const std::string& memberName, bool isArrow){\
		CreateComponent<CPP::Tree::Expr::Tag>(exprEntityId);\
		CreateComponent<CPP::Tree::Expr::MemberAccess>(exprEntityId, objectEntityId, memberName, isArrow);\
	}(expr, object, member, arrow)

#define CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR(object, member, arrow)\
		[this](ECS2::Entity::Id objectEntityId, const std::string& memberName, bool isArrow){\
		ECS2::Entity::Id exprEntityId = CreateEntity();\
		CPP__TREE__EXPR__CREATE_MEMBER_ACCESS_EXPR_COMPONENTS(exprEntityId, objectEntityId, memberName, isArrow);\
		return exprEntityId;\
	}(object, member, arrow)

#define CPP__TREE__EXPR__CREATE_CALL_EXPR_COMPONENTS(callExpr, callee, templateArgs, args)\
	[this](\
		ECS2::Entity::Id callExprEntityId,\
		ECS2::Entity::Id calleeEntityId,\
		std::vector<ECS2::Entity::Id> templateArgumentEntityIds,\
		std::vector<ECS2::Entity::Id> argumentEntityIds){\
	CreateComponent<CPP::Tree::Expr::Tag>(callExprEntityId);\
	CreateComponent<CPP::Tree::Expr::CallExpr>(callExprEntityId,\
		calleeEntityId,\
		templateArgumentEntityIds,\
		argumentEntityIds);\
	}(callExpr, callee, templateArgs, args)

#define CPP__TREE__EXPR__CREATE_CALL_EXPR(callee, templateArgs, args)\
	[this](ECS2::Entity::Id calleeEntityId,\
		std::vector<ECS2::Entity::Id> templateArgumentEntityIds,\
		std::vector<ECS2::Entity::Id> argumentEntityIds){\
		ECS2::Entity::Id callExprEntityId = CreateEntity();\
		CPP__TREE__EXPR__CREATE_CALL_EXPR_COMPONENTS(\
			callExprEntityId,\
			calleeEntityId,\
			templateArgumentEntityIds, argumentEntityIds);\
		return callExprEntityId;\
	}(callee, templateArgs, args)

#define CPP__TREE__EXPR__CREATE_CALL_EXPR_NO_ARGS(callee)\
	[this](ECS2::Entity::Id calleeEntityId){\
		ECS2::Entity::Id callExprEntityId = CreateEntity();\
		CPP__TREE__EXPR__CREATE_CALL_EXPR_COMPONENTS(\
			callExprEntityId,\
			calleeEntityId,\
			std::vector<ECS2::Entity::Id>{}, std::vector<ECS2::Entity::Id>{});\
		return callExprEntityId;\
	}(callee)

#define CPP__TREE__EXPR__CREATE_ARRAY_SUBSCRIPT(array_, index)\
	[this](ECS2::Entity::Id arrayEntityId, ECS2::Entity::Id indexEntityId){\
		ECS2::Entity::Id arraySubscriptEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(arraySubscriptEntityId);\
		CreateComponent<CPP::Tree::Expr::ArraySubscript>(arraySubscriptEntityId, arrayEntityId, indexEntityId);\
		return arraySubscriptEntityId;\
	}(array_, index)

#define CPP__TREE__EXPR__CREATE_INIT_LIST(inits)\
	[this](std::vector<ECS2::Entity::Id> initEntityIds){\
		ECS2::Entity::Id initListEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Expr::Tag>(initListEntityId);\
		CreateComponent<CPP::Tree::Expr::InitList>(initListEntityId, initEntityIds);\
		return initListEntityId;\
	}(inits)

/*						FUNCTION					*/
#define CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(function, returnValue, templateParams, name, templateExplicitParams, parent, childs)\
	[this](\
		ECS2::Entity::Id functionEntityId,\
		ECS2::Entity::Id returnValueEntityId,\
		std::vector<ECS2::Entity::Id> templateParameters,\
		const std::string& funcName,\
		std::vector<ECS2::Entity::Id> templateExplicitParameters,\
		ECS2::Entity::Id parentEntityId,\
		std::vector<ECS2::Entity::Id> childEntityIds){\
			CreateComponent<CPP::Tree::Node::Tag>(functionEntityId);\
			CreateComponent<CPP::Tree::Decl::Tag>(functionEntityId);\
			CreateComponent<CPP::Tree::Decl::Function>(functionEntityId, returnValueEntityId);\
			CreateComponent<CPP::Tree::Node::Name>(functionEntityId, funcName);\
			if(!templateParameters.empty()){\
				CreateComponent<CPP::Tree::Decl::TemplateDecl>(functionEntityId, templateParameters);\
			}\
			if(!templateExplicitParameters.empty()){\
				CreateComponent<CPP::Tree::Decl::ExplicitTemplateArgs>(functionEntityId, templateExplicitParameters);\
			}\
			if(parentEntityId.IsValid()){\
				CreateComponent<CPP::Tree::Node::ParentEntityId>(functionEntityId, parentEntityId); \
			}\
			CreateComponent<CPP::Tree::Node::ChildEntityIds>(functionEntityId, childEntityIds);\
			return functionEntityId;\
		}(function, returnValue, templateParams, name,templateExplicitParams, parent, childs)

#define CPP__TREE__DECL__CREATE_CONSTRUCTOR(name, initializers, parent, childs)\
	[this](\
		const std::string& constructorName,\
		std::vector<ECS2::Entity::Id> initializerEntityIds,\
		ECS2::Entity::Id parentEntityId,\
		std::vector<ECS2::Entity::Id> childEntityId){\
		ECS2::Entity::Id constructorEntityId = CreateEntity();\
		CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(\
			constructorEntityId,\
			ECS2::Entity::Id::invalid_,\
			std::vector<ECS2::Entity::Id>{},\
			constructorName,\
			std::vector<ECS2::Entity::Id>{},\
			parentEntityId,\
			childEntityId\
			);\
		CreateComponent<CPP::Tree::Decl::Constructor>(constructorEntityId);\
		if(!initializerEntityIds.empty()) {\
			CreateComponent<CPP::Tree::ConstructorInitList>(constructorEntityId, initializerEntityIds);\
		}\
		return constructorEntityId;\
		}(name, initializers, parent, childs)

#define CPP__TREE__DECL__CREATE_EMPTY_CONSTRUCTOR(name, parent)\
	[this](const std::string& constructorName,ECS2::Entity::Id parentEntityId){\
		ECS2::Entity::Id constructorEntityId = CreateEntity();\
		CPP__TREE__DECL__CREATE_CONSTRUCTOR(\
			constructorName,\
			std::vector<ECS2::Entity::Id>{},\
			parentEntityId, std::vector<ECS2::Entity::Id>{CPP__TREE__STMT__CREATE_EMPTY_COMPOUND_STATEMENT()});\
		return constructorEntityId;\
		}(name, parent)

#define CPP__TREE__DECL__CREATE_PUBLIC_EMPTY_CONSTRUCTOR(name, parent)\
	[this](const std::string& constructorName,ECS2::Entity::Id parentEntityId){\
		ECS2::Entity::Id constructorEntityId = CPP__TREE__DECL__CREATE_CONSTRUCTOR(\
			constructorName,\
			std::vector<ECS2::Entity::Id>{},\
			parentEntityId, std::vector<ECS2::Entity::Id>{CPP__TREE__STMT__CREATE_EMPTY_COMPOUND_STATEMENT()});\
		CreateComponent<CPP::Tree::Access::Public_>(constructorEntityId);\
		return constructorEntityId;\
		}(name, parent)



#define CPP__TREE__CREATE_CONSTRUCTOR_INITIALIZER(member, expr)\
	[this](ECS2::Entity::Id memberEntityId, ECS2::Entity::Id exprEntityId){\
		ECS2::Entity::Id initializer = CreateEntity();\
		CreateComponent<CPP::Tree::ConstructorInitializer>(initializer,\
			memberEntityId,\
			exprEntityId \
		);\
		return initializer;\
	}(member, expr)

#define CPP__TREE__DECL__CREATE_FUNCTION(returnValue, name, parent, childs)\
	[this](ECS2::Entity::Id returnValueEntityId,\
		const std::string& funcName,\
		ECS2::Entity::Id parentEntityId,\
		std::vector<ECS2::Entity::Id> childEntityIds){\
		ECS2::Entity::Id functionEntityId = CreateEntity();\
		CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(\
			functionEntityId,\
			returnValueEntityId,\
			std::vector<ECS2::Entity::Id>{},\
			funcName,\
			std::vector<ECS2::Entity::Id>{},\
			parentEntityId,\
			childEntityIds\
			);\
		return functionEntityId;\
		}(returnValue, name, parent, childs)

#define CPP__TREE__DECL__CREATE_TEMPLATE_FUNCTION(returnValue, templateParams, name, templateExplicitParams, parent, childs)\
	[this](ECS2::Entity::Id returnValueEntityId,\
		std::vector<ECS2::Entity::Id> templateParams,\
		const std::string& funcName,\
		std::vector<ECS2::Entity::Id> templateExplicitParameters,\
		ECS2::Entity::Id parentEntityId,\
		std::vector<ECS2::Entity::Id> childEntityIds){\
		ECS2::Entity::Id functionEntityId = CreateEntity();\
		CPP__TREE__DECL__CREATE_FUNCTION_COMPONENTS(\
			functionEntityId,\
			returnValueEntityId,\
			templateParams,\
			funcName,\
			templateExplicitParameters,\
			parentEntityId,\
			childEntityIds\
			);\
		return functionEntityId;\
		}(returnValue, name, templateExplicitParams, parent, childs)


#define CPP__TREE__DECL__CREATE_PUBLIC_METHOD(returnValue, name, parent, childs)\
	[this](ECS2::Entity::Id returnValueEntityId,\
		const std::string& funcName,\
		ECS2::Entity::Id parentEntityId,\
		std::vector<ECS2::Entity::Id> childEntityIds){\
		ECS2::Entity::Id functionEntityId = CPP__TREE__DECL__CREATE_FUNCTION(\
			returnValueEntityId,\
			funcName,\
			parentEntityId,\
			childEntityIds);\
		CreateComponent<CPP::Tree::Access::Public_>(functionEntityId);\
		return functionEntityId;\
		}(returnValue, name, parent, childs)

#define CPP__TREE__DECL__CREATE_PUBLIC_STATIC_METHOD(returnValue, name, parent, childs)\
	[this](ECS2::Entity::Id returnValueEntityId,\
		const std::string& funcName,\
		ECS2::Entity::Id parentEntityId,\
		std::vector<ECS2::Entity::Id> childEntityIds){\
		ECS2::Entity::Id functionEntityId = CPP__TREE__DECL__CREATE_PUBLIC_METHOD(\
			returnValueEntityId, \
			funcName, \
			parentEntityId, \
			childEntityIds);\
		CreateComponent<CPP::Tree::StorageClass::Static_>(functionEntityId);\
		return functionEntityId;\
		}(returnValue, name, parent, childs)

#define CPP__TREE__DECL__CREATE_PUBLIC_STATIC_INLINE_METHOD(returnValue, name, parent, childs)\
	[this](ECS2::Entity::Id returnValueEntityId,\
		const std::string& funcName,\
		ECS2::Entity::Id parentEntityId,\
		std::vector<ECS2::Entity::Id> childEntityIds){\
		ECS2::Entity::Id functionEntityId = CPP__TREE__DECL__CREATE_PUBLIC_STATIC_METHOD(\
			returnValueEntityId, \
			funcName, \
			parentEntityId, \
			childEntityIds);\
		CreateComponent<CPP::Tree::Inline_>(functionEntityId);\
		return functionEntityId;\
		}(returnValue, name, parent, childs)

#define CPP__TREE__DECL__CREATE_USING_DIRECTIVE(namespaceName)\
	[this](std::string namespaceNameStr){\
		ECS2::Entity::Id usingDirectiveEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Decl::Tag>(usingDirectiveEntityId);\
		CreateComponent<CPP::Tree::Decl::UsingDirective>(usingDirectiveEntityId, namespaceNameStr);\
		return usingDirectiveEntityId;\
	}(namespaceName)

/*					TEMPLATE TYPE PARAMETER			*/
#define CPP__TREE__DECL__CREATE_TEMPLATE_TYPE_PARAMETER(typeName, defaultType, isParameterPack)\
	[this](const std::string& name, ECS2::Entity::Id defaultTypeEntityId, bool isPack){\
		ECS2::Entity::Id templateTypeParameterEntityId = CreateEntity();\
		CreateComponent<CPP::Tree::Decl::Tag>(templateTypeParameterEntityId);\
		CreateComponent<CPP::Tree::Decl::TemplateTypeParam>(templateTypeParameterEntityId, name, defaultTypeEntityId, isPack);\
		return templateTypeParameterEntityId;\
	}(typeName, defaultType, isParameterPack)

/*					CLASS DECLARATION					*/
#define CPP__TREE__DECL__CREATE_CLASS_COMPONENTS(classId, name, parent, childs)\
	[this](ECS2::Entity::Id classEntityId, const std::string& className, ECS2::Entity::Id parentEntityId, std::vector<ECS2::Entity::Id> childEntityIds) {\
		CreateComponent<CPP::Tree::Node::Tag>(classEntityId);\
		CreateComponent<CPP::Tree::Decl::Tag>(classEntityId);\
		CreateComponent<CPP::Tree::Decl::Class_>(classEntityId);\
		CreateComponent<CPP::Tree::Node::Name>(classEntityId, className);\
		if(parentEntityId.IsValid()){\
			CreateComponent<CPP::Tree::Node::ParentEntityId>(classEntityId, parentEntityId); \
		}\
		if(!childEntityIds.empty()){\
			CreateComponent<CPP::Tree::Node::ChildEntityIds>(classEntityId, childEntityIds); \
		}\
	}(classId, name, parent, childs)

#define CPP__TREE__DECL__CREATE_CLASS(name)\
	[this](const std::string& className){\
	}(name)


