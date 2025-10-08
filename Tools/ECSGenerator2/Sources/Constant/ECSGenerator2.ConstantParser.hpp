#pragma once 

#include <Constant/ECSGenerator2.ParsedConstant.hpp>

namespace ECSGenerator2 {

	class ConstantParser {
	public:
		std::shared_ptr<ParsedTable> Parse(
			luabridge::LuaRef constant,
			const std::string& constantName) {

			luabridge::LuaRef type = constant["type"];
			luabridge::LuaRef value = constant["value"];

			ParsedConstant::CreateInfo ci{
				.typeName_ = type.cast<std::string>().value(),
				.name_ = constantName,
				.value_ = value.cast<std::string>().value()
			};
			auto parsedConstant = std::make_shared<ParsedConstant>(ci);
			ASSERT_FMSG(std::isupper(parsedConstant->GetName()[0]),
				"First constant name symbol must be uppercase.");
			
			return parsedConstant;
		}

	};

}