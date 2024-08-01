#pragma once 

#include <RAL.Shader.hpp>

namespace RAL {

	class Compiler {
	public:

		virtual std::shared_ptr<RAL::Shader> Compile(const std::string& shaderCode, RAL::Shader::Type shaderType) = 0;

	private:

	};

}

