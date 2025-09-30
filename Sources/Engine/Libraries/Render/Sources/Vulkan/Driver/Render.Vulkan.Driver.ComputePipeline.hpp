#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>

#include <RAL.Common.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.SwapChain.hpp>
#include <Render.Vulkan.Driver.ShaderModule.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>
#include <Render.Vulkan.Driver.RenderPass.hpp>
#include <Render.Vulkan.Driver.PipelineLayout.hpp>
#include <Render.Vulkan.Shader.hpp>
#include <Render.Vulkan.Driver.Image.hpp>

#include "Render.Vulkan.Driver.RenderPass.hpp"

namespace Render::Vulkan {

	class ComputePipeline : public Abstraction<VkPipeline> {
	public:

		struct CreateInfo {
			std::string name_ = "No name";
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::vector<VkPushConstantRange> pushConstants_;
			std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts_;
			std::shared_ptr<ShaderModule> computeShader_ = nullptr;
		};

		ComputePipeline(const CreateInfo& createInfo) :
			createInfo_{ createInfo } {

			VkPipelineShaderStageCreateInfo shaderStageInfo = {
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = VK_SHADER_STAGE_COMPUTE_BIT,
				.module = *createInfo_.computeShader_,
				.pName = "main"
			};
			
			PipelineLayout::CreateInfo pipelineLayoutCreateInfo;
			{
				pipelineLayoutCreateInfo.LD_ = createInfo.LD_;
				pipelineLayoutCreateInfo.pushConstants_ = createInfo.pushConstants_;
				pipelineLayoutCreateInfo.descriptorSetLayouts_.insert(
					pipelineLayoutCreateInfo.descriptorSetLayouts_.begin(),
					createInfo.descriptorSetLayouts_.begin(),
					createInfo.descriptorSetLayouts_.end());
				pipelineLayout_ = std::make_shared<PipelineLayout>(pipelineLayoutCreateInfo);
			}

			VkComputePipelineCreateInfo pipelineCreateInfo{ };
			{
				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.layout = *pipelineLayout_;
				pipelineCreateInfo.stage = shaderStageInfo;
			}

			VkPipeline pipeline = VK_NULL_HANDLE;

			VK_CALL(vkCreateComputePipelines(
				createInfo.LD_->GetHandle(),
				VK_NULL_HANDLE,
				1,
				&pipelineCreateInfo,
				nullptr,
				&pipeline
				),
				"Error while creating compute pipeline.");

			SetHandle(pipeline);
			OS::LogInfo("/render/vulkan/driver/", "Compute pipeline was created successful.");
		}

		~ComputePipeline() noexcept {
			Destroy();
		}

		[[nodiscard]]
		std::shared_ptr<PipelineLayout> GetLayout() const noexcept { return pipelineLayout_; }

	private:

		void Destroy() noexcept {
			ASSERT_FMSG(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkPipelineLayout.");
			vkDestroyPipeline(createInfo_.LD_->GetHandle(), GetHandle(), nullptr);
		}

	private:
		CreateInfo createInfo_{};
		std::shared_ptr<PipelineLayout> pipelineLayout_ = VK_NULL_HANDLE;
	};

}

