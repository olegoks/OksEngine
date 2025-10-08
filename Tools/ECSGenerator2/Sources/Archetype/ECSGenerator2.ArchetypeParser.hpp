#pragma once 

#include <Archetype/ECSGenerator2.ParsedArchetype.hpp>

namespace ECSGenerator2 {

	class ArchetypeParser {
	public:
		std::shared_ptr<ParsedTable> Parse(
			luabridge::LuaRef archetype,
			const std::string& archetypeName) {

			luabridge::LuaRef components = archetype["components"];
			std::vector<ParsedArchetype::Component> archetypeComponents;
			if (!components.isNil()) {
				for (luabridge::Iterator itJ(components); !itJ.isNil(); ++itJ) {
					luabridge::LuaRef componentRef = itJ.value();
					archetypeComponents.push_back({ nullptr, componentRef.cast<std::string>().value() });
				}
			}

			luabridge::LuaRef archetypes = archetype["archetypes"];
			std::vector<ParsedArchetype::Archetype> archetypeRefArchetypes;
			if (!archetypes.isNil()) {
				for (luabridge::Iterator itJ(archetypes); !itJ.isNil(); ++itJ) {
					luabridge::LuaRef archetypeRef = itJ.value();
					archetypeRefArchetypes.push_back(
						{
							nullptr,
							archetypeRef.cast<std::string>().value()
						});
				}
			}

			ASSERT(!archetypeComponents.empty() || !archetypeRefArchetypes.empty());

			ParsedArchetype::CreateInfo ci{
				.name_ = archetypeName,
				.components_ = archetypeComponents,
				.archetypes_ = archetypeRefArchetypes
			};
			auto parsedArchetype = std::make_shared<ParsedArchetype>(ci);
			ASSERT_FMSG(std::isupper(parsedArchetype->GetName()[0]),
				"First archetype name symbol must be uppercase.");

			return parsedArchetype;
		}

	};

}