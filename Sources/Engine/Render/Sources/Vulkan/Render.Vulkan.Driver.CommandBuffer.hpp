#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.CommandPool.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.Pipeline.hpp>
#include <Render.Vulkan.Driver.VertexBuffer.hpp>
#include <Render.Vulkan.Driver.IndexBuffer.hpp>


namespace Render::Vulkan {

	class CommandBuffer {
	public:
		CommandBuffer(std::shared_ptr<CommandPool> commandPool, std::shared_ptr<LogicDevice> logicDevice) :
			logicDevice_{ logicDevice },
			commandPool_ { commandPool } {

			Allocate(logicDevice->GetHandle(), commandPool->GetNative());

		}

		void Begin() noexcept {
			VkCommandBufferBeginInfo beginInfo{};
			{
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0;
				beginInfo.pInheritanceInfo = nullptr;
			}
			VkCall(vkBeginCommandBuffer(GetNative(), &beginInfo), "Error while begin command buffer.");
		}

		struct DepthBufferInfo {
			bool enable = false;
			VkClearValue clearValue_;
		};

		void BeginRenderPass(VkRenderPass renderPass, VkFramebuffer frameBuffer, VkExtent2D extent, const VkClearValue& clearColor, const DepthBufferInfo& depthBufferInfo) noexcept {

			std::vector<VkClearValue> clearValues;
			{
				clearValues.push_back(clearColor);
				if(depthBufferInfo.enable){
					clearValues.push_back(depthBufferInfo.clearValue_);
				}
			}

			VkRenderPassBeginInfo renderPassInfo{};
			{
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass;
				renderPassInfo.framebuffer = frameBuffer;
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = extent;
				renderPassInfo.clearValueCount = static_cast<Common::UInt32>(clearValues.size());
				renderPassInfo.pClearValues = clearValues.data();
			}
			vkCmdBeginRenderPass(
				GetNative(),
				&renderPassInfo,
				VK_SUBPASS_CONTENTS_INLINE);
		}

		template<class PipelineType>
		void BindPipeline(std::shared_ptr<PipelineType> pipeline) noexcept {
			vkCmdBindPipeline(
				GetNative(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline->GetHandle());
		}

		void BindBuffer(std::shared_ptr<VertexBuffer<Vertex3fnc>> vertexBuffer) noexcept {
			VkDeviceSize offsets[] = { 0 };
			const VkBuffer bufferHandle = vertexBuffer->GetNative();
			vkCmdBindVertexBuffers(
				GetNative(),
				0,
				1,
				&bufferHandle,
				offsets);
		}

		void BindBuffer(std::shared_ptr<IndexBuffer> indexBuffer) noexcept {
			vkCmdBindIndexBuffer(
				GetNative(),
				indexBuffer->GetNative(),
				0,
				VK_INDEX_TYPE_UINT16);
		}

		template<class PipelineType>
		void BindDescriptorSet(std::shared_ptr<PipelineType> pipeline, VkDescriptorSet descriptorSet) noexcept {
			vkCmdBindDescriptorSets(
				GetNative(),
				VK_PIPELINE_BIND_POINT_GRAPHICS, // Bind to graphics pipeline(not computation pipeline)
				pipeline->GetLayout()->GetHandle(),
				0,
				1,
				&descriptorSet,
				0,
				nullptr);
		}

		void DrawIndexed(Common::Size indicesNumber) {
			vkCmdDrawIndexed(GetNative(), static_cast<uint32_t>(indicesNumber), 1, 0, 0, 0);
		}

		void EndRenderPass() noexcept {
			vkCmdEndRenderPass(GetNative());
		}

		void End() noexcept {
			VkCall(vkEndCommandBuffer(GetNative()),
				"Error on ending executing commands.");
		}
		[[nodiscard]]
		const VkCommandBuffer& GetNative() const noexcept {
			return commandBuffer_;
		}

		~CommandBuffer() noexcept {
			vkFreeCommandBuffers(logicDevice_->GetHandle(), commandPool_->GetNative(), 1, &GetNative());
		}

	private:

		void Allocate(VkDevice logicDevice, VkCommandPool commandPool) noexcept {

			VkCommandBufferAllocateInfo allocInfo{};
			{
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.commandPool = commandPool;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandBufferCount = 1;
			}

			VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
			VkCall(vkAllocateCommandBuffers(logicDevice, &allocInfo, &commandBuffer),
				"Command buffer allocation error.");
			SetNative(commandBuffer);

		}

		void SetNative(VkCommandBuffer commandBuffer) noexcept {
			OS::Assert((commandBuffer != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
				((commandBuffer == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			commandBuffer_ = commandBuffer;
		}

	private:
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		std::shared_ptr<CommandPool> commandPool_ = nullptr;
		VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;
	};

}