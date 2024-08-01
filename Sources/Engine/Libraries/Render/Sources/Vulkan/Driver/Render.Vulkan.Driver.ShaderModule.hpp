#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <OS.FileSystem.BinaryFile.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class ShaderModule : public Abstraction<VkShaderModule>{
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice>	logicDevice_;
			std::vector<Common::UInt32>		spirv_;
		};

		ShaderModule(const CreateInfo& createInfo) noexcept :
			logicDevice_{ createInfo.logicDevice_ } {

			OS::AssertMessage(createInfo.logicDevice_ != nullptr, "Attempt to create Shader module using nullptr Logic device.");

			VkShaderModuleCreateInfo shaderModuleCreateInfo{};
			{
				shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				shaderModuleCreateInfo.codeSize = createInfo.spirv_.size() * sizeof(Common::UInt32);
				shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(createInfo.spirv_.data());
			}
			VkShaderModule shaderModule = VK_NULL_HANDLE;
			VkCall(vkCreateShaderModule(createInfo.logicDevice_->GetHandle(), &shaderModuleCreateInfo, nullptr, &shaderModule),
				"Error while creating shader module.");
			OS::LogInfo("render/vulkan/driver/shader", { "Shader module was created successfuly." });
			SetHandle(shaderModule);
		}

		~ShaderModule() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(logicDevice_ != nullptr, "Logic device is nullptr.");
			OS::AssertMessage(GetHandle(), "Attempt to destroy VK_NULL_HANDLE VkShaderModule.");
			vkDestroyShaderModule(logicDevice_->GetHandle(), GetHandle(), nullptr);
		}

	private:

		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;

	};
}