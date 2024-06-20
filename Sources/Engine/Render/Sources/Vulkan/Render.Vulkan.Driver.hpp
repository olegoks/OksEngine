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

#include <Render.Vulkan.Shape.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <imconfig.h>
#include <ImgUtil.h>
#include <imgui_impl_vulkan.h>
#include <imgui_impl_glfw.h>


namespace Render::Vulkan {

	class Driver : public RAL::Driver {
	public:

		struct UniformBufferObject {
			alignas(16) Math::Matrix4x4f model_;
			alignas(16) Math::Matrix4x4f view_;
			alignas(16) Math::Matrix4x4f proj_;
			alignas(16) Math::Vector3f lightPos_;
			float lightIntensity_;
		};

		struct Transform {
			alignas(16) Math::Matrix4x4f model_;
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

			//DESCRIPTOR SET LAYOUT
			descriptorSetLayout_ = std::make_shared<DescriptorSetLayout>(logicDevice_);
			modelInfoDescriptorSetLayout_ = std::make_shared<DescriptorSetLayout>(logicDevice_);

			//DESCRIPTOR_POOL
			const Common::Size descriptorPoolSize = swapChain_->GetImagesNumber() * 2;
			descriptorPool_ = std::make_shared<DescriptorPool>(logicDevice_, descriptorPoolSize);

			//DEPTH BUFFER
			if(info.enableDepthTest_) {
				auto depthTestData = std::make_shared<DepthTestData>();
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
			}
			
			//PIPELINE
			Pipeline<Vertex3fnc>::CreateInfo pipelineCreateInfo;
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
				pipelineCreateInfo.colorAttachmentFormat_ = swapChain_->GetFormat().format;
				pipelineCreateInfo.colorAttachmentSize_ = swapChain_->GetSize();
				pipelineCreateInfo.descriptorSetLayouts_.push_back(descriptorSetLayout_);
				pipelineCreateInfo.descriptorSetLayouts_.push_back(modelInfoDescriptorSetLayout_);
				pipelineCreateInfo.vertexShader_ = vertexShader;
				pipelineCreateInfo.fragmentShader_ = fragmentShader;

				if (info.enableDepthTest_) {
					auto depthTestData = std::make_shared<Pipeline<Vertex3fnc>::DepthTestData>();
					depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
					pipelineCreateInfo.depthTestData_ = depthTestData;
				}
				pipeline3fnc_ = std::make_shared<Pipeline<Vertex3fnc>>(pipelineCreateInfo);
			}

			{
				VkRenderPass renderPass = *pipeline3fnc_->GetRenderPass();
				VkExtent2D extent = swapChain_->GetExtent();
				for (const auto& imageView : swapChain_->GetImageViews()) {
					FrameBuffer::CreateInfo createInfo;
					{
						createInfo.logicDevice_ = logicDevice_;
						createInfo.colorImageView_ = imageView;
						createInfo.extent_ = extent;
						createInfo.renderPass_ = renderPass;
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
					createInfo.buffer_ = uniformBuffers_[i];
					createInfo.descriptorPool_ = descriptorPool_;
					createInfo.descriptorSetLayout_ = descriptorSetLayout_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.type_ = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
				descriptorSets_.push_back(std::make_shared<DescriptorSet>(createInfo));
			}
			{
				DescriptorSet::CreateInfo createInfo;
				{
					const VkDeviceSize bufferSize = sizeof(Transform);
					modelInfoBuffer_ = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);
					createInfo.buffer_ = modelInfoBuffer_;
					createInfo.descriptorPool_ = descriptorPool_;
					createInfo.descriptorSetLayout_ = modelInfoDescriptorSetLayout_;
					createInfo.logicDevice_ = logicDevice_;
					createInfo.type_ = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
				modelInfoDescriptorSet_ = std::make_shared<DescriptorSet>(createInfo);
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
				ImGui::CreateContext();
				//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;//ImGuiConfigFlags_DockingEnable;
				GLFWwindow* window = std::any_cast<GLFWwindow*>(info.surface_.param1_);
				ImGui_ImplGlfw_InitForVulkan(window, true);
				ImGui_ImplVulkan_InitInfo init_info = {};
				init_info.Instance = *instance_;
				init_info.PhysicalDevice = *physicalDevice_;
				init_info.Device = *logicDevice_;
				init_info.QueueFamily = graphicsQueueFamily_.index_;
				init_info.Queue = logicDevice_->GetGraphicsQueue();
				init_info.PipelineCache = nullptr;
				init_info.DescriptorPool = descriptorPool_->GetNative();
				init_info.Allocator = nullptr;
				init_info.MinImageCount = 2;
				init_info.ImageCount = swapChain_->GetImagesNumber();
				init_info.CheckVkResultFn = nullptr;
				init_info.RenderPass = *pipeline3fnc_->GetRenderPass();
				ImGui_ImplVulkan_Init(&init_info);

				ImGui_ImplVulkan_CreateFontsTexture();

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
				static VkClearValue clearValue{ 1, 1.0, 0.0, 0.0 };
				CommandBuffer::DepthBufferInfo depthBufferInfo;
				{
					depthBufferInfo.enable = createInfo_.enableDepthTest_;
					depthBufferInfo.clearValue_.depthStencil = { 1.f, 0 };
				}
				commandBuffer->BeginRenderPass(
					*pipeline3fnc_->GetRenderPass(),
					frameBuffers_[i].GetNative(),
					swapChain_->GetExtent(),
					clearValue,
					depthBufferInfo);

				OS::Assert(vertexBuffers_.GetSize() == indexBuffers_.GetSize());
				for (Common::Index modelIndex = 0; modelIndex < vertexBuffers_.GetSize(); modelIndex++) {
					commandBuffer->BindPipeline(pipeline3fnc_);
					commandBuffer->BindBuffer(vertexBuffers_[modelIndex]);
					commandBuffer->BindBuffer(indexBuffers_[modelIndex]);
					{
						std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
						descriptorSets.push_back(descriptorSets_[i]);
						descriptorSets.push_back(modelInfoDescriptorSet_);
						commandBuffer->BindDescriptorSets(pipeline3fnc_, descriptorSets);
					}
					commandBuffer->DrawIndexed(indexBuffers_[modelIndex]->GetIndecesNumber());
				}

				for (auto shape : shapes_) {
					commandBuffer->BindPipeline(pipeline3fnc_);
					commandBuffer->BindShape(shape);
					{
						std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
						descriptorSets.push_back(descriptorSets_[i]);
						descriptorSets.push_back(modelInfoDescriptorSet_);
						commandBuffer->BindDescriptorSets(pipeline3fnc_, descriptorSets);
					}
					commandBuffer->DrawShape(shape);
				}

				ImGui_ImplVulkan_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				ImGui::Begin("My First ImGui Window");

				if (ImGui::Button("Click me!"))
				{
					// Respond to button click
					std::cout << "Button clicked!" << std::endl;
				}

				static float slider_value = 0.0f;
				ImGui::SliderFloat("Slider", &slider_value, 0.0f, 100.0f);

				ImGui::End();
				ImGui::Render();

				ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer, 0);

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
			SetModelTransform();
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

			Math::Vector3f vector{ 1.f, 0.f, 0.f };
			ubo.model_ = /*Math::Matrix4x4f::GetTranslate(Math::Vector3f{ 0, 0, 0 });Math::Matrix4x4f::GetIdentity();*/ Math::Matrix4x4f::GetRotate(time * 30.f, vector);
			Math::Vector3f position = camera_->GetPosition();
			Math::Vector3f direction = camera_->GetDirection();
			//ubo.view_ = Math::Matrix4x4f::GetView(position, direction, { 0.f, 0.f, 1.f });
			//ubo.proj_ = Math::Matrix4x4f::GetPerspective(45, swapChain_->GetExtent().width / (float)swapChain_->GetExtent().height, 0.1, 10);
			ubo.view_ = Math::Matrix4x4f::GetView(position, direction, { 0.f, 0.f, 1.f });
			ubo.proj_ = Math::Matrix4x4f::GetPerspective(30, camera_->GetWidth() / (float)camera_->GetHeight(), 1.f, 1000);

			ubo.proj_[1][1] *= -1;

			std::shared_ptr<UniformBuffer> currentUniformBuffer = uniformBuffers_[currentImage];
			currentUniformBuffer->Fill(&ubo);
		}

		void SetModelTransform(/*const Math::Matrix4x4f& transform*/) {

			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			Math::Vector3f vector{ 1.f, 0.f, 0.f };
			Transform newTransform; 
			{
				newTransform.model_ = Math::Matrix4x4f::GetRotate(time * - 30.f, vector);
			}
			modelInfoBuffer_->Fill(&newTransform);
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


		virtual void DrawShape(const RAL::Shape& shape) override {

			if (shape.GetVertexType() == RAL::VertexType::V3f_N3f_C3f) {

				DS::Vector<RAL::Vertex3fnc> vertices;
				shape.ForEachVertex<RAL::Vertex3fnc>([&vertices](const RAL::Vertex3fnc& vertex) {
						vertices.PushBack(vertex);
					});

				auto vertexStagingBuffer = std::make_shared<StagingBuffer>(
					physicalDevice_,
					logicDevice_,
					vertices.GetSize() * sizeof(RAL::Vertex3fnc));

				vertexStagingBuffer->Fill(vertices.GetData());

				auto vertex3fncBuffer = std::make_shared<VertexBuffer<RAL::Vertex3fnc>>(
					physicalDevice_,
					logicDevice_,
					vertices.GetSize());

				const DS::Vector<RAL::Index16> indices = shape.GetIndices();

				auto indexStagingBuffer = std::make_shared<StagingBuffer>(
					physicalDevice_,
					logicDevice_,
					indices.GetSize() * sizeof(RAL::Index16));


				indexStagingBuffer->Fill(indices.GetData());
				auto indexBuffer = std::make_shared<IndexBuffer<RAL::Index16>>(
					physicalDevice_,
					logicDevice_,
					indices.GetSize());
				
				CommandBuffer::CreateInfo commandBufferCreateInfo;
				{
					commandBufferCreateInfo.logicDevice_ = logicDevice_;
					commandBufferCreateInfo.commandPool_ = commandPool_;
				}
				
				auto commandBuffer = std::make_shared<CommandBuffer>(commandBufferCreateInfo);
				commandBuffer->Begin();
				commandBuffer->Copy(vertexStagingBuffer, vertex3fncBuffer);
				commandBuffer->Copy(indexStagingBuffer, indexBuffer);
				commandBuffer->End();

				VkSubmitInfo submitInfo{};
				{
					submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
					submitInfo.commandBufferCount = 1;
					submitInfo.pCommandBuffers = &commandBuffer->GetHandle();
				}
				vkQueueSubmit(logicDevice_->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(logicDevice_->GetGraphicsQueue());

				Shape::CreateInfo shapeCreateInfo;
				{
					shapeCreateInfo.vertexBuffer_ = vertex3fncBuffer;
					shapeCreateInfo.indexBuffer_ = indexBuffer;
				}
				auto newShape = std::make_shared<Shape>(shapeCreateInfo);
				shapes_.push_back(newShape);
			}
		}

		virtual void DebugDrawIndexed(
			const RAL::Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {
			for (Common::Index vertexIndex = 0; vertexIndex < verticesNumber; vertexIndex++) {
				const RAL::Vertex3fnc& vertex = vertices[vertexIndex];
				vertices_.Add(vertex);
			}
			indices_.Add(indices, indicesNumber, verticesNumber);

		}

		virtual void DrawIndexed(
			const RAL::Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {
			//for (Common::Index vertexIndex = 0; vertexIndex < verticesNumber; vertexIndex++) {
			//	const RAL::Vertex3fnc& vertex = vertices[vertexIndex];
			//	vertices_.Add(vertex);
			//}
			//indices_.Add(indices, indicesNumber, verticesNumber);


			auto vertexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, verticesNumber * sizeof(RAL::Vertex3fnc));
			vertexStagingBuffer->Fill(vertices);
			auto vertex3fncBuffer = std::make_shared<VertexBuffer<RAL::Vertex3fnc>>(physicalDevice_, logicDevice_, verticesNumber);

			DataCopy(vertexStagingBuffer, vertex3fncBuffer, logicDevice_, commandPool_);

			auto indexStagingBuffer = std::make_shared<StagingBuffer>(physicalDevice_, logicDevice_, indicesNumber * sizeof(RAL::Index16));
			indexStagingBuffer->Fill(indices);
			auto indexBuffer = std::make_shared<IndexBuffer<RAL::Index16>>(physicalDevice_, logicDevice_, indicesNumber);

			DataCopy(indexStagingBuffer, indexBuffer, logicDevice_, commandPool_);


			Shape::CreateInfo shapeCreateInfo;
			{
				shapeCreateInfo.vertexBuffer_ = vertex3fncBuffer;
				shapeCreateInfo.indexBuffer_ = indexBuffer;
			}
			auto newShape = std::make_shared<Shape>(shapeCreateInfo);
			shapes_.push_back(newShape);

			//vertexBuffers_.PushBack(vertex3fncBuffer);
			//indexBuffers_.PushBack(indexBuffer);
		}



		virtual void DrawIndexed(
			const RAL::Vertex3fc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {

			for (Common::Index vertexIndex = 0; vertexIndex < verticesNumber; vertexIndex++) {
				const RAL::Vertex3fc& vertex = vertices[vertexIndex];
				RAL::Vertex3fnc vertex3fnc{ vertex , RAL::Normal3f{ 0.f, 0.f, 0.f } };
				vertices_.Add(vertex3fnc);
			}
			indices_.Add(indices, indicesNumber, verticesNumber);

		}

		virtual void DrawIndexed(
			const RAL::Vertex3f* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber,
			const RAL::Color3f& color) override {

			for (Common::Index vertexIndex = 0; vertexIndex < verticesNumber; vertexIndex++) {
				RAL::Vertex3fnc vertex{ vertices[vertexIndex], color, RAL::Normal3f{ 0, 0, 0 } };
				vertices_.Add(vertex);
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
		std::shared_ptr<CommandBuffer> commandBuffer_ = nullptr;
		std::shared_ptr<Pipeline<Vertex3fnc>> pipeline3fnc_ = nullptr;
		std::vector<FrameBuffer> frameBuffers_;
		
		//std::shared_ptr<StagingBuffer> vertexStagingBuffer_ = nullptr;

		//std::shared_ptr<VertexBuffer<Vertex3fnc>> vertex3fncBuffer_ = nullptr;
		//std::shared_ptr<VertexBuffer<Vertex3fnc>> vertex3fncBuffer_ = nullptr;

		//std::shared_ptr<StagingBuffer> indexStagingBuffer_ = nullptr;
		//std::shared_ptr<IndexBuffer> indexBuffer_ = nullptr;
		std::vector<std::shared_ptr<UniformBuffer>> uniformBuffers_;
		std::shared_ptr<DescriptorSetLayout> descriptorSetLayout_ = nullptr;
		std::shared_ptr<DescriptorPool> descriptorPool_ = nullptr;
		std::vector<std::shared_ptr<DescriptorSet>> descriptorSets_;

		QueueFamily graphicsQueueFamily_;
		QueueFamily presentQueueFamily_;

		std::vector<std::shared_ptr<CommandBuffer>> commandBuffers_;
		std::vector<std::shared_ptr<ImageContext>> imageContexts_;
		std::vector<std::shared_ptr<FrameContext>> frameContexts_;

		std::shared_ptr<UniformBuffer> modelInfoBuffer_ = nullptr;
		std::shared_ptr<DescriptorSetLayout> modelInfoDescriptorSetLayout_ = nullptr;
		std::shared_ptr<DescriptorSet> modelInfoDescriptorSet_ = nullptr;

		DS::Vector<std::shared_ptr<VertexBuffer<RAL::Vertex3fnc>>> vertexBuffers_;
		DS::Vector<std::shared_ptr<IndexBuffer<RAL::Index16>>> indexBuffers_;

		std::vector<std::shared_ptr<Shape>> shapes_;

		std::shared_ptr<DepthTestData> depthTestData_ = nullptr;
	};

}