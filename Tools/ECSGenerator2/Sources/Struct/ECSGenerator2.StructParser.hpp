#pragma once 

#include <Struct/ECSGenerator2.ParsedStruct.hpp>

namespace ECSGenerator2 {

	class StructParser {
	public:
		std::shared_ptr<ParsedTable> Parse(
			luabridge::LuaRef component,
			const std::string& componentName) {

			bool serializable = true;
			luabridge::LuaRef serializableRef = component["serializable"];
			if (!serializableRef.isNil()) {
				serializable = serializableRef.cast<bool>().value();
			}
			luabridge::LuaRef fields = component["fields"];

			if (componentName == "ECSMenu") {
				Common::BreakPointLine();
			}

			std::vector<ParsedStruct::FieldInfo> parsedFields;

			if (!fields.isNil()) {
				for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
					luabridge::LuaRef field = it.value();
					ParsedStruct::FieldInfo fieldInfo{
						.typeName_ = field["type"].cast<std::string>().value(),
						.name_ = field["name"].cast<std::string>().value()
					};
					parsedFields.push_back(fieldInfo);
				}
			}
			ParsedStruct::CreateInfo ci{
				.name_ = componentName,
				.fields_ = parsedFields
			};
			auto parsedComponentFile = std::make_shared<ParsedStruct>(ci);
			ASSERT_FMSG(std::isupper(parsedComponentFile->GetName()[0]), "First component name symbol must be uppercase.");
			return parsedComponentFile;
		}

	};

}