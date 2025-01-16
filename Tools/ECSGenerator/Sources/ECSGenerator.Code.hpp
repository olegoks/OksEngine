#pragma once

#include <string>
#include <vector>
#include <memory>
#include <OS.Assert.hpp>

namespace ECSGenerator {

	class Function {
	public:

		struct Parameter {

			//enum class InputType {
			//	Pointer,
			//	ConstPointer,
			//	Reference,
			//	ConstReference,
			//	Value,
			//	Undefined
			//};
			std::string inputType_;
			std::string valueName_;

		};

		struct CreateInfo {
			std::string name_;
			std::vector<Parameter> parameters_;
			std::string returnType_;
			std::string code_;
		};

		std::string GeneratePrototype() {
			std::string code;
			code += ci_.returnType_ + " " + ci_.name_ + "(";
			for (Common::Index i = 0; i < ci_.parameters_.size(); i++) {
				const Parameter& paramter = ci_.parameters_[i];
				code += paramter.inputType_ + " " + paramter.valueName_;
				if (i < ci_.parameters_.size() - 1) {
					code += ", ";
				}
			}
			code += ");\n";
		}

		std::string GenerateRealization() {
			std::string code;
			code += ci_.returnType_ + " " + ci_.name_ + "(";
			for (Common::Index i = 0; i < ci_.parameters_.size(); i++) {
				const Parameter& paramter = ci_.parameters_[i];
				code += paramter.inputType_ + " " + paramter.valueName_;
				if (i < ci_.parameters_.size() - 1) {
					code += ", ";
				}
			}
			code += "){\n";
			code += ci_.code_;
			code += "};\n";

		}

		CreateInfo ci_;
	};

	class Struct {
	public:

		struct Field {
			std::string type_;
			std::string name_;
		};

		struct CreateInfo {
			std::string name_;
			std::shared_ptr<Struct> parent_;
			std::vector<Field> fields_;
			bool generateDefaultConstructor_ = true;
			bool generateConstructor_ = true;
		};

		Struct(const CreateInfo& createInfo)
			: ci_{ createInfo } {}

		void AddFunctionPrototype(const Function& function) {

		}

		std::string GenerateCode() {
			//std::string code;
			//
			//code += "struct " + ci_.name_;
			//if (ci_.parent_ != nullptr) {
			//	OS::NotImplemented();
			//}

			////Default constructor.
			//code += "\t" + ci_.name_ + "()\n\t {}\n";

			////Main constructor.
			//if (!ci_.fields_.empty()) {
			//	//Main constructor.
			//	code += "\t" + component.GetName() + "(";
			//	component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
			//		code += field.GetTypeName() + " " + field.GetName();
			//		if (!isLast) {
			//			code += ", ";
			//		}
			//		return true;
			//		});
			//	code += ") : \n";
			//	code += "\t\tECSComponent{ nullptr }, \n";

			//	component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
			//		code += "\t\t" + field.GetName() + "_{ " + field.GetName() + " }";
			//		if (!isLast) {
			//			code += ",\n";
			//		}
			//		return true;
			//		});
			//	code += "{ }\n";

			/*}

			component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
				code += "\t" + field.GetTypeName() + " " + field.GetName() + "_;\n";
				return true;
				});


			code += "};\n";*/
		}

		CreateInfo ci_;
	};


	class File {
	public:

		void AddPragmaOnce() {
			addPragmaOnce_ = true;
		}

		void IncludeHeader(const std::string& name) {
			headers_.push_back(name);
		}
		void AddNamespace(const std::string& name) {
			namespaces_.push_back(name);
		}
		bool addPragmaOnce_ = false;
		std::vector<std::string> headers_;
		std::vector<std::string> namespaces_;
	};

}