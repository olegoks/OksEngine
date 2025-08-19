#pragma once

#include <string>
#include <vector>
#include <memory>
#include <format>
#include <unordered_set>
#include <OS.Assert.hpp>

#include <Common.StringLiterals.hpp>

namespace ECSGenerator2::CodeStructure {

	class Code {
	public:

		Code() {
			using namespace Common;
			code_.reserve(1_KB);
		}

		Code(const std::string& code)
			: code_{ code } {

		}

		//Code& Add(const std::string& code) {
		//	code_ += code;
		//	return *this;
		//}

		//template<class ...Args>
		//Code& Add(const std::string& format, Args... args) {
		//	code_ += std::format(format, args...);
		//	return *this;
		//}

		//template<class ...Args>
		//Code& Add(const char* formatString, Args... args) {
		//	code_ += std::vformat(formatString, std::make_format_args(args...));
		//	return *this;
		//}

		template<class ...Args>
		Code& Add(const std::string& formatString, Args... args) {
			code_ += std::vformat(formatString, std::make_format_args(args...));
			return *this;
		}

		template<>
		Code& Add<>(const std::string& formatString) {
			code_ += formatString;//std::format(formatString);
			return *this;
		}

		Code& AddComment(const std::string& text) {
			code_ += "//" + text + "\n";
			return *this;
		}

		Code& Add(const Code& code) {
			code_ += code.code_;
			return *this;
		}
		Code& Comma() {
			code_ += ',';
			return *this;
		}

		Code& NewLine() {
			code_ += '\n';
			return *this;
		}

		Code& ApplyTab() {
			for (Common::Index i = 0; i < code_.length(); i++) {
				const char current = code_[i];
				if (current == '\n' && i != code_.length() - 1) {
					code_.insert(i + 1, "\t");
					++i;
				}
			}
			code_.insert(0, "\t");
			return *this;
		}

		std::string code_;
	};

	class Base {
	public:

		enum class Type {
			Function,
			Namespace,
			Struct,
			Scope,
			File,
			Variable,
			Undefined
		};

		virtual Type GetType() const noexcept = 0;

	private:
	};


	class Scope : public Base {
	public:
		void AddCode(const std::string& code) {
			code_.push_back(code);
		}

		std::string GenerateCode() {

		}

		std::vector<std::string> code_;

	};

	class Function : public Base {
	public:

		struct Parameter {
			std::string inputType_;
			std::string valueName_;

		};

		struct CreateInfo {
			std::string name_;
			std::vector<Parameter> parameters_;
			std::string returnType_;
			Code code_;
			bool isPrototype_ = false;
			bool inlineModifier_ = false;
			bool staticModifier_ = false;
			std::vector<std::string> templateParameters_;
			std::vector<std::string> specializedTemplateParameters_;
		};

		Function(const CreateInfo& ci) : ci_{ ci } {}

		virtual Type GetType() const noexcept override {
			return Type::Function;
		}

		CreateInfo ci_;
	};

	class Variable : public Base {
	public:

		struct CreateInfo {
			std::string type_;
			std::string name_;
			std::string initValue_;
		};
		Variable(const CreateInfo& createInfo) 
			: ci_{ createInfo } { }

		virtual Type GetType() const noexcept override {
			return Type::Variable;
		}

		CreateInfo ci_;
	};

	class Struct : public Base {
	public:

		struct Field {
			std::string type_;
			std::string name_;
			bool copyable_ = true;
		};

		struct CreateInfo {
			std::string name_;
			std::string parent_;
			std::vector<Field> fields_;
			std::vector<std::shared_ptr<Function>> methods_;
			bool defaultConstructor_ = true;
			bool constructor_ = true;
			bool forwardDeclaration_ = false;
		};

		Struct(const CreateInfo& createInfo)
			: ci_{ createInfo } {}

		void AddFunctionPrototype(const Function& function) {

		}

		const std::string& GetParentName() const noexcept {
			return ci_.parent_;
		}

		bool HasParent() const noexcept {
			return !ci_.parent_.empty();
		}

		std::string GetName() const noexcept {
			return ci_.name_;
		}

		virtual Type GetType() const noexcept override {
			return Type::Struct;
		}

		using ProcessField = std::function<void(const Field& field, bool isLast)>;

		bool AreThereFields() const noexcept {
			return !ci_.fields_.empty();
		}

		void ForEachField(ProcessField&& processField) {
			for (Common::Index i = 0; i < ci_.fields_.size(); i++) {
				processField(ci_.fields_[i], i == ci_.fields_.size() - 1);
			}
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



				m3dnormalizeSafe

			code += "};\n";*/
		}

		CreateInfo ci_;
	};

	class Namespace : public Base {
	public:

		Namespace(const std::string& name)
			: name_{ name } {  }

		void Add(std::shared_ptr<Base> base) {
			base_.push_back(base);
		}

		const std::string& GetName() const noexcept {
			return name_;
		}

		virtual Type GetType() const noexcept override {
			return Type::Namespace;
		}

		std::vector<std::shared_ptr<Base>> base_;
		std::string name_;
	};


	class File : public Base {
	public:

		struct Includes {
			std::unordered_set<std::filesystem::path> paths_;
		};

		struct CreateInfo {
			bool isHpp_ = true;
			Includes includes_;
			std::shared_ptr<Base> base_;
		};

		virtual Type GetType() const noexcept override {
			return Type::File;
		}

		void ForEachInclude(std::function<void(const std::filesystem::path&)>&& processInclude) const noexcept {
			for (const auto& include : createInfo_.includes_.paths_) {
				processInclude(include);
			}
		}

		[[nodiscard]]
		bool IsHpp() const noexcept {
			return createInfo_.isHpp_;
		}

		const std::shared_ptr<Base> GetBase() const noexcept {
			return createInfo_.base_;
		}

		File(const CreateInfo& createInfo) :
			createInfo_{ createInfo } {}

		CreateInfo createInfo_;
	};

}