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


	class ParsedStruct : public ParsedTable {
	public:

		struct FieldInfo {
			std::vector<std::string> namespace_;
			std::string typeName_;
			std::string name_;

			const std::string& GetName() const {
				return name_;
			}
			const std::string& GetTypeName() const {
				return typeName_;
			}
		};

		struct CreateInfo {
			std::string name_;
			std::vector<FieldInfo> fields_;
		};

		ParsedStruct(const CreateInfo& createInfo) :
			ci_{ createInfo } { }
            

		virtual ParsedTable::Type GetType() const noexcept override {
			return ParsedTable::Type::Struct;
		}

		bool AreThereFields() const {
			return !ci_.fields_.empty();
		}

		using ProcessField = std::function<bool(const FieldInfo& fieldInfo, bool isLast)>;

		void ForEachField(ProcessField&& processField) const {
			for (Common::Index i = 0; i < ci_.fields_.size(); i++) {
				const FieldInfo info = ci_.fields_[i];
				if (!processField(info, (i == ci_.fields_.size() - 1))) {
					break;
				}
			}
		}
        
		[[nodiscard]]
		std::string GetName() {
			return ci_.name_;
		}

		[[nodiscard]]
		std::string GetLowerName() {
			return std::string{ static_cast<char>(std::tolower(GetName()[0])) } + GetName().substr(1);
		}

		CreateInfo ci_;

	};


}