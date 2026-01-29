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


	class ParsedEnum : public ParsedTable {
	public:

		struct Value {
			std::string name_;
		};

		struct CreateInfo {
			std::string name_;
			std::vector<Value> values_;
		};

		ParsedEnum(const CreateInfo& createInfo) :
			ci_{ createInfo } { }
            

		virtual ParsedTable::Type GetType() const noexcept override {
			return ParsedTable::Type::Enum;
		}


		virtual std::shared_ptr<ParsedTable> Clone() const noexcept override {
			return std::make_shared<ParsedEnum>(*this);
		}

		using ProcessValue = std::function<bool(const Value& valueInfo, bool isLast)>;

		void ForEachValue(ProcessValue&& processValue) const {
			for (Common::Index i = 0; i < ci_.values_.size(); i++) {
				const Value value = ci_.values_[i];
				if (!processValue(value, (i == ci_.values_.size() - 1))) {
					break;
				}
			}
		}
        
		[[nodiscard]]
		const std::string& GetName() const noexcept override {
			return ci_.name_;
		}

		[[nodiscard]]
		std::string GetLowerName() {
			return std::string{ static_cast<char>(std::tolower(GetName()[0])) } + GetName().substr(1);
		}

		CreateInfo ci_;

	};


}