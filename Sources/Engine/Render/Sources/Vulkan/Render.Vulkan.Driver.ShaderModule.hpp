#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

export module OksEngine.Render.Vulkan.Driver.ShaderModule;

import OksEngine.OS.Assert;
import OksEngine.OS.Logger;
import OksEngine.OS.FileSystem.BinaryFile;

import OksEngine.Render.Vulkan.Common;
import OksEngine.Render.Vulkan.Driver.LogicDevice;

namespace Render::Vulkan {

	class ShaderModule {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_;
			std::shared_ptr<OS::BinaryFile> binaryShaderFile_;
		};

		ShaderModule(const CreateInfo& createInfo) noexcept : logicDevice_{ createInfo.logicDevice_ } {

			OS::AssertMessage(createInfo.logicDevice_ != nullptr, "Attempt to create Shader module using nullptr Logic device.");

			VkShaderModuleCreateInfo shaderModuleCreateInfo{};
			{
				shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				shaderModuleCreateInfo.codeSize = createInfo.binaryShaderFile_->GetSize();
				shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(createInfo.binaryShaderFile_->GetData().get());
			}
			VkCall(vkCreateShaderModule(createInfo.logicDevice_->GetHandle(), &shaderModuleCreateInfo, nullptr, &shaderModule_),
				"Error while creating shader module.");
			OS::LogInfo("render/vulkan/driver/shader", { "Shader module %s was created successfuly.", createInfo.binaryShaderFile_->GetName().c_str() });
		}

		[[nodiscard]]
		VkShaderModule GetNative() const noexcept { return shaderModule_; }

		~ShaderModule() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(logicDevice_ != nullptr, "Logic device is nullptr.");
			OS::AssertMessage(shaderModule_ != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkShaderModule.");
			vkDestroyShaderModule(logicDevice_->GetHandle(), GetNative(), nullptr);
		}

		void SetNative(VkShaderModule shaderModule) noexcept {
			OS::Assert((shaderModule != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
				((shaderModule == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			shaderModule_ = shaderModule;
		}

	private:

		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		VkShaderModule shaderModule_ = VK_NULL_HANDLE;
	};
}