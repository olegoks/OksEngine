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

			EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

			glslang::TShader shader(TypeToStage(GetType()));
			shader.setStrings(shaderStrings, 1);

			TBuiltInResource resources;
			initResources(resources);

			if (!shader.parse(&resources, 100, false, messages)) {
				OS::AssertFailMessage({ "Shader compilation failed: {}", shader.getInfoLog() });
				glslang::FinalizeProcess();

			}

			glslang::TProgram program;
			program.addShader(&shader);

			if (!program.link(messages)) {
				OS::AssertFailMessage({ "Shader linking failed: {}", program.getInfoDebugLog() });
				glslang::FinalizeProcess();
			}
			std::vector<unsigned> spirv;
			glslang::GlslangToSpv(*program.getIntermediate(TypeToStage(GetType())), spirv);

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
			case Type::Fragment: {
				return EShLanguage::EShLangFragment;
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