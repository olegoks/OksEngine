#pragma once 

#include <Enum/ECSGenerator2.ParsedEnum.hpp>

namespace ECSGenerator2 {


	class EnumCodeStructureGenerator {
	public:

		struct CreateInfo {
			std::filesystem::path includeDirectory_;
		};

		EnumCodeStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		std::shared_ptr<CodeStructure::Enum> GenerateEnumCodeStructure(std::shared_ptr<ParsedEnum> parsedEnum) {

			std::vector<std::string> values;

			parsedEnum->ForEachValue([&](const ParsedEnum::Value& value, bool isLast)->bool {

				values.push_back(value.name_);

				return true;
				});

			CodeStructure::Enum::CreateInfo sci{
				.name_ = parsedEnum->GetName(),
				.values_ = values
			};
			auto enumObject = std::make_shared<CodeStructure::Enum>(sci);

			return enumObject;
		}

		CreateInfo ci_;
	};

}