#pragma once 

#include <Component/ECSGenerator2.ParsedComponent.hpp>

namespace ECSGenerator2 {

	class ComponentParser {
	public:
		std::shared_ptr<ParsedTable> Parse(
			luabridge::LuaRef component,
			const std::string& componentName) {

			//Serializable
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

			//ManualEditFunction
			bool manualEditFunction = false;
			luabridge::LuaRef manualEditFunctionRef = component["manualEditFunction"];
			if (!manualEditFunctionRef.isNil()) {
				manualEditFunction = manualEditFunctionRef.cast<bool>().value();
			}

			//ManualBindFunction
			bool manualBindFunction = false;
			luabridge::LuaRef manualBindFunctionRef = component["manualBindFunction"];
			if (!manualBindFunctionRef.isNil()) {
				manualBindFunction = manualBindFunctionRef.cast<bool>().value();
			}

			//ManualAddFunction
			bool manualAddFunction = false;
			luabridge::LuaRef manualAddFunctionRef = component["manualAddFunction"];
			if (!manualAddFunctionRef.isNil()) {
				manualAddFunction = manualAddFunctionRef.cast<bool>().value();
			}

			//ManualParseFunction
			bool manualParseFunction = false;
			luabridge::LuaRef manualParseFunctionRef = component["manualParseFunction"];
			if (!manualParseFunctionRef.isNil()) {
				manualParseFunction = manualParseFunctionRef.cast<bool>().value();
			}

			//ManualSerializeFunction
			bool manualSerializeFunction = false;
			luabridge::LuaRef manualSerializeFunctionRef = component["manualSerializeFunction"];
			if (!manualSerializeFunctionRef.isNil()) {
				manualSerializeFunction = manualSerializeFunctionRef.cast<bool>().value();
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

					if (!field["alignment"].isNil()) {
						fieldInfo.alignment_ = field["alignment"].cast<Common::Size>().value();
					}

					if (!field["copyable"].isNil()) {
						fieldInfo.copyable_ = field["copyable"].cast<bool>().value();
					}

					parsedFields.push_back(fieldInfo);
				}
			}
			ParsedComponent::CreateInfo ci{
				.name_ = componentName,
				.serializable_ = serializable,
				.manualEditFunction_ = manualEditFunction,
				.manualBindFunction_ = manualBindFunction,
				.manualAddFunction_ = manualAddFunction,
				.manualParseFunction_ = manualParseFunction,
				.manualSerializeFunction_ = manualSerializeFunction,
				.alignment_ = alignment,
				.fields_ = parsedFields
			};
			auto parsedComponentFile = std::make_shared<ParsedComponent>(ci);
			ASSERT_FMSG(std::isupper(parsedComponentFile->GetName()[0]), "First component name symbol must be uppercase.");
			return parsedComponentFile;
		}

	};

}