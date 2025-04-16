#pragma once

#include <ECSGenerator.Common.hpp>
#include <ECSGenerator.Code.hpp>

namespace ECSGenerator {


	class CodeFile {
	public:

		void AddCode(const std::string& code) {
			code_.Add(code);
		}

		void AddCode(const Code& code) {
			code_.Add(code);
		}

		std::filesystem::path path_;
		Code code_;
	};


}