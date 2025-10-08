#pragma once 

#include <ECSGenerator2.Common.hpp>
#include <ECSGenerator2.CodeFile.hpp>

#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <random>  
#include <format>

#include <ECSGenerator2.ParsedTable.hpp>

namespace ECSGenerator2 {

	class ParsedConstant : public ParsedTable {
	public:

		struct CreateInfo {
			std::string typeName_ = "";
			std::string name_ = "";
			std::string value_ = "";
		};

		ParsedConstant(const CreateInfo& createInfo) :
			ci_{ createInfo } { }

		virtual ParsedTable::Type GetType() const noexcept override {
			return ParsedTable::Type::Constant;
		}

		virtual std::shared_ptr<ParsedTable> Clone() const noexcept override {
			return std::make_shared<ParsedConstant>(*this);
		}

		[[nodiscard]]
		virtual const std::string& GetName() const noexcept override {
			return ci_.name_;
		}

		[[nodiscard]]
		std::string GetLowerName() {
			return std::string{ static_cast<char>(std::tolower(GetName()[0])) } + GetName().substr(1);
		}

		CreateInfo ci_;

	};

	using ParsedConstantPtr = std::shared_ptr<ParsedConstant>;
}