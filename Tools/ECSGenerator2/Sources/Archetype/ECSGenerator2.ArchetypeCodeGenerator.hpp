#pragma once 

#include <Archetype/ECSGenerator2.ParsedArchetype.hpp>

namespace ECSGenerator2 {


	class ArchetypeCodeStructureGenerator {
	public:

		struct CreateInfo {
			std::filesystem::path includeDirectory_;
		};

		ArchetypeCodeStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}


		std::vector<ParsedComponentPtr> GenerateArchetypeRequiredComponentIncludes(std::shared_ptr<ParsedArchetype> archetype) {

			std::vector<ParsedComponentPtr> requiredComponentIncludes;
			if (archetype->GetName() == "Static_Game_Object") {
				Common::BreakPointLine();
			}
			std::function<void(ParsedArchetype::Component&)> processComponent 
				= [&requiredComponentIncludes](ParsedArchetype::Component& component) {
					requiredComponentIncludes.push_back(Common::pointer_cast<ParsedComponent>(component.ptr_));
				};

			archetype->ForEachComponentRecursive(processComponent);

			return requiredComponentIncludes;
		}

		std::shared_ptr<CodeStructure::CodeBlock> GenerateArchetypeComponentsFilterConstant(std::shared_ptr<ParsedArchetype> archetype) {

			CodeStructure::Code code;

			std::string archetypeFullName = archetype->GetFullName();

			std::string defineComponentsList;

			archetype->ForEachComponent([&](ParsedArchetype::Component& component, bool isLast) {
				defineComponentsList += component.ptr_->GetFullName();
				if (!isLast || !archetype->ci_.archetypes_.empty()) {
					defineComponentsList += ", ";
				}
				});

			archetype->ForEachRefArchetype([&](ParsedArchetype::ChildArchetype& refArchetype, bool isLastArchetype) {

				std::string refArchetypeName = Common::pointer_cast<ParsedArchetype>(refArchetype.ptr_)->GetFullName();

				std::transform(refArchetypeName.begin(), refArchetypeName.end(), refArchetypeName.begin(),
					[](unsigned char c) { return std::toupper(c); });

				defineComponentsList += refArchetypeName;
				if (!isLastArchetype) {
					defineComponentsList += ", ";
				}

				});


			code.Add(
				"\nstatic const ::ECS2::ComponentsFilter {}ArchetypeComponentsFilter{{::ECS2::ComponentsFilter{{}}.SetBits<{}>()}};",
				archetype->GetName(), defineComponentsList);

			auto constantVariable = std::make_shared<CodeStructure::CodeBlock>(code);

			return constantVariable;
		}

		std::shared_ptr<CodeStructure::CodeBlock> GenerateArchetypeDefine(std::shared_ptr<ParsedArchetype> archetype) {

			CodeStructure::Code code;

			if (archetype->GetName() == "Static_Game_Object") {
				Common::BreakPointLine();
			}

			code.Add("// clang-format off\n");
			std::string archetypeFullName = archetype->GetFullName();

			std::transform(archetypeFullName.begin(), archetypeFullName.end(), archetypeFullName.begin(),
				[](unsigned char c) { return std::toupper(c); });

			std::string defineComponentsList;

			archetype->ForEachComponent([&](ParsedArchetype::Component& component, bool isLast) {
				defineComponentsList += component.ptr_->GetFullName();
				if (!isLast || !archetype->ci_.archetypes_.empty()) {
					defineComponentsList += ",\\\n";
				}
				});

			archetype->ForEachRefArchetype([&](ParsedArchetype::ChildArchetype& refArchetype, bool isLastArchetype) {

				std::string refArchetypeName = Common::pointer_cast<ParsedArchetype>(refArchetype.ptr_)->GetFullName();

				std::transform(refArchetypeName.begin(), refArchetypeName.end(), refArchetypeName.begin(),
					[](unsigned char c) { return std::toupper(c); });

				defineComponentsList += refArchetypeName;
				if (!isLastArchetype) {
					defineComponentsList += ",\\\n";
				}

				});

			code.Add("#define {} {}\n", archetypeFullName, defineComponentsList);
			code.Add("// clang-format on\n");

			auto constantVariable = std::make_shared<CodeStructure::CodeBlock>(code);

			return constantVariable;
		}

		CreateInfo ci_;
	};

}