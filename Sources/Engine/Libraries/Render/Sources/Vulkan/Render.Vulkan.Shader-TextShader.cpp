#pragma once

#include <vector>
#include <iostream>
//#include <shaderc\shaderc.hpp>
#include <Render.Vulkan.Shader-TextShader.hpp>


namespace Render::Vulkan
{

	void TextShader::Compile() {
		//const char kShaderSource[] =
		//	"#version 310 es\n"
		//	"void main() { int x = MY_DEFINE; }\n";

		//const std::string source_name = "";
		//const std::string&  source = kShaderSource;
		//shaderc_shader_kind kind = shaderc_glsl_vertex_shader;
		//	shaderc::Compiler compiler;
		//	shaderc::CompileOptions options;
		//	bool optimize = false;
		//	// Like -DMY_DEFINE=1
		//	options.AddMacroDefinition("MY_DEFINE", "1");
		//	if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_size);

		//	shaderc::SpvCompilationResult module =
		//		compiler.CompileGlslToSpv(source, kind, source_name.c_str(), options);

		//	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		//		std::cerr << module.GetErrorMessage();
		//		//return std::vector<uint32_t>();
		//	}
	}
}