#pragma once 

#include <Archetype/ECSGenerator2.ParsedArchetype.hpp>

namespace ECSGenerator2 {


	class ArchetypeCodeStructureGenerator {
	public:

		struct CreateInfo {
			std::filesystem::path includeDirectory_;
		};

		ArchetypeCodeStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}


		std::shared_ptr<CodeStructure::CodeBlock> GenerateArchetypeDefine(std::shared_ptr<ParsedArchetype> archetype) {

			CodeStructure::Code code;

			std::string archetypeName = archetype->ci_.name_;

			std::transform(archetypeName.begin(), archetypeName.end(), archetypeName.begin(),
				[](unsigned char c) { return std::toupper(c); });

			std::string componentsList;

			archetype->ForEachComponent([&](ParsedArchetype::Component& component, bool isLast) {
				componentsList += component.ptr_->GetFullName();
				if (!isLast) {
					componentsList += ", ";
				}
				});

			archetype->ForEachRefArchetype([&](ParsedArchetype::Archetype& refArchetype, bool isLast) {

				std::string refArchetypeName = refArchetype.ptr_->GetName();

				std::transform(refArchetypeName.begin(), refArchetypeName.end(), refArchetypeName.begin(),
					[](unsigned char c) { return std::toupper(c); });

				componentsList += refArchetypeName;
				if (!isLast) {
					componentsList += ", ";
				}
				});



			code.Add("#define {} {}", archetypeName, componentsList);

			auto constantVariable = std::make_shared<CodeStructure::CodeBlock>(code);

			return constantVariable;
		}

		CreateInfo ci_;
	};

}