#pragma once 

#include <Constant/ECSGenerator2.ParsedConstant.hpp>

namespace ECSGenerator2 {


	class ConstantCodeStructureGenerator {
	public:

		struct CreateInfo {
			std::filesystem::path includeDirectory_;
		};

		ConstantCodeStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}


		std::shared_ptr<CodeStructure::Variable> GenerateConstantVariable(std::shared_ptr<ParsedConstant> constant) {

			CodeStructure::Variable::CreateInfo vci{
				.type_ = "constexpr static inline " + constant->ci_.typeName_,
				.name_ = constant->ci_.name_,
				.initValue_ = constant->ci_.value_
			};

			auto constantVariable = std::make_shared<CodeStructure::Variable>(vci);

			return constantVariable;
		}

		CreateInfo ci_;
	};

}