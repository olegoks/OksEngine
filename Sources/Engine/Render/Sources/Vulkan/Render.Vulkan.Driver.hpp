#pragma once

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

namespace Render::Vulkan {

	class Driver : public RAL::Driver {
	public:

		//struct CreateInfo {
		//	VkSurfaceKHR surface_;
		//	//std::shared_ptr<Resource::Resource> vertexShader_ = nullptr;
		//	//std::shared_ptr<Resource::Resource> fragmentShader_ = nullptr;
		//	//bool enableDepthBuffer_ = false;
		//};

		struct UniformBufferObject {
			alignas(16) Math::Matrix4x4f model_;
			alignas(16) Math::Matrix4x4f view_;
			alignas(16) Math::Matrix4x4f proj_;
			alignas(16) Math::Vector3f lightPos_;
			float lightIntensity_;
		};

		struct Transform {
			Math::Matrix4x4f model_;
			Math::Matrix4x4f view_;
			Math::Matrix4x4f projection_; 
			Math::Vector3f lightPos_;
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
					submitInfo.pCommandBuffers = &imageContext_->commandBuffer_->GetNative();
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
		static PhysicalDevice::Formats GetAvailablePhysicalDeviceSurfaceFormats(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

			uint32_t formatCount;
			VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->GetHandle(), windowSurface->GetHandle(), &formatCount, nullptr),
				"Error while getting number of available formats.");

			std::vector<VkSurfaceFormatKHR> formats;
			if (formatCount != 0) {
				formats.resize(formatCount);
				formats.shrink_to_fit();
				VkCall(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->GetHandle(), windowSurface->GetHandle(), &formatCount, formats.data()),
					"Error while getting available formats.");
			}

			return formats;
		}

		[[nodiscard]]
		static PhysicalDevice::PresentModes GetAvailablePresentModes(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

			std::vector<VkPresentModeKHR> presentModes;

			uint32_t presentModeCount;
			{
				VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->GetHandle(), windowSurface->GetHandle(), &presentModeCount, nullptr), 
					"Error while getting number of present modes.");
			}
			if (presentModeCount != 0) {
				presentModes.resize(presentModeCount);
				VkCall(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->GetHandle(), windowSurface->GetHandle(), &presentModeCount, presentModes.data()), 
					"Error while getting number of present modes.");
			}

			return presentModes;
		}
		
		[[nodiscard]]
		static VkPresentModeKHR ChoosePresentMode(const PhysicalDevice::PresentModes& availablePresentModes) noexcept {

			for (const auto& availablePresentMode : availablePresentModes) {
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
					return availablePresentMode;
				}
			}

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		[[nodiscard]]
		std::pair<QueueFamily, QueueFamily> ChooseQueueFamilies(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

			const QueueFamilies graphicsQueueFamilies = physicalDevice->GetGraphicsQueueFamilies();
			OS::AssertMessage(!graphicsQueueFamilies.IsEmpty(), "Chosen physical device doesn't have any graphics queue family.");

			const QueueFamilies presentQueueFamilies = GetPresentQueueFamilies(physicalDevice, windowSurface);
			OS::AssertMessage(!graphicsQueueFamilies.IsEmpty(), "Chosen physical device doesn't have any present queue family.");

			for (const QueueFamily& graphicsQueueFamily : graphicsQueueFamilies) {
				for (const QueueFamily& presentQueueFamily : presentQueueFamilies) {
					if (graphicsQueueFamily.index_ == presentQueueFamily.index_) {
						graphicsQueueFamily_ = graphicsQueueFamily;
						presentQueueFamily_ = presentQueueFamily;
						OS::LogInfo("/render/vulkan/driver/physical device", "Found queue family that supports present and graphics commands.");
					}
				}
			}

			graphicsQueueFamily_ = *graphicsQueueFamilies.begin();
			presentQueueFamily_ = *presentQueueFamilies.begin();

			return { graphicsQueueFamily_, presentQueueFamily_ };
		}

		[[nodiscard]]
		static QueueFamilies GetPresentQueueFamilies(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

			const QueueFamilies queueFamilies = physicalDevice->GetQueueFamilies();
			QueueFamilies presentQueueFamilies;

			for (const QueueFamily& queueFamily : queueFamilies) {
				VkBool32 presentSupport = false;
				VkCall(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice->GetHandle(), queueFamily.index_, windowSurface->GetHandle(), &presentSupport),
					"Error while checking queue family present commands support.");
				if (presentSupport) {
					presentQueueFamilies.AddQueueFamily(queueFamily);
				}
			}

			return presentQueueFamilies;
		}


		std::shared_ptr<PhysicalDevice> ChoosePhysicalDevice(const std::vector<std::shared_ptr<PhysicalDevice>>& availablePhysicalDevices, std::shared_ptr<WindowSurface> windowSurface, const Extensions& requiredExtensions) noexcept {

			auto isDeviceSuitable = [](
				std::shared_ptr<PhysicalDevice> physicalDevice,
				std::shared_ptr<WindowSurface> windowSurface,
				const Extensions& requiredExtensions) noexcept->bool {

					if (!physicalDevice->IsDiscrete()) {
						return false;
					}

					if (!physicalDevice->SupportsGeometryShader()) {
						return false;
					}

					auto areRequiredExtensionsAvailable = [](const Extensions& requiredExtensions, const Extensions& availableExtensions)noexcept->bool {

						for (const Extension& requiredExtension : requiredExtensions) {
							const bool requiredExtensionAvailable = availableExtensions.Contains(requiredExtension);
							if (!requiredExtensionAvailable) {
								return false;
							}
						}
						return true;
					};

					const Extensions availableExtensions = physicalDevice->GetAvailableExtensions();

					if (!areRequiredExtensionsAvailable(requiredExtensions, availableExtensions)) {
						return false;
					}

					const auto graphicsQueueFamilies = physicalDevice->GetGraphicsQueueFamilies();
					const auto presentQueueFamilies = GetPresentQueueFamilies(physicalDevice, windowSurface);

					if (graphicsQueueFamilies.IsEmpty() || presentQueueFamilies.IsEmpty()) {
						return false;
					}

					const PhysicalDevice::Formats formats = GetAvailablePhysicalDeviceSurfaceFormats(physicalDevice, windowSurface);
					if (formats.empty()) {
						return false;
					}

					const PhysicalDevice::PresentModes presentModes = GetAvailablePresentModes(physicalDevice, windowSurface);
					if (presentModes.empty()) {
						return false;
					}

					return true;
			};

			for (const auto& physicalDevice : availablePhysicalDevices) {
				if (isDeviceSuitable(physicalDevice, windowSurface, requiredExtensions)) {
					OS::LogInfo("render/vulkan/driver", { "Found sutable device {}.", physicalDevice->GetName()});
					return physicalDevice;
				}
			}

			OS::AssertFailMessage("Failed to find a suitable GPU!");
			return nullptr;
		}

		Driver(const CreateInfo& info) noexcept : RAL::Driver{ info }{

			auto windowInfo = info.surface_;

			Extensions requiredExtensions;
			requiredExtensions.AddExtension("VK_EXT_debug_utils");
			requiredExtensions.AddExtension("VK_EXT_debug_report");
			//requiredExtensions.AddExtension("VK_KHR_portability_subset");

			if(windowInfo.uiSubsystem_ == RAL::UISubsystem::GLFW) {
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

			Debug::CreateInfo debugCreateInfo;
			{
				debugCreateInfo.instance_ = instance_;
				debugCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				debug_ = std::make_shared<Debug>(debugCreateInfo);
			}

			LogicDevice::CreateInfo logicDeviceCreateInfo;
			{
				logicDeviceCreateInfo.physicalDevice_ = physicalDevice_;
				logicDeviceCreateInfo.requiredExtensions_ = requiredDeviceExtensions;
				logicDeviceCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				logicDeviceCreateInfo.presentQueueFamily_ = presentQueueFamily_;
				logicDeviceCreateInfo.graphicsQueueFamily_ = graphicsQueueFamily_;
				logicDevice_ = std::make_shared<LogicDevice>(logicDeviceCreateInfo);
			}

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


			CommandPool::CreateInfo commandPoolCreateInfo;
			{
				commandPoolCreateInfo.logicDevice_ = logicDevice_;
				commandPoolCreateInfo.queueFamily_ = graphicsQueueFamily_;
				commandPool_ = std::make_shared<CommandPool>(commandPoolCreateInfo);
			}

			{
				const VkDeviceSize bufferSize = sizeof(UniformBufferObject);
				const SwapChain::Images images = swapChain_->GetImages();
				const Common::Size imagesNumber = images.size();
				for (Common::Index i = 0; i < imagesNumber; i++) {
					auto uniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);
					uniformBuffers_.push_back(uniformBuffer);
				}
			}

			descriptorSetLayout_ = std::make_shared<DescriptorSetLayout>(logicDevice_);
			const Common::Size descriptorPoolSize = swapChain_->GetImages().size();
			descriptorPool_ = std::make_shared<DescriptorPool>(logicDevice_, descriptorPoolSize);

			Pipeline<Vertex3fc>::CreateInfo pipelineCreateInfo;
			{
				ShaderModule::CreateInfo vertexShaderModuleCreateInfo;
				{
					vertexShaderModuleCreateInfo.logicDevice_ = logicDevice_;
					DS::Vector<Common::Byte> spirv{ info.vertexShader_.GetCode(), info.vertexShader_.GetSize() };
					vertexShaderModuleCreateInfo.spirv_ = std::move(spirv);

				}
				auto vertexShader = std::make_shared<ShaderModule>(vertexShaderModuleCreateInfo);

				ShaderModule::CreateInfo fragmentShaderModuleCreateInfo;
				{
					fragmentShaderModuleCreateInfo.logicDevice_ = logicDevice_;
					DS::Vector<Common::Byte> spirv{ info.fragmentShader_.GetCode(), info.fragmentShader_.GetSize() };
					fragmentShaderModuleCreateInfo.spirv_ = std::move(spirv);

				}
				auto fragmentShader = std::make_shared<ShaderModule>(fragmentShaderModuleCreateInfo);

				pipelineCreateInfo.physicalDevice_ = physicalDevice_;
				pipelineCreateInfo.logicDevice_ = logicDevice_;
				pipelineCreateInfo.swapChain_ = swapChain_;
				pipelineCreateInfo.descriptorSetLayout_ = descriptorSetLayout_;
				pipelineCreateInfo.vertexShader_ = vertexShader;
				pipelineCreateInfo.fragmentShader_ = fragmentShader;
				pipelineCreateInfo.depthTest_ = createInfo_.enableDepthBuffer_;

			}

			pipeline3fc_ = std::make_shared<Pipeline<Vertex3fc>>(pipelineCreateInfo);

			{
				VkRenderPass renderPass = *pipeline3fc_->GetRenderPass();
				VkExtent2D extent = swapChain_->GetExtent();
				for (const auto& imageView : swapChain_->GetImageViews()) {
					FrameBuffer::CreateInfo createInfo;
					{
						createInfo.logicDevice_ = logicDevice_;
						createInfo.colorImageView_ = imageView;
						createInfo.extent_ = extent;
						createInfo.renderPass_ = renderPass;
						if (pipeline3fc_->IsDepthTestEnabled()) {
							createInfo.depthBufferImageView_ = pipeline3fc_->GetDepthBufferImageView();
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
					createInfo.buffer_ = uniformBuffers_[i];
					createInfo.descriptorPool_ = descriptorPool_;
					createInfo.descriptorSetLayout_ = descriptorSetLayout_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.type_ = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
				DescriptorSet descriptorSet{ createInfo };
				descriptorSets_.push_back(descriptorSet);
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
			OS::LogInfo("/render/vulkan/driver/", "Vulkan driver initialized successfuly.");
		}

		void DataCopy(std::shared_ptr<Buffer> bufferFrom, std::shared_ptr<Buffer> bufferTo, std::shared_ptr<LogicDevice> logicDevice, std::shared_ptr<CommandPool> commandPool) {

			VkCommandBufferAllocateInfo allocInfo{};
			{
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = commandPool->GetNative();
				allocInfo.commandBufferCount = 1;
			}

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(logicDevice->GetHandle(), &allocInfo, &commandBuffer);

			VkCommandBufferBeginInfo beginInfo{};
			{
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			}

			vkBeginCommandBuffer(commandBuffer, &beginInfo);
			VkBufferCopy copyRegion{};
			{
				copyRegion.srcOffset = 0;
				copyRegion.dstOffset = 0;
				copyRegion.size = bufferFrom->GetSizeInBytes();
			}
			vkCmdCopyBuffer(commandBuffer, bufferFrom->GetNative(), bufferTo->GetNative(), 1, &copyRegion);
			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			{
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;
			}
			vkQueueSubmit(logicDevice->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
			vkQueueWaitIdle(logicDevice->GetGraphicsQueue());

			vkFreeCommandBuffers(logicDevice->GetHandle(), commandPool->GetNative(), 1, &commandBuffer);
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

			//OS::Profiler profiler{ [](const OS:: Profiler::Info& info) {
			//	
			//	const Common::Size executionInMs = info.executionTime_.GetValue() / 1'000'000;
			//	if (executionInMs < 1) {
			//		return;
			//	}
			//	std::string message;
			//	{
			//		message += "Profiled function: %s\n";
			//		message += "Execution time: %d ms";
			//	}
			//	OS::LogInfo("render/vulkan", { message.c_str(), info.functionName_.c_str(),executionInMs });
			//} };

			vertexStagingBuffer_ = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, vertices_.GetSizeInBytes());
			vertexStagingBuffer_->Fill(vertices_.GetData()/*, vertices_.GetSizeInBytes()*/);
			vertex3fcBuffer_ = std::make_shared<VertexBuffer<Vertex3fc>/*<Vertex3fnñt>*/>(physicalDevice_, logicDevice_, vertices_.GetVerticesNumber());

			DataCopy(vertexStagingBuffer_, vertex3fcBuffer_, logicDevice_, commandPool_);

			indexStagingBuffer_ = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indices_.GetSizeInBytes());
			indexStagingBuffer_->Fill(indices_.GetData()/*, indices_.GetSizeInBytes()*/);
			indexBuffer_ = std::make_shared<IndexBuffer>(physicalDevice_, logicDevice_, indices_.GetIndicesNumber());
			DataCopy(indexStagingBuffer_, indexBuffer_, logicDevice_, commandPool_);

			for (Common::Index i = 0; i < frameBuffers_.size(); i++) {

				auto commandBuffer = std::make_shared<CommandBuffer>(commandPool_, logicDevice_);

				commandBuffer->Begin();
				static VkClearValue clearValue{ 0, 1.0, 0.0, 0.0 };
				CommandBuffer::DepthBufferInfo depthBufferInfo;
				{
					depthBufferInfo.enable = createInfo_.enableDepthBuffer_;
					depthBufferInfo.clearValue_.depthStencil = { 1.f, 0 };
				}
				commandBuffer->BeginRenderPass(
					*pipeline3fc_->GetRenderPass(),
					frameBuffers_[i].GetNative(),
					swapChain_->GetExtent(),
					clearValue,
					depthBufferInfo);
				commandBuffer->BindPipeline(pipeline3fc_);
				commandBuffer->BindBuffer(vertex3fcBuffer_);
				commandBuffer->BindBuffer(indexBuffer_);
				commandBuffer->BindDescriptorSet(pipeline3fc_, descriptorSets_[i].GetNative());
				commandBuffer->DrawIndexed(indexBuffer_->GetIndecesNumber());
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

			UpdateUniformBuffers(frameContext->imageContext_->index_);
			frameContext->Render();
			frameContext->ShowImage();
			
			frameContext->WaitForQueueIdle();

			currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		}

		void UpdateUniformBuffers(uint32_t currentImage) {
			Common::DiscardUnusedParameter(currentImage);

			UniformBufferObject ubo{};

			ubo.lightIntensity_ = light_->GetIntensity();
			ubo.lightPos_ = light_->GetPosition();

			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			Math::Vector3f vector{ 1.f, 1.f, 0.f };
			ubo.model_ = /*Math::Matrix4x4f::GetTranslate(Math::Vector3f{ 0, 0, 0 });Math::Matrix4x4f::GetIdentity();*/ Math::Matrix4x4f::GetRotate(time * 90.f, vector);
			Math::Vector3f position = camera_->GetPosition();
			Math::Vector3f direction = camera_->GetDirection();
			//ubo.view_ = Math::Matrix4x4f::GetView(position, direction, { 0.f, 0.f, 1.f });
			//ubo.proj_ = Math::Matrix4x4f::GetPerspective(45, swapChain_->GetExtent().width / (float)swapChain_->GetExtent().height, 0.1, 10);
			ubo.view_ = Math::Matrix4x4f::GetView(position, direction, { 0.f, 0.f, 1.f });
			ubo.proj_ = Math::Matrix4x4f::GetPerspective(135, camera_->GetWidth() / (float)camera_->GetHeight(), 0.01f, 100);

			ubo.proj_[1][1] *= -1;

			std::shared_ptr<UniformBuffer> currentUniformBuffer = uniformBuffers_[currentImage];
			currentUniformBuffer->Fill(&ubo);
		}

		void EndRender() override {
			vertices_.Clear();
			indices_.Clear();
			commandBuffers_.clear();
		}


		//virtual void DrawIndexed(
		//	const RAL::Vertex3fnñt* vertices,
		//	Common::Size verticesNumber,
		//	const RAL::Index16* indices,
		//	Common::Size indicesNumber,
		//	const RAL::Texture& texture) override {

		//	Common::DiscardUnusedParameter(texture);

		//	vertices_.Add(vertices, verticesNumber);
		//	indices_.Add(indices, indicesNumber, verticesNumber);

		//}

		//virtual void DrawIndexed(
		//	const RAL::Vertex3fnt* vertices,
		//	Common::Size verticesNumber,
		//	const RAL::Index16* indices,
		//	Common::Size indicesNumber, 
		//	const RAL::Texture& texture) override {

		//	Common::DiscardUnusedParameter(texture);

		//	for (Common::Index vertexIndex = 0; vertexIndex < verticesNumber; vertexIndex++) {
		//		const RAL::Vertex3fnt& vertex = vertices[vertexIndex];
		//		vertices_.Add({ vertex.position_, vertex.normal_, Math::Vector3f{ 0, 0, 0 }, vertex.texel_ });
		//	}
		//	indices_.Add(indices, indicesNumber, verticesNumber);

		//}

		virtual void DrawIndexed(
			const RAL::Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {
			for (Common::Index vertexIndex = 0; vertexIndex < verticesNumber; vertexIndex++) {
				const RAL::Vertex3fnc& vertex = vertices[vertexIndex];
				vertices3fnc_.Add({ vertex.position_, vertex.normal_, vertex.color_, Math::Vector2f{ 0, 0 } });
			}
			indices_.Add(indices, indicesNumber, verticesNumber);

		}


		virtual void DrawIndexed(
			const RAL::Vertex3fc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {

			vertices3fc_.Add(vertices, verticesNumber);
			indices_.Add(indices, indicesNumber, verticesNumber);

		}

		virtual void DrawIndexed(
			const RAL::Vertex3f* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber,
			const RAL::Color& color) override {

			for (Common::Index vertexIndex = 0; vertexIndex < verticesNumber; vertexIndex++) {
				vertices_.Add({ vertices[vertexIndex], color, });
			}

			indices_.Add(indices, indicesNumber, verticesNumber);

		}

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
				submitInfo.pCommandBuffers = &commands->GetNative();
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
		std::shared_ptr<Pipeline<Vertex3fc>> pipeline3fc_ = nullptr;
		std::vector<FrameBuffer> frameBuffers_;
		
		std::shared_ptr<StagingBuffer> vertexStagingBuffer_ = nullptr;

		std::shared_ptr<VertexBuffer<Vertex3fc>> vertex3fcBuffer_ = nullptr;
		std::shared_ptr<VertexBuffer<Vertex3fnc>> vertex3fncBuffer_ = nullptr;

		std::shared_ptr<StagingBuffer> indexStagingBuffer_ = nullptr;
		std::shared_ptr<IndexBuffer> indexBuffer_ = nullptr;
		std::vector<std::shared_ptr<UniformBuffer>> uniformBuffers_;
		std::shared_ptr<DescriptorSetLayout> descriptorSetLayout_ = nullptr;
		std::shared_ptr<DescriptorPool> descriptorPool_ = nullptr;
		std::vector<DescriptorSet> descriptorSets_;

		QueueFamily graphicsQueueFamily_;
		QueueFamily presentQueueFamily_;

		std::vector<std::shared_ptr<CommandBuffer>> commandBuffers_;
		std::vector<std::shared_ptr<ImageContext>> imageContexts_;
		std::vector<std::shared_ptr<FrameContext>> frameContexts_;
	};

}