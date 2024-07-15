#pragma once

#include <map>

#include <Render.Vulkan.Common.hpp>

#include <Render.Vulkan.Driver.Instance.hpp>
#include <Render.Vulkan.Driver.Debug.hpp>
#include <Render.Vulkan.Driver.WindowSurface.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.LogicDevice.hpp>
#include <Render.Vulkan.Driver.SwapChain.hpp>
#include <Render.Vulkan.Driver.ShaderModule.hpp>
#include <Render.Vulkan.Driver.CommandPool.hpp>

#include <Render.Vulkan.Driver.CommandBuffer.hpp>
//
#include <Render.Vulkan.Driver.Buffer.hpp>
#include <Render.Vulkan.Driver.VertexBuffer.hpp>
#include <Render.Vulkan.Driver.IndexBuffer.hpp>
#include <Render.Vulkan.Driver.UniformBuffer.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>
//
#include <Render.Vulkan.Driver.DescriptorPool.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
//
#include <Render.Vulkan.Driver.Pipeline.hpp>
#include <Render.Vulkan.Driver.FrameBuffer.hpp>
//
#include <Render.Vulkan.Driver.Fence.hpp>
#include <Render.Vulkan.Driver.Semaphore.hpp>
//
#include <Render.Vulkan.Driver.Image.hpp>
#include <Render.Vulkan.Driver.ImageView.hpp>
#include <Render.Vulkan.Driver.DeviceMemory.hpp>

#include <Render.Vulkan.Driver.Sampler.hpp>
#include <Render.Vulkan.Shape.hpp>
//
//#include <imgui.h>
//#include <imgui_internal.h>
//#include <imconfig.h>
//#include <ImgUtil.h>
//#include <imgui_impl_vulkan.h>
//#include <imgui_impl_glfw.h>
//
//
//#include <implot.h>
//#include <implot_internal.h>

namespace Render::Vulkan {

	class Driver : public RAL::Driver {
	public:

		struct GlobalData {
			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
			alignas(16) glm::vec3 lightPos_;
			float lightIntensity_;
		};

		class ImageContext {
		public:
			Common::UInt32 index_ = Common::Limits<Common::UInt32>::Max();
			std::shared_ptr<Fence> inRender_ = nullptr;
			std::shared_ptr<Semaphore> imageAvailableSemaphore_ = nullptr;
			std::shared_ptr<CommandBuffer> commandBuffer_ = nullptr;
		public:
			[[nodiscard]]
			bool IsBusy() const noexcept { return (inRender_) ? (!inRender_->IsSignaled()) : (false); }
			void WaitForFree() const noexcept { if (IsBusy()) { inRender_->Wait(); inRender_->Reset(); }; }
		};

		struct DepthTestData {
			std::shared_ptr<Image> image_ = nullptr;
			std::shared_ptr<ImageView> imageView_ = nullptr;
			std::shared_ptr<DeviceMemory> imageMemory_ = nullptr;
		};

		class FrameContext {
		public:
			std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
			std::shared_ptr<SwapChain> swapChain_ = nullptr;
			VkQueue graphicsQueue_;
			VkQueue presentQueue_;
			std::shared_ptr<ImageContext> imageContext_ = nullptr;
			std::shared_ptr<Semaphore> imageAvailableSemaphore_ = nullptr;
			std::shared_ptr<Semaphore> renderFinishedSemaphore_ = nullptr;
			std::shared_ptr<Fence> renderFinishedFence_ = nullptr;
		public:

			void SetImage(std::shared_ptr<ImageContext> image) noexcept {
				if (imageContext_ != nullptr) {
					imageContext_->inRender_ = nullptr;
				}
				imageContext_ = image;
				OS::Assert(imageContext_->inRender_ == nullptr);
				imageContext_->inRender_ = renderFinishedFence_;
			}

			void WaitForRenderToImageFinish() noexcept {
				if (imageContext_ != nullptr) {
					OS::AssertMessage(imageContext_->inRender_ == renderFinishedFence_, "Fence of image and frame are different.");
					imageContext_->inRender_->Wait();
					imageContext_->inRender_->Reset();
				}
			}

			void Render() {

				VkSubmitInfo submitInfo{};
				{
					submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
					submitInfo.waitSemaphoreCount = 1;
					submitInfo.pWaitSemaphores = &imageAvailableSemaphore_->GetNative();
					VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
					submitInfo.pWaitDstStageMask = waitStages;
					submitInfo.commandBufferCount = 1;
					submitInfo.pCommandBuffers = &imageContext_->commandBuffer_->GetHandle();
					submitInfo.signalSemaphoreCount = 1;
					submitInfo.pSignalSemaphores = &renderFinishedSemaphore_->GetNative();

					VkCall(vkQueueSubmit(logicDevice_->GetGraphicsQueue(), 1, &submitInfo, renderFinishedFence_->GetNative()),
						"Error while submitting commands.");
				}

			}

			void ShowImage() {
				VkSwapchainKHR swapChainHandle_ = VK_NULL_HANDLE;
				swapChainHandle_ = swapChain_->GetHandle();
				VkPresentInfoKHR presentInfo{};
				{
					presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
					presentInfo.waitSemaphoreCount = 1;
					presentInfo.pWaitSemaphores = &renderFinishedSemaphore_->GetNative();
					presentInfo.swapchainCount = 1;

					presentInfo.pSwapchains = &swapChainHandle_;
					presentInfo.pImageIndices = &imageContext_->index_;
					presentInfo.pResults = nullptr;

					VkCall(vkQueuePresentKHR(logicDevice_->GetPresentQueue(), &presentInfo),
						"Error while showing image.");
				}
			}

			void WaitForQueueIdle() const noexcept {
				VkCall(vkQueueWaitIdle(logicDevice_->GetPresentQueue()),
					"Error while waitting for queue idle.");
			}

		};

		[[nodiscard]]
		static PhysicalDevice::Formats GetAvailablePhysicalDeviceSurfaceFormats(
			std::shared_ptr<PhysicalDevice> physicalDevice,
			std::shared_ptr<WindowSurface> windowSurface) noexcept;

		[[nodiscard]]
		static PhysicalDevice::PresentModes GetAvailablePresentModes(
			std::shared_ptr<PhysicalDevice> physicalDevice,
			std::shared_ptr<WindowSurface> windowSurface) noexcept;

		[[nodiscard]]
		static VkPresentModeKHR ChoosePresentMode(
			const PhysicalDevice::PresentModes& availablePresentModes) noexcept;

		[[nodiscard]]
		std::pair<QueueFamily, QueueFamily> ChooseQueueFamilies(
			std::shared_ptr<PhysicalDevice> physicalDevice,
			std::shared_ptr<WindowSurface> windowSurface) noexcept;

		[[nodiscard]]
		static QueueFamilies GetPresentQueueFamilies(
			std::shared_ptr<PhysicalDevice> physicalDevice,
			std::shared_ptr<WindowSurface> windowSurface) noexcept;

		[[nodiscard]]
		static std::shared_ptr<PhysicalDevice> ChoosePhysicalDevice(
			const std::vector<std::shared_ptr<PhysicalDevice>>& availablePhysicalDevices,
			std::shared_ptr<WindowSurface> windowSurface,
			const Extensions& requiredExtensions) noexcept;

		Driver(const CreateInfo& info) noexcept :
			RAL::Driver{ info }{

			auto windowInfo = info.surface_;

			Extensions requiredExtensions;
			requiredExtensions.AddExtension("VK_EXT_debug_utils");
			requiredExtensions.AddExtension("VK_EXT_debug_report");
			//requiredExtensions.AddExtension("VK_KHR_portability_subset");

			if (windowInfo.uiSubsystem_ == RAL::UISubsystem::GLFW) {
				const Common::UInt32 extensionsCount = std::any_cast<Common::UInt32>(windowInfo.param2_);
				const char** extensions = std::any_cast<const char**>(windowInfo.param3_);
				for (Common::UInt32 i = 0; i < extensionsCount; i++) {
					requiredExtensions.AddExtension(extensions[i]);
				}
			}

			ValidationLayers requiredValidationLayers;
			requiredValidationLayers.AddLayer("VK_LAYER_KHRONOS_validation");
			//requiredValidationLayers.AddLayer("VK_LAYER_LUNARG_api_dump");
			//requiredValidationLayers.AddLayer("VK_LAYER_KHRONOS_profiles");

			Instance::CreateInfo instanceCreateInfo;
			{
				instanceCreateInfo.requiredExtensions_ = requiredExtensions;
				instanceCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				instance_ = std::make_shared<Instance>(instanceCreateInfo);
			}

			WindowSurface::CreateInfo windowSurfaceCreateInfo;
			{
				windowSurfaceCreateInfo.instance_ = instance_;
				windowSurfaceCreateInfo.renderSurface_ = info.surface_;
				windowSurface_ = std::make_shared<WindowSurface>(windowSurfaceCreateInfo);
			}

			Extensions requiredDeviceExtensions;
			requiredDeviceExtensions.AddExtension("VK_KHR_swapchain");

			const auto availablePhysicalDevices = instance_->GetPhysicalDevices();
			physicalDevice_ = ChoosePhysicalDevice(availablePhysicalDevices, windowSurface_, requiredDeviceExtensions);

			auto [graphicsQueueFamily, presentQueueFamily] = ChooseQueueFamilies(physicalDevice_, windowSurface_);
			graphicsQueueFamily_ = graphicsQueueFamily;
			presentQueueFamily_ = presentQueueFamily;

			//DEBUG
			Debug::CreateInfo debugCreateInfo;
			{
				debugCreateInfo.instance_ = instance_;
				debugCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				debug_ = std::make_shared<Debug>(debugCreateInfo);
			}

			//LOGIC_DEVICE
			LogicDevice::CreateInfo logicDeviceCreateInfo;
			{
				logicDeviceCreateInfo.physicalDevice_ = physicalDevice_;
				logicDeviceCreateInfo.requiredExtensions_ = requiredDeviceExtensions;
				logicDeviceCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				logicDeviceCreateInfo.presentQueueFamily_ = presentQueueFamily_;
				logicDeviceCreateInfo.graphicsQueueFamily_ = graphicsQueueFamily_;
				logicDevice_ = std::make_shared<LogicDevice>(logicDeviceCreateInfo);
			}

			//SWAPCHAIN
			SwapChain::CreateInfo swapChainCreateInfo;
			{
				swapChainCreateInfo.imageSize_ = info.surface_.size_;
				swapChainCreateInfo.logicDevice_ = logicDevice_;
				swapChainCreateInfo.physicalDevice_ = physicalDevice_;
				swapChainCreateInfo.windowSurface_ = windowSurface_;
				swapChainCreateInfo.presentQueueFamily_ = presentQueueFamily_;
				swapChainCreateInfo.graphicsQueueFamily_ = graphicsQueueFamily_;
				const PhysicalDevice::Formats& availableFormats = GetAvailablePhysicalDeviceSurfaceFormats(physicalDevice_, windowSurface_);
				swapChainCreateInfo.format_ = ChooseSwapChainSurfaceFormat(availableFormats);
				swapChainCreateInfo.presentMode_ = ChoosePresentMode(GetAvailablePresentModes(physicalDevice_, windowSurface_));
				swapChainCreateInfo.capabilities_ = GetCapabilities(physicalDevice_, windowSurface_);
				swapChainCreateInfo.extent_ = ChooseSwapExtent(physicalDevice_, windowSurface_, info.surface_.size_);
				swapChain_ = std::make_shared<SwapChain>(swapChainCreateInfo);
			}

			//COMMAND_POOL
			CommandPool::CreateInfo commandPoolCreateInfo;
			{
				commandPoolCreateInfo.logicDevice_ = logicDevice_;
				commandPoolCreateInfo.queueFamily_ = graphicsQueueFamily_;
				commandPool_ = std::make_shared<CommandPool>(commandPoolCreateInfo);
			}

			{
				const VkDeviceSize bufferSize = sizeof(GlobalData);
				for (Common::Index i = 0; i < swapChain_->GetImagesNumber(); i++) {
					auto uniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);
					globalDataUBs_.push_back(uniformBuffer);
				}
			}

			//DESCRIPTOR_POOL
			const Common::Size descriptorPoolSize = swapChain_->GetImagesNumber() * 2;
			descriptorPool_ = std::make_shared<DescriptorPool>(logicDevice_, descriptorPoolSize);

			//DEPTH BUFFER
			//if (info.enableDepthTest_) {
			{
				auto depthTestData =  std::make_shared<DepthTestData>();
				{
					Image::CreateInfo depthImageCreateInfo;
					{
						depthImageCreateInfo.physicalDevice_ = physicalDevice_;
						depthImageCreateInfo.logicDevice_ = logicDevice_;
						depthImageCreateInfo.format_ = VK_FORMAT_D32_SFLOAT;
						depthImageCreateInfo.size_ = swapChain_->GetSize();
						depthImageCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
						depthImageCreateInfo.usage_ = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
					}
					depthTestData->image_ = std::make_shared<AllocatedImage>(depthImageCreateInfo);
					depthTestData->imageView_ = CreateImageViewByImage(logicDevice_, depthTestData->image_, VK_IMAGE_ASPECT_DEPTH_BIT);
				}
				depthTestData_ = depthTestData;
				//}
			}


			//DESCRIPTOR SET LAYOUTS
			{
				DescriptorSetLayout::CreateInfo descriptorSetLayoutCreateInfo;
				{
					descriptorSetLayoutCreateInfo.logicDevice_ = logicDevice_;
					std::vector<VkDescriptorSetLayoutBinding> bindings{
						{
							0,
							VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
							1,
							VK_SHADER_STAGE_VERTEX_BIT,
							nullptr
						}
					};
					descriptorSetLayoutCreateInfo.bindings_ = bindings;
				}
				globalDataDSL_ = std::make_shared<DescriptorSetLayout>(descriptorSetLayoutCreateInfo);
			}


			RenderPass::CreateInfo RPCreateInfo{};
			{
				RPCreateInfo.logicDevice_ = logicDevice_;
				RPCreateInfo.colorAttachmentFormat_ = swapChain_->GetFormat().format;
				auto depthTestInfo = std::make_shared<RenderPass::DepthTestInfo>();
				depthTestInfo->depthStencilBufferFormat_ = depthTestData_->image_->GetFormat();
				RPCreateInfo.depthTestInfo_ = depthTestInfo;
			}
			renderPass_ = std::make_shared<RenderPass>(RPCreateInfo);


			//IMGUI Native PIPELINE
			{
				auto imguiNativePipelineInfo = info.imguiNativePipeline_;

				ImguiNativePipeline::CreateInfo createInfo;
				{
					createInfo.physicalDevice_ = physicalDevice_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.renderPass_ = renderPass_;
					createInfo.colorAttachmentFormat_ = swapChain_->GetFormat().format;
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = imguiNativePipelineInfo->vertexShader_;
					createInfo.fragmentShader_ = imguiNativePipelineInfo->fragmentShader_;
					if (imguiNativePipelineInfo->enableDepthTest_) {
						auto depthTestData = std::make_shared<ImguiNativePipeline::DepthTestInfo>();
						depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
						createInfo.depthTestInfo_ = depthTestData;
					}
				}
				imguiNativePipeline_ = std::make_shared<ImguiNativePipeline>(createInfo);
			}

			//IMGUI PIPELINE
			{
				auto imguiPipelineInfo = info.imguiPipeline_;

				ImguiPipeline::CreateInfo createInfo;
				{
					createInfo.physicalDevice_ = physicalDevice_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.renderPass_ = renderPass_;
					createInfo.colorAttachmentFormat_ = swapChain_->GetFormat().format;
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = imguiPipelineInfo->vertexShader_;
					createInfo.fragmentShader_ = imguiPipelineInfo->fragmentShader_;
					if (imguiPipelineInfo->enableDepthTest_) {
						auto depthTestData = std::make_shared<LinesPipeline::DepthTestInfo>();
						depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
						createInfo.depthTestInfo_ = depthTestData;
					}
				}
				imguiPipeline_ = std::make_shared<ImguiPipeline>(createInfo);
			}


			//PIPELINE for lines
			{
				auto linesPipelineInfo = info.linesPipeline_;

				LinesPipeline::CreateInfo createInfo;
				{
					createInfo.physicalDevice_ = physicalDevice_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.renderPass_ = renderPass_;
					createInfo.colorAttachmentFormat_ = swapChain_->GetFormat().format;
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = linesPipelineInfo->vertexShader_;
					createInfo.fragmentShader_ = linesPipelineInfo->fragmentShader_;
					createInfo.descriptorSetLayouts_.push_back(globalDataDSL_);
					createInfo.descriptorSetLayouts_.push_back(modelInfoDSL_);
					createInfo.descriptorSetLayouts_.push_back(texturedModelDSL_);
					if (linesPipelineInfo->enableDepthTest_) {
						auto depthTestData = std::make_shared<LinesPipeline::DepthTestInfo>();
						depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
						createInfo.depthTestInfo_ = depthTestData;
					}
				}
				linesPipeline_ = std::make_shared<LinesPipeline>(createInfo);
			}

			//PIPELINE for textured models
			{
				auto texturedPipelineInfo = info.texturedPipeline_;

				TexturedModelPipeline::CreateInfo createInfo;
				{
					createInfo.physicalDevice_ = physicalDevice_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.renderPass_ = renderPass_;
					createInfo.colorAttachmentFormat_ = swapChain_->GetFormat().format;
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = texturedPipelineInfo->vertexShader_;
					createInfo.fragmentShader_ = texturedPipelineInfo->fragmentShader_;
					createInfo.descriptorSetLayouts_.push_back(globalDataDSL_);
					createInfo.descriptorSetLayouts_.push_back(modelInfoDSL_);
					createInfo.descriptorSetLayouts_.push_back(texturedModelDSL_);
					if (texturedPipelineInfo->enableDepthTest_) {
						auto depthTestData = std::make_shared<TexturedModelPipeline::DepthTestInfo>();
						depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
						createInfo.depthTestInfo_ = depthTestData;
					}
				}
				texturedModelPipeline_ = std::make_shared<TexturedModelPipeline>(createInfo);

			}

			//PIPELINE
			{
				auto flatShadedPipelineInfo = info.flatShadedPipeline_;

				FlatShadedModelPipeline::CreateInfo createInfo;
				{
					createInfo.physicalDevice_ = physicalDevice_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.renderPass_ = renderPass_;
					createInfo.colorAttachmentFormat_ = swapChain_->GetFormat().format;
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = flatShadedPipelineInfo->vertexShader_;
					createInfo.fragmentShader_ = flatShadedPipelineInfo->fragmentShader_;
					createInfo.descriptorSetLayouts_.push_back(globalDataDSL_);
					createInfo.descriptorSetLayouts_.push_back(modelInfoDSL_);
					if (flatShadedPipelineInfo->enableDepthTest_) {
						auto depthTestData = std::make_shared<FlatShadedModelPipeline::DepthTestInfo>();
						depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
						createInfo.depthTestInfo_ = depthTestData;
					}
				}
				flatShadedModelPipeline_ = std::make_shared<FlatShadedModelPipeline>(createInfo);

			}

			{
				VkExtent2D extent = swapChain_->GetExtent();
				for (const auto& imageView : swapChain_->GetImageViews()) {
					FrameBuffer::CreateInfo createInfo;
					{
						createInfo.logicDevice_ = logicDevice_;
						createInfo.colorImageView_ = imageView;
						createInfo.extent_ = extent;
						createInfo.renderPass_ = *renderPass_;
						if (depthTestData_ != nullptr) {
							createInfo.depthBufferImageView_ = depthTestData_->imageView_;
						}
					}
					FrameBuffer frameBuffer{ createInfo };
					frameBuffers_.push_back(std::move(frameBuffer));
				}
				OS::LogInfo("/render/vulkan/driver/", "Frame buffers created successfuly.");
			}


			for (Common::Index i = 0; i < frameBuffers_.size(); i++) {
				DescriptorSet::CreateInfo createInfo;
				{
					//createInfo.buffer_ = uniformBuffers_[i];
					createInfo.descriptorPool_ = descriptorPool_;
					createInfo.DSL_ = globalDataDSL_;
					createInfo.logicDevice_ = logicDevice_;
					//createInfo.type_ = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
				auto descriptorSet = std::make_shared<DescriptorSet>(createInfo);
				descriptorSet->UpdateBufferWriteConfiguration(
					globalDataUBs_[i],
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					0, 0, globalDataUBs_[i]->GetSizeInBytes());
				globalDataDSs_.push_back(descriptorSet);
			}

			for (Common::Index i = 0; i < swapChain_->GetImages().size(); i++) {

				auto imageContext = std::make_shared<ImageContext>();
				{
					imageContext->index_ = static_cast<Common::UInt32>(i);
					imageContext->inRender_ = nullptr;
				}
				imageContexts_.push_back(imageContext);
			}

			for (Common::Index i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
				auto frameContext = std::make_shared<FrameContext>();
				{
					frameContext->logicDevice_ = logicDevice_;
					frameContext->swapChain_ = swapChain_;
					frameContext->imageContext_ = nullptr;

					Fence::CreateInfo fenceCreateInfo;
					{
						fenceCreateInfo.logicDevice_ = logicDevice_;
						fenceCreateInfo.flags_ = VK_FENCE_CREATE_SIGNALED_BIT;
					}
					frameContext->renderFinishedFence_ = std::make_shared<Fence>(fenceCreateInfo);
					frameContext->renderFinishedSemaphore_ = std::make_shared<Semaphore>(logicDevice_);
					frameContext->imageAvailableSemaphore_ = std::make_shared<Semaphore>(logicDevice_);
				}
				frameContexts_.push_back(frameContext);
			}


			{
				//ImGui_ImplVulkan_InitInfo init_info = {};
				//init_info.Instance = *instance_;
				//init_info.PhysicalDevice = *physicalDevice_;
				//init_info.Device = *logicDevice_;
				//init_info.QueueFamily = graphicsQueueFamily_.index_;
				//init_info.Queue = logicDevice_->GetGraphicsQueue();
				//init_info.PipelineCache = nullptr;
				//init_info.DescriptorPool = descriptorPool_->GetNative();
				//init_info.Allocator = nullptr;
				//init_info.MinImageCount = 2;
				//init_info.ImageCount = swapChain_->GetImagesNumber();
				//init_info.CheckVkResultFn = nullptr;
				//init_info.RenderPass = *renderPass_;//*flatShadedModelPipeline_->GetRenderPass();
				//ImGui_ImplVulkan_Init(&init_info);

				//ImGui_ImplVulkan_CreateFontsTexture();

			}

			OS::LogInfo("/render/vulkan/driver/", "Vulkan driver initialized successfuly.");
		}

		void DataCopy(
			std::shared_ptr<Buffer> bufferFrom,
			std::shared_ptr<Buffer> bufferTo,
			std::shared_ptr<LogicDevice> logicDevice,
			std::shared_ptr<CommandPool> commandPool) {

			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.logicDevice_ = logicDevice;
				commandBufferCreateInfo.commandPool_ = commandPool;
			}
			auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
			commandBuffer->Begin();
			commandBuffer->Copy(bufferFrom, bufferTo);
			commandBuffer->End();
			VkSubmitInfo submitInfo{};
			{
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
			}
			vkQueueSubmit(logicDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(logicDevice->GetGraphicsQueue());
		}

		void DataCopy(
			std::shared_ptr<Buffer> bufferFrom,
			std::shared_ptr<Image> imageTo,
			std::shared_ptr<LogicDevice> logicDevice,
			std::shared_ptr<CommandPool> commandPool) {

			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.logicDevice_ = logicDevice;
				commandBufferCreateInfo.commandPool_ = commandPool;
			}
			auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
			commandBuffer->Begin();
			commandBuffer->Copy(bufferFrom, imageTo);
			commandBuffer->End();
			VkSubmitInfo submitInfo{};
			{
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
			}
			vkQueueSubmit(logicDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(logicDevice->GetGraphicsQueue());
		}

		void ChangeImageLayout(
			std::shared_ptr<Image> image,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			std::shared_ptr<LogicDevice> logicDevice,
			std::shared_ptr<CommandPool> commandPool) {

			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.logicDevice_ = logicDevice;
				commandBufferCreateInfo.commandPool_ = commandPool;
			}
			auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
			commandBuffer->Begin();

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
				commandBuffer->ImageMemoryBarrier(
					image,
					oldLayout, newLayout,
					0, VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT
				);
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				commandBuffer->ImageMemoryBarrier(
					image,
					oldLayout, newLayout,
					VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				);
			}
			else {
				OS::NotImplemented();
			}

			commandBuffer->End();
			VkSubmitInfo submitInfo{};
			{
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
			}
			vkQueueSubmit(logicDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(logicDevice->GetGraphicsQueue());
		}



		void CreateVertexBuffer() const noexcept {
			Buffer::CreateInfo bufferCreateInfo;
			{
				bufferCreateInfo.logicDevice_ = logicDevice_;
				bufferCreateInfo.size_ = 10;
				bufferCreateInfo.usage_ = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			}
			auto buffer = std::make_shared<Buffer>(bufferCreateInfo);
		}

		void StartRender() override {

			for (Common::Index i = 0; i < frameBuffers_.size(); i++) {

				CommandBuffer::CreateInfo commandBufferCreateInfo;
				{
					commandBufferCreateInfo.logicDevice_ = logicDevice_;
					commandBufferCreateInfo.commandPool_ = commandPool_;
				}
				auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);

				commandBuffer->Begin();
				static VkClearValue clearValue{ 1.0, 1.0, 1.0, 0.0 };
				CommandBuffer::DepthBufferInfo depthBufferInfo;
				{
					const bool enableDepthTest = (depthTestData_ != nullptr);//)false;// createInfo_.flatShadedPipeline_->enableDepthTest_ || createInfo_.texturedPipeline_->enableDepthTest_;
					depthBufferInfo.enable = enableDepthTest;
					depthBufferInfo.clearValue_.depthStencil = { 1.f, 0 };
				}
				commandBuffer->BeginRenderPass(
					*renderPass_,
					frameBuffers_[i].GetNative(),
					swapChain_->GetExtent(),
					clearValue,
					depthBufferInfo);

				for (auto shape : coloredShapes_) {
					commandBuffer->BindPipeline(flatShadedModelPipeline_);
					commandBuffer->BindShape(shape);
					{
						std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
						descriptorSets.push_back(globalDataDSs_[i]);
						descriptorSets.push_back(shape->GetTransformDescriptorSet());
						commandBuffer->BindDescriptorSets(flatShadedModelPipeline_, descriptorSets);
					}
					commandBuffer->DrawShape(shape);
				}

				for (auto shape : texturedShapes_) {
					commandBuffer->BindPipeline(texturedModelPipeline_);
					commandBuffer->BindShape(shape);
					{
						std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
						descriptorSets.push_back(globalDataDSs_[i]);
						descriptorSets.push_back(shape->GetTransformDescriptorSet());
						descriptorSets.push_back(shape->GetTexture()->GetDescriptorSet());
						commandBuffer->BindDescriptorSets(texturedModelPipeline_, descriptorSets);
					}
					commandBuffer->DrawShape(shape);
				}

				for (auto shape : UIShapes_) {
					commandBuffer->BindPipeline(imguiPipeline_);
					commandBuffer->BindShape(shape);
					{
						std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
						descriptorSets.push_back(shape->GetTransformDescriptorSet());
						descriptorSets.push_back(shape->GetTexture()->GetDescriptorSet());
						commandBuffer->BindDescriptorSets(imguiPipeline_, descriptorSets);
					}
					commandBuffer->DrawShape(shape);
				}


				std::vector<Geom::Vertex3fc> lines{
					{ { 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f } },
					{ { 10.f, 0.f, 0.f }, { 1.f, 0.f, 0.f } },
					{ { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f } },
					{ { 0.f, 10.f, 0.f }, { 0.f, 1.f, 0.f } },
					{ { 0.f, 0.f, 0.f }, { 0.f, 0.f, 1.f } },
					{ { 0.f, 0.f, 10.f }, { 0.f, 0.f, 1.f } }
				};

				for (float x = -30.f; x < 30.f; x += 1.f) {
					lines.push_back({ { x, 0.f, -10.f }, { 0.f, 0.f, 1.f } });
					lines.push_back({ { x, 0.f, 10.f }, { 0.f, 0.f, 1.f } });
				}

				for (float z = -30.f; z < 30.f; z += 1.f) {
					lines.push_back({ { -10.f, 0.f, z }, { 0.f, 0.f, 1.f } });
					lines.push_back({ { 10.f, 0.f, z }, { 0.f, 0.f, 1.f } });
				}


				static auto vertexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, lines.size() * sizeof(Vertex3fc));
				vertexStagingBuffer->Fill(lines.data());
				static auto vertex3fcBuffer = std::make_shared<VertexBuffer<Vertex3fc>>(physicalDevice_, logicDevice_, lines.size());

				DataCopy(vertexStagingBuffer, vertex3fcBuffer, logicDevice_, commandPool_);

				commandBuffer->BindPipeline(linesPipeline_);
				commandBuffer->BindBuffer(vertex3fcBuffer);
				std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
				descriptorSets.push_back(globalDataDSs_[i]);
				commandBuffer->BindDescriptorSets(linesPipeline_, descriptorSets);
				commandBuffer->Draw(lines.size());

				commandBuffer->EndRenderPass();
				commandBuffer->End();

				commandBuffers_.push_back(commandBuffer);
			}

		}

		void Render() override {

			OS::AssertMessage(camera_ != nullptr, "Camera was not set to the vulkan driver.");

			std::shared_ptr<FrameContext> frameContext = frameContexts_[currentFrame];
			auto image = GetNextImage(frameContext->imageAvailableSemaphore_);
			frameContext->SetImage(image);
			frameContext->WaitForRenderToImageFinish();

			UpdateGlobalData(frameContext->imageContext_->index_);

			frameContext->Render();
			frameContext->ShowImage();

			frameContext->WaitForQueueIdle();

			currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		}

		void UpdateGlobalData(uint32_t currentImage) {

			GlobalData globalData{};
			globalData.lightIntensity_ = light_->GetIntensity();
			globalData.lightPos_ = light_->GetPosition();
			globalData.view = glm::lookAt(
				camera_->GetPosition(),
				camera_->GetPosition() + camera_->GetDirection(),
				camera_->GetUp()
			);
			globalData.proj = glm::perspective(
				glm::radians(45.0f),
				camera_->GetWidth() / (float)camera_->GetHeight(),
				camera_->GetNearPlane(), camera_->GetFarPlane());
			globalData.proj[1][1] *= -1;
			std::shared_ptr<UniformBuffer> currentUniformBuffer = globalDataUBs_[currentImage];

			currentUniformBuffer->Fill(&globalData);
		}


		void EndRender() override {
			commandBuffers_.clear();
			UIShapes_.clear();
		}

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const RAL::Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {

			auto vertexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, verticesNumber * sizeof(Vertex3fnc));
			vertexStagingBuffer->Fill(vertices);
			auto vertex3fncBuffer = std::make_shared<VertexBuffer<Vertex3fnc>>(physicalDevice_, logicDevice_, verticesNumber);

			DataCopy(vertexStagingBuffer, vertex3fncBuffer, logicDevice_, commandPool_);

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(Index16));
			indexStagingBuffer->Fill(indices);
			auto indexBuffer = std::make_shared<IndexBuffer<Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);

			const VkDeviceSize bufferSize = sizeof(Transform);
			auto transformUniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);
			DescriptorSet::CreateInfo transformDesciptorSetCreateInfo;
			{
				transformDesciptorSetCreateInfo.descriptorPool_ = descriptorPool_;
				transformDesciptorSetCreateInfo.DSL_ = flatShadedModelPipeline_->GetLayout()->GetDSLs()[0];
				transformDesciptorSetCreateInfo.logicDevice_ = logicDevice_;
			}
			auto modelTransform = std::make_shared<DescriptorSet>(transformDesciptorSetCreateInfo);
			modelTransform->UpdateBufferWriteConfiguration(
				transformUniformBuffer,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				0, 0, transformUniformBuffer->GetSizeInBytes());

			ColoredShape::CreateInfo shapeCreateInfo;
			{
				shapeCreateInfo.model_ = model_;
				shapeCreateInfo.vertexBuffer_ = vertex3fncBuffer;
				shapeCreateInfo.indexBuffer_ = indexBuffer;
				shapeCreateInfo.transformBuffer_ = transformUniformBuffer;
				shapeCreateInfo.modelDataDescriptorSet = modelTransform;
			}
			auto newShape = std::make_shared<ColoredShape>(shapeCreateInfo);
			coloredShapes_.push_back(newShape);
			return newShape->GetId();
		}


		virtual Common::UInt64 DrawIndexed(
			glm::mat3 model,
			const RAL::Vertex2ftc* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber,
			std::shared_ptr<RAL::Texture> texture) override {

			auto vertexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, verticesNumber * sizeof(Vertex2ftc));
			vertexStagingBuffer->Fill(vertices);
			auto vertex3fntBuffer = std::make_shared<VertexBuffer<Vertex2ftc>>(physicalDevice_, logicDevice_, verticesNumber);

			DataCopy(vertexStagingBuffer, vertex3fntBuffer, logicDevice_, commandPool_);

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(Index16));
			indexStagingBuffer->Fill(indices);
			auto indexBuffer = std::make_shared<IndexBuffer<RAL::Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);

			auto textureStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, texture->GetPixelsNumber() * sizeof(RAL::Color4b));
			textureStagingBuffer->Fill(texture->GetPixels<Color4b>());

			AllocatedTextureImage::CreateInfo textureImageCreateInfo;
			{
				textureImageCreateInfo.size_ = { texture->GetSize() };
				textureImageCreateInfo.format_ = VK_FORMAT_R8G8B8A8_UNORM;
				textureImageCreateInfo.logicDevice_ = logicDevice_;
				textureImageCreateInfo.physicalDevice_ = physicalDevice_;
			}
			auto textureImage = std::make_shared<AllocatedTextureImage>(textureImageCreateInfo);
			ChangeImageLayout(textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, logicDevice_, commandPool_);
			DataCopy(textureStagingBuffer, textureImage, logicDevice_, commandPool_);
			ChangeImageLayout(textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, logicDevice_, commandPool_);
			auto textureImageView = CreateImageViewByImage(logicDevice_, textureImage, VK_IMAGE_ASPECT_COLOR_BIT);
			Sampler::CreateInfo samplerCreateInfo;
			{
				samplerCreateInfo.logicDevice_ = logicDevice_;
				samplerCreateInfo.magFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.minFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.maxAnisotropy_ = physicalDevice_->GetProperties().limits.maxSamplerAnisotropy;
			}
			auto textureSampler = std::make_shared<Sampler>(samplerCreateInfo);

			DescriptorSet::CreateInfo createInfo;
			{
				createInfo.descriptorPool_ = descriptorPool_;
				createInfo.DSL_ = imguiPipeline_->GetLayout()->GetDSLs()[1];
				createInfo.logicDevice_ = logicDevice_;
			}

			auto textureDescriptorSet = std::make_shared<DescriptorSet>(createInfo);
			textureDescriptorSet->UpdateImageWriteConfiguration(
				textureImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				textureSampler,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				0);

			Texture::CreateInfo textureCreateInfo;
			{
				textureCreateInfo.textureImage_ = textureImage;
				textureCreateInfo.textureImageView_ = textureImageView;
				textureCreateInfo.sampler = textureSampler;
				textureCreateInfo.descriptorSet_ = textureDescriptorSet;
			}
			auto vkTexture = std::make_shared<Texture>(textureCreateInfo);
			const VkDeviceSize bufferSize = sizeof(glm::mat3);
			auto transformUniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);
			transformUniformBuffer->Fill(&model);
			DescriptorSet::CreateInfo transformDesciptorSetCreateInfo;
			{
				transformDesciptorSetCreateInfo.descriptorPool_ = descriptorPool_;
				transformDesciptorSetCreateInfo.DSL_ = imguiPipeline_->GetLayout()->GetDSLs()[0];
				transformDesciptorSetCreateInfo.logicDevice_ = logicDevice_;
			}
			auto modelTransform = std::make_shared<DescriptorSet>(transformDesciptorSetCreateInfo);

			modelTransform->UpdateBufferWriteConfiguration(
				transformUniformBuffer,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				0, 0, transformUniformBuffer->GetSizeInBytes());

			UIShape::CreateInfo texturedShapeCreateInfo;
			{
				texturedShapeCreateInfo.model_ = model;
				texturedShapeCreateInfo.transformBuffer_ = transformUniformBuffer;
				texturedShapeCreateInfo.transformDescriptorSet_ = modelTransform;
				texturedShapeCreateInfo.vertexBuffer_ = vertex3fntBuffer;
				texturedShapeCreateInfo.indexBuffer_ = indexBuffer;
				texturedShapeCreateInfo.texture_ = vkTexture;
			}
			auto texturedShape = std::make_shared<UIShape>(texturedShapeCreateInfo);
			//texturedShape->SetPosition({ 0, 0, 0 });
			UIShapes_.push_back(texturedShape);
			return texturedShape->GetId();
			//return 0;
		}

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const RAL::Vertex3fnt* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber,
			std::shared_ptr<RAL::Texture> texture) override {

			auto vertexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, verticesNumber * sizeof(Vertex3fnt));
			vertexStagingBuffer->Fill(vertices);
			auto vertex3fntBuffer = std::make_shared<VertexBuffer<Vertex3fnt>>(physicalDevice_, logicDevice_, verticesNumber);

			DataCopy(vertexStagingBuffer, vertex3fntBuffer, logicDevice_, commandPool_);

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(Index16));
			indexStagingBuffer->Fill(indices);
			auto indexBuffer = std::make_shared<IndexBuffer<RAL::Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);

			auto textureStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, texture->GetPixelsNumber() * sizeof(RAL::Color4b));
			textureStagingBuffer->Fill(texture->GetPixels<Color4b>());

			AllocatedTextureImage::CreateInfo textureImageCreateInfo;
			{
				textureImageCreateInfo.size_ = { texture->GetSize() };
				textureImageCreateInfo.format_ = VK_FORMAT_R8G8B8A8_UNORM;
				textureImageCreateInfo.logicDevice_ = logicDevice_;
				textureImageCreateInfo.physicalDevice_ = physicalDevice_;
			}
			auto textureImage = std::make_shared<AllocatedTextureImage>(textureImageCreateInfo);
			ChangeImageLayout(textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, logicDevice_, commandPool_);
			DataCopy(textureStagingBuffer, textureImage, logicDevice_, commandPool_);
			ChangeImageLayout(textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, logicDevice_, commandPool_);
			auto textureImageView = CreateImageViewByImage(logicDevice_, textureImage, VK_IMAGE_ASPECT_COLOR_BIT);
			Sampler::CreateInfo samplerCreateInfo;
			{
				samplerCreateInfo.logicDevice_ = logicDevice_;
				samplerCreateInfo.magFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.minFilter_ = VK_FILTER_LINEAR;
				samplerCreateInfo.maxAnisotropy_ = physicalDevice_->GetProperties().limits.maxSamplerAnisotropy;
			}
			auto textureSampler = std::make_shared<Sampler>(samplerCreateInfo);

			DescriptorSet::CreateInfo createInfo;
			{
				createInfo.descriptorPool_ = descriptorPool_;
				createInfo.DSL_ = texturedModelPipeline_->GetLayout()->GetDSLs()[2];
				createInfo.logicDevice_ = logicDevice_;
			}

			auto textureDescriptorSet = std::make_shared<DescriptorSet>(createInfo);
			textureDescriptorSet->UpdateImageWriteConfiguration(
				textureImageView,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				textureSampler,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				0);

			Texture::CreateInfo textureCreateInfo;
			{
				textureCreateInfo.textureImage_ = textureImage;
				textureCreateInfo.textureImageView_ = textureImageView;
				textureCreateInfo.sampler = textureSampler;
				textureCreateInfo.descriptorSet_ = textureDescriptorSet;
			}

			const VkDeviceSize bufferSize = sizeof(Transform);
			auto transformUniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);
			DescriptorSet::CreateInfo transformDesciptorSetCreateInfo;
			{
				transformDesciptorSetCreateInfo.descriptorPool_ = descriptorPool_;
				transformDesciptorSetCreateInfo.DSL_ = texturedModelPipeline_->GetLayout()->GetDSLs()[1];
				transformDesciptorSetCreateInfo.logicDevice_ = logicDevice_;
			}
			auto modelTransform = std::make_shared<DescriptorSet>(transformDesciptorSetCreateInfo);
			modelTransform->UpdateBufferWriteConfiguration(
				transformUniformBuffer,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				0, 0, transformUniformBuffer->GetSizeInBytes());

			TexturedShape::CreateInfo texturedShapeCreateInfo;
			{
				texturedShapeCreateInfo.model_ = model_;
				texturedShapeCreateInfo.transformBuffer_ = transformUniformBuffer;
				texturedShapeCreateInfo.transformDescriptorSet_ = modelTransform;
				texturedShapeCreateInfo.vertexBuffer_ = vertex3fntBuffer;
				texturedShapeCreateInfo.indexBuffer_ = indexBuffer;
				texturedShapeCreateInfo.texture_ = std::make_shared<Texture>(textureCreateInfo);;
			}
			auto texturedShape = std::make_shared<TexturedShape>(texturedShapeCreateInfo);
			//texturedShape->SetPosition({ 0, 0, 0 });
			texturedShapes_.push_back(texturedShape);
			return texturedShape->GetId();
		}

		//virtual void SetPosition(
		//	Common::Index shapeIndex,
		//	const Vector3f& newPosition) override {
		//	for (auto shape : coloredShapes_) {
		//		if (shape->GetId() == shapeIndex) {
		//			shape->SetPosition(newPosition);
		//		}
		//	}

		//	for (auto shape : texturedShapes_) {
		//		if (shape->GetId() == shapeIndex) {
		//			shape->SetPosition(newPosition);
		//		}
		//	}
		//}

		virtual void SetModelMatrix(
			Common::Index shapeIndex,
			const glm::mat4& modelMatrix) override {
			for (auto shape : coloredShapes_) {
				if (shape->GetId() == shapeIndex) {
					shape->SetModelMatrix(modelMatrix);
				}
			}

			for (auto shape : texturedShapes_) {
				if (shape->GetId() == shapeIndex) {
					shape->SetModelMatrix(modelMatrix);
				}
			}
		}

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const RAL::Vertex3fc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {

			OS::Assert(indicesNumber % 3 == 0);

			std::vector<RAL::Vertex3fnc> convertedVertices;
			for (Common::Index i = 0; i < indicesNumber; i += 3) {
				Vector3f first = vertices[1].position_ - vertices[0].position_;
				Vector3f second = vertices[1].position_ - vertices[2].position_;
				const RAL::Normal3f normal = first.CrossProduct(second).Normalize();
				convertedVertices.push_back({ vertices[0], normal });
				convertedVertices.push_back({ vertices[1], normal });
				convertedVertices.push_back({ vertices[2], normal });
			}


			auto vertexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, verticesNumber * sizeof(Vertex3fnc));
			vertexStagingBuffer->Fill(convertedVertices.data());
			auto vertexBuffer = std::make_shared<VertexBuffer<Vertex3fnc>>(physicalDevice_, logicDevice_, verticesNumber);

			DataCopy(vertexStagingBuffer, vertexBuffer, logicDevice_, commandPool_);

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(Index16));
			indexStagingBuffer->Fill(indices);
			auto indexBuffer = std::make_shared<IndexBuffer<Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);


			const VkDeviceSize bufferSize = sizeof(Transform);
			auto transformUniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);
			DescriptorSet::CreateInfo transformDesciptorSetCreateInfo;
			{
				transformDesciptorSetCreateInfo.descriptorPool_ = descriptorPool_;
				transformDesciptorSetCreateInfo.DSL_ = modelInfoDSL_;
				transformDesciptorSetCreateInfo.logicDevice_ = logicDevice_;
			}
			auto modelTransform = std::make_shared<DescriptorSet>(transformDesciptorSetCreateInfo);
			modelTransform->UpdateBufferWriteConfiguration(
				transformUniformBuffer,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				0, 0, transformUniformBuffer->GetSizeInBytes());

			ColoredShape::CreateInfo createInfo;
			{
				createInfo.model_ = model_;
				createInfo.vertexBuffer_ = vertexBuffer;
				createInfo.indexBuffer_ = indexBuffer;
				createInfo.transformBuffer_ = transformUniformBuffer;
				createInfo.modelDataDescriptorSet = modelTransform;
			}

			auto shape = std::make_shared<ColoredShape>(createInfo);
			coloredShapes_.push_back(shape);
			return shape->GetId();
		}

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const RAL::Vertex3f* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber,
			const RAL::Color3f& color) override {


			std::vector<Vertex3fnc> convertedVertices;
			for (Common::Index i = 0; i < indicesNumber; i += 3) {
				Math::Vector3f first = vertices[i + 1].position_ - vertices[i].position_;
				Math::Vector3f second = vertices[i + 1].position_ - vertices[i + 2].position_;
				const Normal3f normal = first.CrossProduct(second).Normalize();
				convertedVertices.push_back({ vertices[i].position_, normal, color });
				convertedVertices.push_back({ vertices[i + 1].position_, normal, color });
				convertedVertices.push_back({ vertices[i + 2].position_, normal, color });
			}

			auto vertexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, verticesNumber * sizeof(Vertex3fnc));
			vertexStagingBuffer->Fill(convertedVertices.data());
			auto vertexBuffer = std::make_shared<VertexBuffer<Vertex3fnc>>(physicalDevice_, logicDevice_, verticesNumber);

			DataCopy(vertexStagingBuffer, vertexBuffer, logicDevice_, commandPool_);

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(Index16));
			indexStagingBuffer->Fill(indices);
			auto indexBuffer = std::make_shared<IndexBuffer<Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);

			ColoredShape::CreateInfo createInfo;
			{
				createInfo.model_ = model_;
				createInfo.vertexBuffer_ = vertexBuffer;
				createInfo.indexBuffer_ = indexBuffer;
			}

			auto shape = std::make_shared<ColoredShape>(createInfo);
			coloredShapes_.push_back(shape);
			return shape->GetId();
		}

		[[nodiscard]]
		std::shared_ptr<ImageContext> GetNextImage(std::shared_ptr<Semaphore> imageAvailableSemaphore) noexcept {

			uint32_t imageIndex;
			VkCall(vkAcquireNextImageKHR(
				logicDevice_->GetHandle(),
				swapChain_->GetHandle(),
				UINT64_MAX,
				imageAvailableSemaphore->GetNative(),
				VK_NULL_HANDLE,
				&imageIndex),
				"Error while getting next swap chain image.");

			std::shared_ptr<ImageContext> imageContext = imageContexts_[imageIndex];
			imageContext->index_ = imageIndex;
			imageContext->commandBuffer_ = commandBuffers_[imageIndex];

			return imageContext;
		}

		static void ExecuteCommands(
			VkQueue queue,
			std::shared_ptr<Semaphore> waitBeforeExecuting,
			std::shared_ptr<Semaphore> semaphoreExecutionFinish,
			VkPipelineStageFlags waitStages,
			std::shared_ptr<CommandBuffer> commands,
			std::shared_ptr<Fence> fenceExecutionFinished) {

			VkSubmitInfo submitInfo{};
			{
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = &waitBeforeExecuting->GetNative();
				//VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
				submitInfo.pWaitDstStageMask = &waitStages;//waitStages;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commands->GetHandle();
				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = &semaphoreExecutionFinish->GetNative();

				VkCall(vkQueueSubmit(queue, 1, &submitInfo, fenceExecutionFinished->GetNative()),
					"Error while submitting commands.");
			}

		}

		void ShowImage(

			std::shared_ptr<Semaphore> waitBeforeShowing,
			std::shared_ptr<SwapChain> swapChain,
			uint32_t imageIndex) {

			VkPresentInfoKHR presentInfo{};
			{
				presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
				presentInfo.waitSemaphoreCount = 1;
				presentInfo.pWaitSemaphores = &waitBeforeShowing->GetNative();
				presentInfo.swapchainCount = 1;
				presentInfo.pSwapchains = &swapChain_->GetHandle();
				presentInfo.pImageIndices = &imageIndex;
				presentInfo.pResults = nullptr;

				VkCall(vkQueuePresentKHR(logicDevice_->GetPresentQueue(), &presentInfo),
					"Error while showing image.");
			}
		}

	private:
		size_t currentFrame = 0;

		const int MAX_FRAMES_IN_FLIGHT = 2;

		std::shared_ptr<Instance> instance_ = nullptr;
		std::shared_ptr<Debug> debug_ = nullptr;
		std::shared_ptr<WindowSurface> windowSurface_ = nullptr;
		std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
		std::shared_ptr<LogicDevice> logicDevice_ = nullptr;
		std::shared_ptr<SwapChain> swapChain_ = nullptr;
		std::shared_ptr<CommandPool> commandPool_ = nullptr;
		std::shared_ptr<RenderPass> renderPass_ = nullptr;


		std::shared_ptr<ImguiNativePipeline> imguiNativePipeline_ = nullptr;
		std::shared_ptr<ImguiPipeline> imguiPipeline_ = nullptr;
		std::shared_ptr<LinesPipeline> linesPipeline_ = nullptr;
		std::shared_ptr<TexturedModelPipeline> texturedModelPipeline_ = nullptr;
		std::shared_ptr<FlatShadedModelPipeline> flatShadedModelPipeline_ = nullptr;
		std::vector<FrameBuffer> frameBuffers_;
		std::vector<std::shared_ptr<UniformBuffer>> globalDataUBs_; // Global data uniform buffers.

		std::shared_ptr<DescriptorPool> descriptorPool_ = nullptr;
		std::vector<std::shared_ptr<DescriptorSet>> globalDataDSs_; // Global data descriptor sets.

		QueueFamily graphicsQueueFamily_;
		QueueFamily presentQueueFamily_;

		std::vector<std::shared_ptr<CommandBuffer>> commandBuffers_;
		std::vector<std::shared_ptr<ImageContext>> imageContexts_;
		std::vector<std::shared_ptr<FrameContext>> frameContexts_;

		std::shared_ptr<UniformBuffer> modelInfoBuffer_ = nullptr;
		std::shared_ptr<DescriptorSetLayout> globalDataDSL_ = nullptr; // Global Data descriptor set layout.
		std::shared_ptr<DescriptorSetLayout> modelInfoDSL_ = nullptr;
		std::shared_ptr<DescriptorSetLayout> texturedModelDSL_ = nullptr;
		std::shared_ptr<DescriptorSet> texturedModelInfoDescriptorSet_ = nullptr;

		std::vector<std::shared_ptr<UIShape>> UIShapes_;
		std::vector<std::shared_ptr<TexturedShape>> texturedShapes_;
		std::vector<std::shared_ptr<ColoredShape>> coloredShapes_;

		std::shared_ptr<DepthTestData> depthTestData_ = nullptr;
	};

}