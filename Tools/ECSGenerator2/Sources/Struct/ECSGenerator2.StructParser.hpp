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
			
			//Alignment
			Common::Size alignment = Common::Limits<Common::Size>::Max();
			luabridge::LuaRef alignmentRef = component["alignment"];
			if (!alignmentRef.isNil()) {
				alignment = alignmentRef.cast<Common::Size>().value();
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

					if (!field["defaultValue"].isNil()) {
						fieldInfo.defaultValue_ = field["defaultValue"].cast<std::string>().value();
					}

					if (!field["alignment"].isNil()) {
						fieldInfo.alignment_ = field["alignment"].cast<Common::Size>().value();
					}


					parsedFields.push_back(fieldInfo);
				}
			}
			ParsedStruct::CreateInfo ci{
				.name_ = componentName,
				.alignment_ = alignment,
				.fields_ = parsedFields
			};
			auto parsedComponentFile = std::make_shared<ParsedStruct>(ci);
			ASSERT_FMSG(std::isupper(parsedComponentFile->GetName()[0]), "First component name symbol must be uppercase.");
			return parsedComponentFile;
		}

	};

}