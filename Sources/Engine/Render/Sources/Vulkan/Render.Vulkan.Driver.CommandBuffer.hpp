#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.CommandPool.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.Pipeline.hpp>
#include <Render.Vulkan.Driver.VertexBuffer.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
#include <Render.Vulkan.Driver.IndexBuffer.hpp>


namespace Render::Vulkan {

	class CommandBuffer : public Abstraction<VkCommandBuffer>{
	public:


		struct CreateInfo {
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<CommandPool> commandPool_ = nullptr;
		};

		CommandBuffer(const CreateInfo& createInfo) :
			createInfo_{ createInfo } {

			Allocate(*createInfo.logicDevice_, *createInfo.commandPool_);

		}

		void Begin() noexcept {
			VkCommandBufferBeginInfo beginInfo{};
			{
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = 0;
				beginInfo.pInheritanceInfo = nullptr;
			}
			VkCall(vkBeginCommandBuffer(GetHandle(), &beginInfo), "Error while begin command buffer.");
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
				GetHandle(),
				&renderPassInfo,
				VK_SUBPASS_CONTENTS_INLINE);
		}

		template<class PipelineType>
		void BindPipeline(std::shared_ptr<PipelineType> pipeline) noexcept {
			vkCmdBindPipeline(
				GetHandle(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline->GetHandle());
		}

		void BindBuffer(std::shared_ptr<VertexBuffer<Vertex3fnc>> vertexBuffer) noexcept {
			VkDeviceSize offsets[] = { 0 };
			const VkBuffer bufferHandle = vertexBuffer->GetNative();
			vkCmdBindVertexBuffers(
				GetHandle(),
				0,
				1,
				&bufferHandle,
				offsets);
		}

		void BindBuffer(std::shared_ptr<IndexBuffer<RAL::Index16>> indexBuffer) noexcept {
			vkCmdBindIndexBuffer(
				GetHandle(),
				indexBuffer->GetNative(),
				0,
				VK_INDEX_TYPE_UINT16);
		}

		template<class PipelineType>
		void BindDescriptorSets(std::shared_ptr<PipelineType> pipeline, const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets) noexcept {
			
			std::vector<VkDescriptorSet> descriptorSetsHandles;
			for (auto descriptorSetPtr : descriptorSets) {
				descriptorSetsHandles.push_back(descriptorSetPtr->GetNative());
			}
			
			vkCmdBindDescriptorSets(
				GetHandle(),
				VK_PIPELINE_BIND_POINT_GRAPHICS, // Bind to graphics pipeline(not computation pipeline)
				pipeline->GetLayout()->GetHandle(),
				0,
				static_cast<Common::UInt32>(descriptorSets.size()),
				descriptorSetsHandles.data(),
				0,
				nullptr);
		}



		void DrawIndexed(Common::Size indicesNumber) {
			vkCmdDrawIndexed(GetHandle(), static_cast<uint32_t>(indicesNumber), 1, 0, 0, 0);
		}

		void EndRenderPass() noexcept {
			vkCmdEndRenderPass(GetHandle());
		}

		void Copy(std::shared_ptr<Buffer> from, std::shared_ptr<Buffer> to) noexcept {
			VkBufferCopy copyRegion{};
			{
				copyRegion.srcOffset = 0;
				copyRegion.dstOffset = 0;
				copyRegion.size = from->GetSizeInBytes();
			}
			vkCmdCopyBuffer(GetHandle(), from->GetNative(), to->GetNative(), 1, &copyRegion);
		}

		void End() noexcept {
			VkCall(vkEndCommandBuffer(GetHandle()),
				"Error on ending executing commands.");
		}

		~CommandBuffer() noexcept {
			vkFreeCommandBuffers(*createInfo_.logicDevice_, *createInfo_.commandPool_, 1, &GetHandle());
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
			SetHandle(commandBuffer);

		}

		//void SetNative(VkCommandBuffer commandBuffer) noexcept {
		//	OS::Assert((commandBuffer != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
		//		((commandBuffer == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
		//	commandBuffer_ = commandBuffer;
		//}

	private:
		CreateInfo createInfo_;
	};

}