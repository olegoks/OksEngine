#pragma once 

#include <RAL.Driver.hpp>

#include <glslang/Include/glslang_c_interface.h>


#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

namespace Render::Vulkan {

	inline void initResources(TBuiltInResource& Resources) {
		Resources.maxLights = 32;
		Resources.maxClipPlanes = 6;
		Resources.maxTextureUnits = 32;
		Resources.maxTextureCoords = 32;
		Resources.maxVertexAttribs = 64;
		Resources.maxVertexUniformComponents = 4096;
		Resources.maxVaryingFloats = 64;
		Resources.maxVertexTextureImageUnits = 32;
		Resources.maxCombinedTextureImageUnits = 80;
		Resources.maxTextureImageUnits = 32;
		Resources.maxFragmentUniformComponents = 4096;
		Resources.maxDrawBuffers = 32;
		Resources.maxVertexUniformVectors = 128;
		Resources.maxVaryingVectors = 8;
		Resources.maxFragmentUniformVectors = 16;
		Resources.maxVertexOutputVectors = 16;
		Resources.maxFragmentInputVectors = 15;
		Resources.minProgramTexelOffset = -8;
		Resources.maxProgramTexelOffset = 7;
		Resources.maxClipDistances = 8;
		Resources.maxComputeWorkGroupCountX = 65535;
		Resources.maxComputeWorkGroupCountY = 65535;
		Resources.maxComputeWorkGroupCountZ = 65535;
		Resources.maxComputeWorkGroupSizeX = 1024;
		Resources.maxComputeWorkGroupSizeY = 1024;
		Resources.maxComputeWorkGroupSizeZ = 64;
		Resources.maxComputeUniformComponents = 1024;
		Resources.maxComputeTextureImageUnits = 16;
		Resources.maxComputeImageUniforms = 8;
		Resources.maxComputeAtomicCounters = 8;
		Resources.maxComputeAtomicCounterBuffers = 1;
		Resources.maxVaryingComponents = 60;
		Resources.maxVertexOutputComponents = 64;
		Resources.maxGeometryInputComponents = 64;
		Resources.maxGeometryOutputComponents = 128;
		Resources.maxFragmentInputComponents = 128;
		Resources.maxImageUnits = 8;
		Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
		Resources.maxImageSamples = 0;
		Resources.maxVertexImageUniforms = 0;
		Resources.maxTessControlImageUniforms = 0;
		Resources.maxTessEvaluationImageUniforms = 0;
		Resources.maxGeometryImageUniforms = 0;
		Resources.maxFragmentImageUniforms = 8;
		Resources.maxCombinedImageUniforms = 8;
		Resources.maxGeometryTextureImageUnits = 16;
		Resources.maxGeometryOutputVertices = 256;
		Resources.maxGeometryTotalOutputComponents = 1024;
		Resources.maxGeometryUniformComponents = 1024;
		Resources.maxGeometryVaryingComponents = 64;
		Resources.maxTessControlInputComponents = 128;
		Resources.maxTessControlOutputComponents = 128;
		Resources.maxTessControlTextureImageUnits = 16;
		Resources.maxTessControlUniformComponents = 1024;
		Resources.maxTessControlTotalOutputComponents = 4096;
		Resources.maxTessEvaluationInputComponents = 128;
		Resources.maxTessEvaluationOutputComponents = 128;
		Resources.maxTessEvaluationTextureImageUnits = 16;
		Resources.maxTessEvaluationUniformComponents = 1024;
		Resources.maxTessPatchComponents = 120;
		Resources.maxPatchVertices = 32;
		Resources.maxTessGenLevel = 64;
		Resources.maxViewports = 16;
		Resources.maxVertexAtomicCounters = 0;
		Resources.maxTessControlAtomicCounters = 0;
		Resources.maxTessEvaluationAtomicCounters = 0;
		Resources.maxGeometryAtomicCounters = 0;
		Resources.maxFragmentAtomicCounters = 8;
		Resources.maxCombinedAtomicCounters = 8;
		Resources.maxAtomicCounterBindings = 1;
		Resources.maxVertexAtomicCounterBuffers = 0;
		Resources.maxTessControlAtomicCounterBuffers = 0;
		Resources.maxTessEvaluationAtomicCounterBuffers = 0;
		Resources.maxGeometryAtomicCounterBuffers = 0;
		Resources.maxFragmentAtomicCounterBuffers = 1;
		Resources.maxCombinedAtomicCounterBuffers = 1;
		Resources.maxAtomicCounterBufferSize = 16384;
		Resources.maxTransformFeedbackBuffers = 4;
		Resources.maxTransformFeedbackInterleavedComponents = 64;
		Resources.maxCullDistances = 8;
		Resources.maxCombinedClipAndCullDistances = 8;
		Resources.maxSamples = 4;
		Resources.limits.nonInductiveForLoops = 1;
		Resources.limits.whileLoops = 1;
		Resources.limits.doWhileLoops = 1;
		Resources.limits.generalUniformIndexing = 1;
		Resources.limits.generalAttributeMatrixVectorIndexing = 1;
		Resources.limits.generalVaryingIndexing = 1;
		Resources.limits.generalSamplerIndexing = 1;
		Resources.limits.generalVariableIndexing = 1;
		Resources.limits.generalConstantMatrixVectorIndexing = 1;
	}



	class Includer : public glslang::TShader::Includer {
	public:

		Includer(const std::vector<std::string>& includePaths)
			: includePaths_(includePaths) {}

		IncludeResult* includeLocal(const char* headerName,
			const char* includerName,
			size_t inclusionDepth) override {
			return includeSystem(headerName, includerName, inclusionDepth);
		}

		IncludeResult* includeSystem(const char* headerName,
			const char* includerName,
			size_t inclusionDepth) override {
			Common::DiscardUnusedParameter(includerName);
			Common::DiscardUnusedParameter(inclusionDepth);
			// Ищем файл в указанных путях
			for (const auto& path : includePaths_) {
				std::string fullPath = path + "/" + headerName;
				if (std::filesystem::exists(fullPath)) {
					//TODO: need nromal solutution
					readFiles_.push_back(readFile(fullPath));
					return new IncludeResult(
						headerName,
						readFiles_.back().c_str(),
						readFiles_.back().size(),
						nullptr
					);
					
				}
			}
			return nullptr; // Файл не найден
		}

		void releaseInclude(IncludeResult* result) override {
			delete result;
		}

		std::string readFile(const std::string& filepath) {
			std::ifstream file(filepath);
			return std::string(
				(std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>()
			);
		}
		std::vector<std::string> readFiles_;
		std::vector<std::string> includePaths_;
	};


	class Shader : public RAL::Driver::Shader {
	public:

		struct CreateInfo {
			RAL::Driver::Shader::CreateInfo ralCreateInfo_;
		};

		Shader(const CreateInfo& createInfo) :
			RAL::Driver::Shader{ createInfo.ralCreateInfo_ }
		{
			Compile();
		}

		const Common::UInt32* GetSpirvCode() const noexcept
		{
			return spirv_.data();
		}

		const Common::Size GetSpirvSize() const noexcept
		{
			return spirv_.size();
		}

		const std::vector<Common::UInt32> GetSpirv() const noexcept
		{
			return spirv_;
		}

	private:

		void Compile() {
			glslang::InitializeProcess();

			const char* shaderStrings[1];
			shaderStrings[0] = GetCode().c_str();

			EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules/* | EShMsgDebugInfo*/);

			//TODO: take shaders include dir from config
			std::vector<std::string> includePaths = {
				"D:/OksEngine/Resources/Shaders/"
			};
			TBuiltInResource resources;
			initResources(resources);


			Includer includer(includePaths);


			glslang::TShader shader(TypeToStage(GetType()));
			shader.setStrings(shaderStrings, 1);
#if !defined(NDEBUG)
			shader.setDebugInfo(true);
#endif
			shader.setSourceFile(GetName().c_str());
			//shader.addSourceText(GetCode().c_str(), GetCode().size());
			
			//shader.addProcesses
			//shader.preprocess();
			//shader.setSourceEntryPoint("main");
			std::string preprocessedCode;
			const bool result = shader.preprocess(&resources, 450, ENoProfile, false, false, messages, &preprocessedCode, includer);
			
			ASSERT_FMSG(result, "Errors while preprocessing glsl shader: {}", shader.getInfoLog());


			if (!shader.parse(&resources, 450, false, messages, includer)) {

				ASSERT_FAIL_FMSG("Shader {} compilation failed: {}", GetName(), shader.getInfoLog());
				glslang::FinalizeProcess();

			}


			glslang::TProgram program;
			//program.ы
			program.addShader(&shader);

			if (!program.link(messages)) {
				ASSERT_FAIL_FMSG("Shader linking failed: {}", program.getInfoDebugLog());
				glslang::FinalizeProcess();
			}

			spv::SpvBuildLogger logger;
			glslang::SpvOptions spvOptions;

			// Критически важные настройки для NonSemantic.Shader.DebugInfo.100
#if !defined(NDEBUG)
			spvOptions.generateDebugInfo = true;
			spvOptions.stripDebugInfo = false;
			spvOptions.disableOptimizer = true;
			spvOptions.validate = true;
			spvOptions.optimizeSize = false;
#else
			spvOptions.generateDebugInfo = false;
			spvOptions.stripDebugInfo = true;
			spvOptions.disableOptimizer = false;
			spvOptions.validate = false;
			spvOptions.optimizeSize = true;
#endif
			std::vector<unsigned> spirv;
			glslang::GlslangToSpv(*program.getIntermediate(TypeToStage(GetType())), spirv, &logger, &spvOptions);

			OS::LogInfo("render/shader_compile", logger.getAllMessages());
			glslang::FinalizeProcess();

			spirv_ = std::move(spirv);

		}

		[[nodiscard]]
		static EShLanguage TypeToStage(Type type) noexcept {
			switch (type) {
			case Type::Vertex: {
				return EShLanguage::EShLangVertex;
				break;
			}
			case Type::Geometry: {
				return EShLanguage::EShLangGeometry;
				break;
			}
			case Type::Fragment: {
				return EShLanguage::EShLangFragment;
				break;
			}
			case Type::Compute: {
				return EShLanguage::EShLangCompute;
				break;
			}
			default: {
				OS::NotImplemented();
				return EShLanguage::EShLangCount;
			}
			};

		}

	private:
		std::vector<Common::UInt32> spirv_;
	};

}