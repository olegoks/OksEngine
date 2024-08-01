#pragma once 

#include <RAL.Common.hpp>

namespace RAL {

	//class BaseShader {
	//public:

	//	enum class Type {
	//		Vertex,
	//		Fragment,
	//		Undefined
	//	};

	//	struct CreateInfo {
	//		Type type_;
	//	};

	//	BaseShader(const CreateInfo& createInfo) : type_{ createInfo.type_ } { }

	//private:
	//	Type type_;
	//};


	//class TextShader : public BaseShader {
	//public:

	//	struct CreateInfo {
	//		BaseShader::Type type_ = BaseShader::Type::Undefined;
	//		std::string code_;

	//	};

	//	TextShader(const CreateInfo& createInfo) : BaseShader{ createInfo.type_ } {  }

	//private:

	//};

	//class BinaryShader {
	//public:
	//private:

	//};


	class Shader {
	public:

		Shader() noexcept = default;
		enum class Type {
			Vertex,
			Fragment,
			Undefined
		};
		struct CreateInfo {
			std::string name_;
			Type type_ = Type::Undefined;
			std::string code_;
		};

		Shader(const CreateInfo& createInfo) : createInfo_{ createInfo } {}

		Shader(const Shader& copyShader) {
			createInfo_ = copyShader.createInfo_;
		}

		[[nodiscard]]
		const std::string& GetCode() const noexcept {
			return createInfo_.code_;
		}

		[[nodiscard]]
		Type GetType() const noexcept { return createInfo_.type_; }

		virtual ~Shader() noexcept = default;
	private:
		CreateInfo createInfo_;
	};

}