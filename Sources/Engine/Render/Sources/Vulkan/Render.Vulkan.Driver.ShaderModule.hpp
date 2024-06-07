#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <OS.FileSystem.BinaryFile.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>

namespace Render::Vulkan {

	class ShaderModule {
	public:

		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_;
			DS::Vector<Common::Byte>	 spirv_;
			RAL::Shader					 text_;
		};

		ShaderModule(const CreateInfo& createInfo) noexcept : logicDevice_{ createInfo.logicDevice_ } {

			OS::AssertMessage(createInfo.logicDevice_ != nullptr, "Attempt to create Shader module using nullptr Logic device.");

			VkShaderModuleCreateInfo shaderModuleCreateInfo{};
			{
				shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				shaderModuleCreateInfo.codeSize = createInfo.spirv_.GetSize();
				shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(createInfo.spirv_.GetData());
			}
			VkCall(vkCreateShaderModule(createInfo.logicDevice_->GetHandle(), &shaderModuleCreateInfo, nullptr, &shaderModule_),
				"Error while creating shader module.");
			OS::LogInfo("render/vulkan/driver/shader", { "Shader module was created successfuly." });
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