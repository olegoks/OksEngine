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

			archetype->ForEachComponent([&](ParsedArchetype::Component& component, bool isLast) {
				requiredComponentIncludes.push_back(std::dynamic_pointer_cast<ParsedComponent>(component.ptr_));
				});

			archetype->ForEachRefArchetype([&](ParsedArchetype::ChildArchetype& refArchetype, bool isLastArchetype) {

				std::dynamic_pointer_cast<ParsedArchetype>(refArchetype.ptr_)->ForEachComponent(
					[&](ParsedArchetype::Component& component, bool isLastComponent) {

						requiredComponentIncludes.push_back(std::dynamic_pointer_cast<ParsedComponent>(component.ptr_));

					});

				});

			return requiredComponentIncludes;
		}


		std::shared_ptr<CodeStructure::CodeBlock> GenerateArchetypeDefine(std::shared_ptr<ParsedArchetype> archetype) {

			CodeStructure::Code code;

			if (archetype->GetName() == "Node_Bone_Animated") {
				Common::BreakPointLine();
			}

			std::string archetypeName = archetype->ci_.name_;

			std::transform(archetypeName.begin(), archetypeName.end(), archetypeName.begin(),
				[](unsigned char c) { return std::toupper(c); });

			std::string archetypeComponentsFilterComponentsList;
			std::string defineComponentsList;

			archetype->ForEachComponent([&](ParsedArchetype::Component& component, bool isLast) {
				defineComponentsList += component.ptr_->GetFullName();
				archetypeComponentsFilterComponentsList += component.ptr_->GetFullName();
				if (!isLast || !archetype->ci_.archetypes_.empty()) {
					defineComponentsList += ", ";
					archetypeComponentsFilterComponentsList += ", ";
				}
				});

			archetype->ForEachRefArchetype([&](ParsedArchetype::ChildArchetype& refArchetype, bool isLastArchetype) {

				std::string refArchetypeName = refArchetype.ptr_->GetName();

				std::transform(refArchetypeName.begin(), refArchetypeName.end(), refArchetypeName.begin(),
					[](unsigned char c) { return std::toupper(c); });

				defineComponentsList += refArchetypeName;
				if (!isLastArchetype) {
					defineComponentsList += ", ";
				}

				std::dynamic_pointer_cast<ParsedArchetype>(refArchetype.ptr_)->ForEachComponent(
					[&](ParsedArchetype::Component& component, bool isLastComponent) {
					
						archetypeComponentsFilterComponentsList += component.ptr_->GetFullName();
						if (!isLastArchetype || !isLastComponent) {
							archetypeComponentsFilterComponentsList += ", ";
						}
					
					});

				});

			code.Add("#define {} {}\n", archetypeName, defineComponentsList);

			code.Add(
				"\nstatic const ::ECS2::ComponentsFilter {}ArchetypeComponentsFilter{{::ECS2::ComponentsFilter{{}}.SetBits<{}>()}};", 
				archetype->GetName(), archetypeComponentsFilterComponentsList);


			auto constantVariable = std::make_shared<CodeStructure::CodeBlock>(code);

			return constantVariable;
		}

		CreateInfo ci_;
	};

}