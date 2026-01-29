#pragma once 

#include <Enum/ECSGenerator2.ParsedEnum.hpp>

namespace ECSGenerator2 {

	class EnumParser {
	public:
		std::shared_ptr<ParsedTable> Parse(
			luabridge::LuaRef enumRef,
			const std::string& enumName) {
			
			
			luabridge::LuaRef values = enumRef["values"];

			std::vector<ParsedEnum::Value> parsedValues;

			if (!values.isNil()) {
				for (luabridge::Iterator it(values); !it.isNil(); ++it) {
					luabridge::LuaRef valueRef = it.value();

					ParsedEnum::Value valueInfo{
						.name_ = valueRef.cast<std::string>().value()
					};

					parsedValues.push_back(valueInfo);
				}
			}
			ParsedEnum::CreateInfo ci{
				.name_ = enumName,
				.values_ = parsedValues
			};
			auto parsedEnumFile = std::make_shared<ParsedEnum>(ci);
			ASSERT_FMSG(std::isupper(parsedEnumFile->GetName()[0]), "First component name symbol must be uppercase.");
			return parsedEnumFile;
		}

	};

}