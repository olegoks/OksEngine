#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.Types.hpp>

#include <OS.Assert.hpp>
#include <OS.Logger.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Shape.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Shader.hpp>
#include <Render.Vulkan.Driver.CommandPool.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.Pipeline.hpp>
#include <Render.Vulkan.Driver.VertexBuffer.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
#include <Render.Vulkan.Driver.IndexBuffer.hpp>


namespace Render::Vulkan {

	class CommandBuffer : public Abstraction<VkCommandBuffer> {
	public:


		struct CreateInfo {
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::shared_ptr<CommandPool> commandPool_ = nullptr;
		};

		CommandBuffer(const CreateInfo& createInfo) :
			createInfo_{ createInfo } {

			Allocate(*createInfo.LD_, *createInfo.commandPool_);

		}

		void Begin() noexcept {
			VkCommandBufferBeginInfo beginInfo{};
			{
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //The command buffer will be rerecorded right after executing it once.
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
				if (depthBufferInfo.enable) {
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

		void SetViewport(const VkViewport& viewport) noexcept {
			vkCmdSetViewport(GetHandle(), 0, 1, &viewport);
		}


		void SetScissor(const VkRect2D& scissor) noexcept {
			vkCmdSetScissor(GetHandle(), 0, 1, &scissor);
		}

		template<class PipelineType>
		void BindPipeline(std::shared_ptr<PipelineType> pipeline) noexcept {
			vkCmdBindPipeline(
				GetHandle(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline->GetHandle());
		}

		/*void BindBuffer(std::shared_ptr<VertexBuffer<Vertex2ftc>> vertexBuffer) noexcept {
			VkDeviceSize offsets[] = { 0 };
			const VkBuffer bufferHandle = vertexBuffer->GetNative();
			vkCmdBindVertexBuffers(
				GetHandle(),
				0,
				1,
				&bufferHandle,
				offsets);
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

		void BindBuffer(std::shared_ptr<VertexBuffer<Vertex3fc>> vertexBuffer) noexcept {
			VkDeviceSize offsets[] = { 0 };
			const VkBuffer bufferHandle = vertexBuffer->GetNative();
			vkCmdBindVertexBuffers(
				GetHandle(),
				0,
				1,
				&bufferHandle,
				offsets);
		}

		void BindBuffer(std::shared_ptr<VertexBuffer<Vertex3fnt>> vertexBuffer) noexcept {
			VkDeviceSize offsets[] = { 0 };
			const VkBuffer bufferHandle = vertexBuffer->GetNative();
			vkCmdBindVertexBuffers(
				GetHandle(),
				0,
				1,
				&bufferHandle,
				offsets);
		}*/

		//void BindBuffer(std::shared_ptr<IndexBuffer<Index16>> indexBuffer) noexcept {
		//	vkCmdBindIndexBuffer(
		//		GetHandle(),
		//		indexBuffer->GetNative(),
		//		0,
		//		VK_INDEX_TYPE_UINT16);
		//}

		void BindBuffer(std::shared_ptr<VertexBuffer2> vertexBuffer) noexcept {
			VkDeviceSize offsets[] = { 0 };
			const VkBuffer bufferHandle = vertexBuffer->GetHandle();
			vkCmdBindVertexBuffers(
				GetHandle(),
				0,
				1,
				&bufferHandle,
				offsets);
		}

		void BindBuffer(std::shared_ptr<IndexBuffer2> indexBuffer) noexcept {
			vkCmdBindIndexBuffer(
				GetHandle(),
				*indexBuffer,
				0,
				VK_INDEX_TYPE_UINT16);
		}


		template<class PipelineType>
		void BindDescriptorSets(std::shared_ptr<PipelineType> pipeline, const std::vector<std::shared_ptr<DescriptorSet>>& descriptorSets) noexcept {

			std::vector<VkDescriptorSet> descriptorSetsHandles;
			for (auto descriptorSetPtr : descriptorSets) {
				descriptorSetsHandles.push_back(descriptorSetPtr->GetHandle());
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

		void Draw(Common::Size verticesNumber) {
			vkCmdDraw(
				GetHandle(),
				static_cast<uint32_t>(verticesNumber), 
				1,
				0, 
				0);
		}

		void DrawIndexed(Common::Size indicesNumber) {
			vkCmdDrawIndexed(GetHandle(), static_cast<uint32_t>(indicesNumber), 1, 0, 0, 0);
		}

		void EndRenderPass() noexcept {
			vkCmdEndRenderPass(GetHandle());
		}

		void Copy(std::shared_ptr<Buffer> from, std::shared_ptr<Buffer> to, Common::Size offsetFrom, Common::Size offsetTo, Common::Size bytesNumber) noexcept {

			Copy(*from, *to, offsetFrom, offsetTo, bytesNumber);
		}

		void Copy(const Buffer& from, const Buffer& to, Common::Size offsetFrom, Common::Size offsetTo, Common::Size bytesNumber) noexcept {
			VkBufferCopy copyRegion{};
			{
				copyRegion.srcOffset = offsetFrom;
				copyRegion.dstOffset = offsetTo;
				copyRegion.size = bytesNumber;
			}
			vkCmdCopyBuffer(GetHandle(), from, to, 1, &copyRegion);
		}

		void Copy(std::shared_ptr<Buffer> from, std::shared_ptr<Image> to) noexcept {
			VkBufferImageCopy copyRegion{};
			{
				copyRegion.bufferOffset = 0;
				copyRegion.bufferRowLength = 0;
				copyRegion.bufferImageHeight = 0;

				copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel = 0;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount = 1;

				copyRegion.imageOffset = { 0, 0, 0 };
				copyRegion.imageExtent = {
					to->GetSize().x,
					to->GetSize().y,
					1
				};
			}
			vkCmdCopyBufferToImage(
				GetHandle(),
				from->GetHandle(),
				*to,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&copyRegion
			);
		}

		void ImageMemoryBarrier(
			VkImage image,
			Common::UInt32 baseMipLevel,
			Common::UInt32 mipLevels,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkAccessFlags sourceAccessMask,
			VkAccessFlags destinationAccessMask,
			VkPipelineStageFlags sourceStage,
			VkPipelineStageFlags destinationStage
		) {
			VkImageMemoryBarrier barrier{};
			{
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.oldLayout = oldLayout;
				barrier.newLayout = newLayout;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = image;
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				barrier.subresourceRange.baseMipLevel = baseMipLevel;
				barrier.subresourceRange.levelCount = mipLevels;
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = 1;
				barrier.srcAccessMask = sourceAccessMask;
				barrier.dstAccessMask = destinationAccessMask;

			}
			vkCmdPipelineBarrier(
				GetHandle(),
				sourceStage, destinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
		}

		void ImageBlit(
			std::shared_ptr<Image> srcImage,
			VkOffset3D srcOffset[2],
			Common::UInt32 srcMipLevel,
			VkImageLayout srcImageLayout,
			std::shared_ptr<Image> dstImage,
			VkOffset3D dstOffset[2],
			Common::UInt32 dstMipLevel,
			VkImageLayout dstImageLayout) {

			VkImageBlit blit {
				.srcSubresource = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel = srcMipLevel,
					.baseArrayLayer = 0,
					.layerCount = 1
				},
				.srcOffsets = { srcOffset[0], srcOffset[1] },
				
				.dstSubresource = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.mipLevel = dstMipLevel,
					.baseArrayLayer = 0,
					.layerCount = 1
				},
				.dstOffsets = { dstOffset[0], dstOffset[1] }
			};

			vkCmdBlitImage(GetHandle(),
				*srcImage, srcImageLayout,
				*dstImage, dstImageLayout,
				1, &blit,
				VK_FILTER_LINEAR);

		}

		//void BindVB()

		//void BindMesh(std::shared_ptr<Mesh> shape) {
		//	BindBuffer(shape->GetVertexBuffer());
		//	BindBuffer(shape->GetIndexBuffer());
		//}

		//void BindShape(std::shared_ptr<ColoredShape> shape) {
		//	BindBuffer(shape->GetVertexBuffer());
		//	BindBuffer(shape->GetIndexBuffer());
		//}

		//void BindShape(std::shared_ptr<UIShape> shape) {
		//	BindBuffer(shape->GetVertexBuffer());
		//	BindBuffer(shape->GetIndexBuffer());
		//}

		//void BindShape(std::shared_ptr<TexturedShape> shape) {
		//	BindBuffer(shape->GetVertexBuffer());
		//	BindBuffer(shape->GetIndexBuffer());
		//}

		void DrawIndexed(std::shared_ptr<IndexBuffer2> ib) {
			DrawIndexed(ib->GetIndicesNumber());
		}

		void End() noexcept {
			VkCall(vkEndCommandBuffer(GetHandle()),
				"Error on ending executing commands.");
		}

		void Submit(VkQueue queue) {

			VkSubmitInfo submitInfo{
				.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.commandBufferCount = 1,
				.pCommandBuffers = &GetHandle()
			};
			VkCall(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE),
				"Error while submitting command buffer.");

		}

		~CommandBuffer() noexcept {
			vkFreeCommandBuffers(*createInfo_.LD_, *createInfo_.commandPool_, 1, &GetHandle());
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

	private:
		CreateInfo createInfo_;
	};

}