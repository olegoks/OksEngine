#pragma once

#include <ECSGenerator2.Common.hpp>
#include <ECSGenerator2.Code.hpp>

namespace ECSGenerator2::CodeStructure {


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