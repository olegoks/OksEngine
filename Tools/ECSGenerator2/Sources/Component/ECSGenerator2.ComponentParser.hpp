#pragma once 

#include <Component/ECSGenerator2.ParsedComponent.hpp>

namespace ECSGenerator2 {

	class ComponentParser {
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

			std::vector<ParsedComponent::FieldInfo> parsedFields;

			if (!fields.isNil()) {
				for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
					luabridge::LuaRef field = it.value();
					ParsedComponent::FieldInfo fieldInfo{
						.typeName_ = field["type"].cast<std::string>().value(),
						.name_ = field["name"].cast<std::string>().value()
					};

					if (!field["copyable"].isNil()) {
						fieldInfo.copyable_ = field["copyable"].cast<bool>().value();
					}

					parsedFields.push_back(fieldInfo);
				}
			}
			ParsedComponent::CreateInfo ci{
				.name_ = componentName,
				.serializable_ = serializable,
				.fields_ = parsedFields
			};
			auto parsedComponentFile = std::make_shared<ParsedComponent>(ci);
			OS::AssertMessage(std::isupper(parsedComponentFile->GetName()[0]), "First component name symbol must be uppercase.");
			return parsedComponentFile;
		}

	};

}