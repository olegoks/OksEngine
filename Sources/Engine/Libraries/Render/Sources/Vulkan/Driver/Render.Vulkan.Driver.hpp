#pragma once

#include <map>
#include <utility>
#include <type_traits>

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
#include <Render.Vulkan.Driver.StorageBuffer.hpp>
#include <Render.Vulkan.Driver.StagingBuffer.hpp>
//
#include <Render.Vulkan.Driver.DescriptorPool.hpp>
#include <Render.Vulkan.Driver.DescriptorSetLayout.hpp>
#include <Render.Vulkan.Driver.DescriptorSet.hpp>
//
#include <Render.Vulkan.Driver.Pipeline.hpp>
#include <Render.Vulkan.Driver.ComputePipeline.hpp>
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

#include <Render.RALToVulkan.hpp>

namespace Render::Vulkan {

	class Driver : public RAL::Driver {
	private:
		size_t currentFrame = 0;

		static inline constexpr Common::Size concurrentFramesNumber = 2;

	public:

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
		};

		struct MultisamplingData {
			std::shared_ptr<Image> image_ = nullptr;
			std::shared_ptr<ImageView> imageView_ = nullptr;
		};

		class FrameContext {
		public:
			std::shared_ptr<LogicDevice> LD_ = nullptr;
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
					ASSERT_FMSG(imageContext_->inRender_ == renderFinishedFence_, "Fence of image and frame are different.");
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

					VK_CALL(vkQueueSubmit(LD_->GetGraphicsQueue(), 1, &submitInfo, renderFinishedFence_->GetHandle()),
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

					VK_CALL(vkQueuePresentKHR(LD_->GetPresentQueue(), &presentInfo),
						"Error while showing image.");
				}
			}

			void WaitForQueueIdle() const noexcept {
				VK_CALL(vkQueueWaitIdle(LD_->GetPresentQueue()),
					"Error while waitting for queue idle.");
			}

			~FrameContext() {

			}

		};

		virtual void FrameBufferResize(glm::u32vec2 newSize) override {
			vkQueueWaitIdle(objects_.LD_->GetPresentQueue());


			//objects_.frameBuffers_.clear();
			objects_.imageContexts_.clear();
			objects_.swapChain_.reset();
			objects_.frameContexts_.clear();
			currentFrame_.reset();
			//objects_.depthTestData_.reset();
			//objects_.multisamplingData_.reset();

			if (newSize.x == 0 || newSize.y == 0) {
				return;
			}

			//SWAPCHAIN
			SwapChain::CreateInfo swapChainCreateInfo;
			{
				swapChainCreateInfo.imageSize_ = newSize;
				swapChainCreateInfo.LD_ = objects_.LD_;
				swapChainCreateInfo.physicalDevice_ = objects_.physicalDevice_;
				swapChainCreateInfo.windowSurface_ = objects_.windowSurface_;
				swapChainCreateInfo.presentQueueFamily_ = objects_.presentQueueFamily_;
				swapChainCreateInfo.graphicsQueueFamily_ = objects_.graphicsQueueFamily_;
				const PhysicalDevice::Formats& availableFormats = GetAvailablePhysicalDeviceSurfaceFormats(objects_.physicalDevice_, objects_.windowSurface_);
				swapChainCreateInfo.format_ = ChooseSwapChainSurfaceFormat(availableFormats);
				swapChainCreateInfo.presentMode_ = ChoosePresentMode(GetAvailablePresentModes(objects_.physicalDevice_, objects_.windowSurface_));
				swapChainCreateInfo.capabilities_ = GetCapabilities(objects_.physicalDevice_, objects_.windowSurface_);
				swapChainCreateInfo.extent_ = ChooseSwapExtent(objects_.physicalDevice_, objects_.windowSurface_, newSize);
				objects_.swapChain_ = std::make_shared<SwapChain>(swapChainCreateInfo);
			}



			////Rendered buffer
			//{

			//	{
			//		Image::CreateInfo multisamplingCreateInfo;
			//		{
			//			multisamplingCreateInfo.physicalDevice_ = objects_.physicalDevice_;
			//			multisamplingCreateInfo.LD_ = objects_.LD_;
			//			multisamplingCreateInfo.format_ = objects_.swapChain_->GetFormat().format;
			//			multisamplingCreateInfo.size_ = objects_.swapChain_->GetSize();
			//			multisamplingCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
			//			multisamplingCreateInfo.usage_ = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			//			multisamplingCreateInfo.samplesCount_ = VK_SAMPLE_COUNT_1_BIT;//objects_.physicalDevice_->GetMaxUsableSampleCount();
			//		}
			//		objects_.renderedBufferImage_ = std::make_shared<AllocatedImage>(multisamplingCreateInfo);
			//		objects_.renderedBufferImageView_ = CreateImageViewByImage(objects_.LD_, objects_.renderedBufferImage_, VK_IMAGE_ASPECT_COLOR_BIT, 1);
			//	}
			//}

			////DEPTH BUFFER
			//{

			//	Image::CreateInfo depthImageCreateInfo;
			//	{
			//		depthImageCreateInfo.physicalDevice_ = objects_.physicalDevice_;
			//		depthImageCreateInfo.LD_ = objects_.LD_;
			//		depthImageCreateInfo.format_ = VK_FORMAT_D32_SFLOAT;
			//		depthImageCreateInfo.size_ = objects_.swapChain_->GetSize();
			//		depthImageCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
			//		depthImageCreateInfo.usage_ = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			//		depthImageCreateInfo.samplesCount_ = objects_.physicalDevice_->GetMaxUsableSampleCount();
			//	}
			//	std::shared_ptr<Image> image = std::make_shared<AllocatedImage>(depthImageCreateInfo);
			//	std::shared_ptr<ImageView> imageView = CreateImageViewByImage(objects_.LD_, image, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

			//	auto depthTestData = std::make_shared<DepthTestData>(image, imageView);
			//	objects_.depthTestData_ = depthTestData;

			//}

			//{
			//	VkExtent2D extent = objects_.swapChain_->GetExtent();
			//	for (const auto& imageView : objects_.swapChain_->GetImageViews()) {
			//		FrameBuffer::CreateInfo createInfo;
			//		{
			//			createInfo.LD_ = objects_.LD_;
			//			createInfo.attachments_.push_back(objects_.multisamplingData_->imageView_);
			//			createInfo.attachments_.push_back(objects_.depthTestData_->imageView_);
			//			createInfo.attachments_.push_back(imageView);

			//			createInfo.attachments_.push_back(objects_.renderedBufferImageView_);

			//			createInfo.extent_ = extent;
			//			createInfo.renderPass_ = objects_.renderPass_;
			//		}
			//		auto frameBuffer = std::make_shared<FrameBuffer>(createInfo);
			//		objects_.frameBuffers_.push_back(frameBuffer);
			//	}
			//	OS::LogInfo("/render/vulkan/driver/", "Frame buffers created successfuly.");
			//}

			for (Common::Index i = 0; i < objects_.swapChain_->GetImages().size(); i++) {

				auto imageContext = std::make_shared<ImageContext>();
				{
					imageContext->index_ = static_cast<Common::UInt32>(i);
					imageContext->inRender_ = nullptr;
				}
				objects_.imageContexts_.push_back(imageContext);
			}

			for (Common::Index i = 0; i < concurrentFramesNumber; i++) {
				auto frameContext = std::make_shared<FrameContext>();
				{
					frameContext->LD_ = objects_.LD_;
					frameContext->swapChain_ = objects_.swapChain_;
					frameContext->imageContext_ = nullptr;

					Fence::CreateInfo fenceCreateInfo;
					{
						fenceCreateInfo.LD_ = objects_.LD_;
						fenceCreateInfo.flags_ = VK_FENCE_CREATE_SIGNALED_BIT;
					}
					frameContext->renderFinishedFence_ = std::make_shared<Fence>(fenceCreateInfo);
					frameContext->renderFinishedSemaphore_ = std::make_shared<Semaphore>(objects_.LD_);
					frameContext->imageAvailableSemaphore_ = std::make_shared<Semaphore>(objects_.LD_);
				}
				objects_.frameContexts_.push_back(frameContext);
			}

		}


		struct Objects {
			std::shared_ptr<Instance> instance_ = nullptr;
			std::shared_ptr<Debug> debug_ = nullptr;
			std::shared_ptr<WindowSurface> windowSurface_ = nullptr;
			std::shared_ptr<PhysicalDevice> physicalDevice_ = nullptr;
			std::shared_ptr<LogicDevice> LD_ = nullptr;
			std::shared_ptr<SwapChain> swapChain_ = nullptr;
			std::shared_ptr<CommandPool> commandPool_ = nullptr;
			std::shared_ptr<RenderPass2> renderPass_ = nullptr;
			std::vector<std::shared_ptr<FrameBuffer>> frameBuffers_;
			std::shared_ptr<DescriptorPool> DP_ = nullptr;
			QueueFamily graphicsQueueFamily_;
			QueueFamily presentQueueFamily_;
			QueueFamily computeQueueFamily_;
			std::vector<std::shared_ptr<CommandBuffer>> commandBuffers_{ { nullptr } };
			std::vector<std::shared_ptr<ImageContext>> imageContexts_;
			std::vector<std::shared_ptr<FrameContext>> frameContexts_;
			std::shared_ptr<DepthTestData> depthTestData_ = nullptr;
			std::shared_ptr<MultisamplingData> multisamplingData_ = nullptr;

			std::shared_ptr<Image> renderedBufferImage_ = nullptr;
			std::shared_ptr<ImageView> renderedBufferImageView_ = nullptr;
			std::shared_ptr<DescriptorSet> renderedBufferDS_ = nullptr;

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
		std::array<QueueFamily, 3> ChooseQueueFamilies(
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

		static Common::Size GetElementSize(VkFormat format) {
			switch (format) {
			case VK_FORMAT_R8G8B8A8_UNORM:    return 4;  // 4 компоненты × 8 бит = 32 бита (4 байта)
			case VK_FORMAT_B8G8R8A8_UNORM:    return 4;
			case VK_FORMAT_R8G8B8A8_UINT:    return 4;  // 4 компоненты × 8 бит = 32 бита (4 байта)
			case VK_FORMAT_B8G8R8A8_UINT:    return 4;
			case VK_FORMAT_D32_SFLOAT:        return 4;  // 32 бита (4 байта)
			default:
				OS::NotImplemented();
				return 0;
			}
		}

		static VkVertexInputBindingDescription GetVertexBindingDescription(VertexType vertexType) {
			switch (vertexType) {
			case VertexType::VF3: {
				return Vertex3f::GetBindingDescription();
				break;
			}
			case VertexType::VF3_NF3_TF2: {
				return Vertex3fnt::GetBindingDescription();
				break;
			}
			case VertexType::VF3_NF3_TF2_BIDUB4_WUB4: {
				return Vertex3fntbw::GetBindingDescription();
				break;
			}
			case VertexType::VF3_CF3: {
				return Vertex3fc::GetBindingDescription();
				break;
			}
			case VertexType::VF3_NF3_CF4: {
				return Vertex3fnc::GetBindingDescription();
				break;
			}
			case VertexType::VF2_TF2_CF3: {
				return Vertex2ftc::GetBindingDescription();
				break;
			}
			default:
				ASSERT_FAIL_MSG("Invalid VertexType value used.");
				return VkVertexInputBindingDescription{	};
			};
		}

		static std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions(VertexType vertexType) {
			switch (vertexType) {
			case VertexType::VF3: {
				return Vertex3f::GetAttributeDescriptions();
				break;
			}
			case VertexType::VF3_NF3_TF2: {
				return Vertex3fnt::GetAttributeDescriptions();
				break;
			}
			case VertexType::VF3_NF3_TF2_BIDUB4_WUB4: {
				return Vertex3fntbw::GetAttributeDescriptions();
				break;
			}
			case VertexType::VF3_CF3: {
				return Vertex3fc::GetAttributeDescriptions();
				break;
			}
			case VertexType::VF3_NF3_CF4: {
				return Vertex3fnc::GetAttributeDescriptions();
				break;
			}
			case VertexType::VF2_TF2_CF3: {
				return Vertex2ftc::GetAttributeDescriptions();
				break;
			}
			default:
				ASSERT_FAIL_MSG("Invalid VertexType value used.");
				return {};
			};
		}

		Driver(const CreateInfo& info) noexcept
			: RAL::Driver{ info }{

			auto windowInfo = info.surface_;

			Extensions requiredExtensions;
#if !defined(NDEBUG)
			requiredExtensions.AddExtension("VK_EXT_debug_utils");
			requiredExtensions.AddExtension("VK_EXT_debug_report");
			//requiredExtensions.AddExtension("VK_KHR_portability_subset");
#endif
			if (windowInfo.uiSubsystem_ == RAL::UISubsystem::GLFW) {
				const Common::UInt32 extensionsCount = std::any_cast<Common::UInt32>(windowInfo.param2_);
				const char** extensions = std::any_cast<const char**>(windowInfo.param3_);
				for (Common::UInt32 i = 0; i < extensionsCount; i++) {
					requiredExtensions.AddExtension(extensions[i]);
				}
			}

			ValidationLayers requiredValidationLayers;
#if !defined(NDEBUG)
			requiredValidationLayers.AddLayer("VK_LAYER_KHRONOS_validation");
#endif
			//requiredValidationLayers.AddLayer("printf_verbose");
			//requiredValidationLayers.AddLayer("VK_LAYER_LUNARG_api_dump");
			//requiredValidationLayers.AddLayer("VK_LAYER_KHRONOS_profiles");

			Instance::CreateInfo instanceCreateInfo;
			{
				instanceCreateInfo.requiredExtensions_ = requiredExtensions;
				instanceCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				objects_.instance_ = std::make_shared<Instance>(instanceCreateInfo);
			}

			SetObjectName = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(*objects_.instance_, "vkSetDebugUtilsObjectNameEXT");


			WindowSurface::CreateInfo windowSurfaceCreateInfo;
			{
				windowSurfaceCreateInfo.instance_ = objects_.instance_;
				windowSurfaceCreateInfo.renderSurface_ = info.surface_;
				objects_.windowSurface_ = std::make_shared<WindowSurface>(windowSurfaceCreateInfo);
			}

			Extensions requiredDeviceExtensions;
			requiredDeviceExtensions.AddExtension("VK_KHR_swapchain");
			//requiredDeviceExtensions.AddExtension("VK_KHR_get_physical_device_properties2");

#if defined(SHADER_DEBUG_PRINTF)
			requiredDeviceExtensions.AddExtension("VK_KHR_shader_non_semantic_info");
#endif
			const auto availablePhysicalDevices = objects_.instance_->GetPhysicalDevices();
			objects_.physicalDevice_ = ChoosePhysicalDevice(availablePhysicalDevices, objects_.windowSurface_, requiredDeviceExtensions);

			std::array<QueueFamily, 3> queueFamilies = ChooseQueueFamilies(objects_.physicalDevice_, objects_.windowSurface_);
			objects_.graphicsQueueFamily_ = queueFamilies[0];
			objects_.presentQueueFamily_ = queueFamilies[1];
			objects_.computeQueueFamily_ = queueFamilies[2];

			objects_.commandBuffers_.resize(1, nullptr);


			//DEBUG
			Debug::CreateInfo debugCreateInfo;
			{
				debugCreateInfo.instance_ = objects_.instance_;
				debugCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				objects_.debug_ = std::make_shared<Debug>(debugCreateInfo);
			}

			//LOGIC_DEVICE
			LogicDevice::CreateInfo logicDeviceCreateInfo;
			{
				logicDeviceCreateInfo.physicalDevice_ = objects_.physicalDevice_;
				logicDeviceCreateInfo.requiredExtensions_ = requiredDeviceExtensions;
				logicDeviceCreateInfo.requiredValidationLayers_ = requiredValidationLayers;
				logicDeviceCreateInfo.presentQueueFamily_ = objects_.presentQueueFamily_;
				logicDeviceCreateInfo.graphicsQueueFamily_ = objects_.graphicsQueueFamily_;
				logicDeviceCreateInfo.computeQueueFamily_ = objects_.computeQueueFamily_;
				objects_.LD_ = std::make_shared<LogicDevice>(logicDeviceCreateInfo);
			}

			//SWAPCHAIN
			SwapChain::CreateInfo swapChainCreateInfo;
			{
				swapChainCreateInfo.imageSize_ = info.surface_.size_;
				swapChainCreateInfo.LD_ = objects_.LD_;
				swapChainCreateInfo.physicalDevice_ = objects_.physicalDevice_;
				swapChainCreateInfo.windowSurface_ = objects_.windowSurface_;
				swapChainCreateInfo.presentQueueFamily_ = objects_.presentQueueFamily_;
				swapChainCreateInfo.graphicsQueueFamily_ = objects_.graphicsQueueFamily_;
				const PhysicalDevice::Formats& availableFormats = GetAvailablePhysicalDeviceSurfaceFormats(objects_.physicalDevice_, objects_.windowSurface_);
				swapChainCreateInfo.format_ = ChooseSwapChainSurfaceFormat(availableFormats);
				swapChainCreateInfo.presentMode_ = ChoosePresentMode(GetAvailablePresentModes(objects_.physicalDevice_, objects_.windowSurface_));
				swapChainCreateInfo.capabilities_ = GetCapabilities(objects_.physicalDevice_, objects_.windowSurface_);
				swapChainCreateInfo.extent_ = ChooseSwapExtent(objects_.physicalDevice_, objects_.windowSurface_, info.surface_.size_);
				objects_.swapChain_ = std::make_shared<SwapChain>(swapChainCreateInfo);
			}

			//COMMAND_POOL
			CommandPool::CreateInfo commandPoolCreateInfo;
			{
				commandPoolCreateInfo.LD_ = objects_.LD_;
				commandPoolCreateInfo.queueFamily_ = objects_.graphicsQueueFamily_;
				objects_.commandPool_ = std::make_shared<CommandPool>(commandPoolCreateInfo);
			}

			//DESCRIPTOR_POOL
			const Common::Size descriptorPoolSize = objects_.swapChain_->GetImagesNumber() * 2000;
			objects_.DP_ = std::make_shared<DescriptorPool>(objects_.LD_, descriptorPoolSize);

			/*RenderPacket::CreateInfo rpci{
				.PD_ = objects_.physicalDevice_,
				.LD_ = objects_.LD_,
				.DP_ = objects_.DP_,
				.swapChain_ = objects_.swapChain_,
				.framesInFlight_ = concurrentFramesNumber
			};

			render_ = std::make_unique<RenderPacket>(rpci);*/

			objects_.commandBuffers_.resize(concurrentFramesNumber, nullptr);

			//Post-effects subpass pipeline
			//{
			//	//https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers
			//	const char* vertexShaderCode =
			//		"#version 450\n"
			//		"layout(location = 0) out vec2 outUV;\n"
			//		"void main()\n"
			//		"{\n"
			//		"outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);\n"
			//		"gl_Position = vec4(outUV * 2.0f + -1.0f, 0.0f, 1.0f);\n"
			//		"}";

			//	Shader::CreateInfo VSCI{
			//		.ralCreateInfo_ = RAL::Shader::CreateInfo{
			//			.name_ = "Post effect vertex shader",
			//			.type_ = RAL::Shader::Type::Vertex,
			//			.code_ = vertexShaderCode
			//		}
			//	};
			//	auto VS = std::make_shared<Shader>(VSCI);


			//	const char* fragmentShaderCode =
			//		"#version 450\n"
			//		"layout(set = 0, binding = 0) uniform sampler2D samplerColor;\n"
			//		"layout(location = 0) in vec2 inUV;\n"
			//		"layout(location = 0) out vec4 outColor;\n"
			//		"void main() {\n"
			//		"float weight[5];\n"
			//		"weight[0] = 0.227027;\n"
			//		"weight[1] = 0.1945946;\n"
			//		"weight[2] = 0.1216216;\n"
			//		"weight[3] = 0.054054;\n"
			//		"weight[4] = 0.016216;\n"

			//		"vec2 tex_offset = 1.0 / textureSize(samplerColor, 0) * 1.0; // gets size of single texel\n"
			//		"vec3 result = texture(samplerColor, inUV).rgb * weight[0]; // current fragment's contribution\n"
			//		"for (int i = 1; i < 5; ++i)\n"
			//		"{\n"
			//		"//if (blurdirection == 1)\n"
			//		"{\n"
			//		"// H\n"
			//		"	result += texture(samplerColor, inUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * 0.5;\n"
			//		"	result += texture(samplerColor, inUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i] * 0.5;\n"
			//		"}\n"
			//		"//else\n"
			//		"//{\n"
			//		"// V\n"
			//		"//result += texture(samplerColor, inUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i] * ubo.blurStrength;\n"
			//		"//result += texture(samplerColor, inUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i] * ubo.blurStrength;\n"
			//		"//}\n"
			//		"}\n"
			//		"outColor = vec4(result, 1.0);\n"
			//		"}\n";

			//	Shader::CreateInfo FSCI{
			//		.ralCreateInfo_ = RAL::Shader::CreateInfo{
			//			.name_ = "Post effect fragment shader",
			//			.type_ = RAL::Shader::Type::Fragment,
			//			.code_ = fragmentShaderCode
			//		}
			//	};
			//	auto FS = std::make_shared<Shader>(FSCI);


			//	std::shared_ptr<Vulkan::Pipeline::DepthTestInfo> depthTestData;
			//	{
			//		depthTestData = std::make_shared<Vulkan::Pipeline::DepthTestInfo>();
			//		depthTestData->enable_ = false;
			//		depthTestData->bufferFormat_ = VK_FORMAT_D32_SFLOAT;//render_->renderPasses_[0].//objects_.depthTestData_->image_->GetFormat();
			//		depthTestData->compareOperation_ = ToVulkanType(RAL::Driver::DepthBuffer::CompareOperation::Less);
			//	}
			//	std::shared_ptr<Vulkan::Pipeline::MultisampleInfo> multisampleInfo;
			//	{
			//		multisampleInfo = std::make_shared<Vulkan::Pipeline::MultisampleInfo>();
			//		multisampleInfo->samplesCount_ = objects_.physicalDevice_->GetMaxUsableSampleCount();
			//	}
			//	Vulkan::Pipeline::CreateInfo createInfo{
			//		.physicalDevice_ = objects_.physicalDevice_,
			//		.LD_ = objects_.LD_,
			//		.renderPass_ = render_->renderPasses_[1].renderPass_,
			//		.pushConstants_ = {},
			//		.descriptorSetLayouts_ = std::vector{ render_->gBufferInfo_->dsl_ },
			//		.vertexShader_ = std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
			//			objects_.LD_,
			//			VS->GetSpirv()
			//			}),
			//		.fragmentShader_ = std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
			//			createInfo.LD_,
			//			FS->GetSpirv()
			//			}),
			//		.depthTestInfo_ = depthTestData,
			//		.colorAttachmentSize_ = objects_.swapChain_->GetSize(),
			//		.colorAttachmentFormat_ = objects_.swapChain_->GetFormat().format,
			//		.multisampleInfo_ = multisampleInfo,
			//		.subpassIndex_ = 0,
			//		.vertexInfo_ = nullptr,
			//		.topology_ = ToVulkanType(RAL::Driver::TopologyType::TriangleList),
			//		.frontFace_ = ToVulkanType(RAL::Driver::FrontFace::CounterClockwise),
			//		.cullMode_ = ToVulkanType(RAL::Driver::CullMode::Front),
			//		.dynamicStates_ = {  VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR }
			//	};


			//	const Pipeline::Id pipelineId = pipelineIdsGenerator_.Generate();

			//	auto pipeline = std::make_shared<Vulkan::Pipeline>(createInfo);
			//	idPipeline_[pipelineId] = pipeline;
			//}

			//for (const auto& [name, pipeline] : info.namePipelineDescriptions_) {

			//}



			for (Common::Index i = 0; i < objects_.swapChain_->GetImages().size(); i++) {

				auto imageContext = std::make_shared<ImageContext>();
				{
					imageContext->index_ = static_cast<Common::UInt32>(i);
					imageContext->inRender_ = nullptr;
				}
				objects_.imageContexts_.push_back(imageContext);
			}

			for (Common::Index i = 0; i < concurrentFramesNumber; i++) {
				auto frameContext = std::make_shared<FrameContext>();
				{
					frameContext->LD_ = objects_.LD_;
					frameContext->swapChain_ = objects_.swapChain_;
					frameContext->imageContext_ = nullptr;

					Fence::CreateInfo fenceCreateInfo;
					{
						fenceCreateInfo.LD_ = objects_.LD_;
						fenceCreateInfo.flags_ = VK_FENCE_CREATE_SIGNALED_BIT;
					}
					frameContext->renderFinishedFence_ = std::make_shared<Fence>(fenceCreateInfo);
					frameContext->renderFinishedSemaphore_ = std::make_shared<Semaphore>(objects_.LD_);
					frameContext->imageAvailableSemaphore_ = std::make_shared<Semaphore>(objects_.LD_);
				}
				objects_.frameContexts_.push_back(frameContext);
			}
			OS::LogInfo("/render/vulkan/driver/", "Vulkan driver initialized successfuly.");
		}

		std::shared_ptr<FrameContext> currentFrame_ = nullptr;
		std::shared_ptr<ImageContext> image_ = nullptr;
		std::shared_ptr<CommandBuffer> GCB_ = nullptr; // Graphics Command buffer
		std::shared_ptr<CommandBuffer> CCB_ = nullptr; // Compute Command buffer

		[[nodiscard]]
		virtual std::shared_ptr<RAL::Driver::Shader> CreateShader(const RAL::Driver::Shader::CreateInfo& createInfo) const override {

			Vulkan::Shader::CreateInfo vulkanShaderCreateInfo{
				.ralCreateInfo_ = createInfo
			};

			return std::make_shared<Vulkan::Shader>(vulkanShaderCreateInfo);
		}


		virtual Pipeline::Id CreatePipeline(const Pipeline::CI& pipelineCI) override {

			//Descriptor set layouts
			std::vector<std::shared_ptr<DescriptorSetLayout>> DSLs;
			for (auto& binding : pipelineCI.shaderBindings_) {
				std::vector<VkDescriptorSetLayoutBinding> bindings;
				VkDescriptorSetLayoutBinding vBinding{
					binding.binding_,
					ToVulkanType(binding.type_),
					1,
					static_cast<VkFlags>(ToVulkanType(binding.stage_)),
					nullptr
				};
				bindings.push_back(vBinding);

				auto DSL = std::make_shared<DescriptorSetLayout>(
					DescriptorSetLayout::CreateInfo{
						binding.name_,
						objects_.LD_,
						bindings
					});
				DSLs.push_back(DSL);
			}

			//Push constants
			std::vector<VkPushConstantRange> vkPushConstantRanges;
			for (auto ralPushConstant : pipelineCI.pushConstants_) {

				VkPushConstantRange vkPushConstantRange{
					ToVulkanType(ralPushConstant.shaderStage_),
					ralPushConstant.offset_,
					ralPushConstant.size_

				};

				vkPushConstantRanges.push_back(vkPushConstantRange);
			}

			std::shared_ptr<Vulkan::Pipeline::DepthTestInfo> depthTestData;
			//if (pipelineCI.enableDepthTest_) {
			depthTestData = std::make_shared<Vulkan::Pipeline::DepthTestInfo>();
			depthTestData->enable_ = pipelineCI.enableDepthTest_;
			depthTestData->bufferFormat_ = VK_FORMAT_D32_SFLOAT;// objects_.depthTestData_->image_->GetFormat();
			depthTestData->compareOperation_ = ToVulkanType(pipelineCI.dbCompareOperation_);
			//}

			/*std::shared_ptr<Vulkan::Pipeline::MultisampleInfo> multisampleInfo;
			{
				multisampleInfo = std::make_shared<Vulkan::Pipeline::MultisampleInfo>();
				multisampleInfo->samplesCount_ = objects_.physicalDevice_->GetMaxUsableSampleCount();
			}*/

			std::shared_ptr<Vulkan::Pipeline::VertexInfo> vertexInfo = nullptr;
			if(pipelineCI.vertexType_ != VertexType::Undefined){
				vertexInfo = std::make_shared<Vulkan::Pipeline::VertexInfo>(
					GetVertexBindingDescription(pipelineCI.vertexType_),
					GetVertexAttributeDescriptions(pipelineCI.vertexType_)
				);
			}

			auto renderPass = idRenderPass_[pipelineCI.renderPassId_];

			//Multisampling info.
			std::shared_ptr<Vulkan::Pipeline::MultisampleInfo> multisampleInfo = nullptr;
			if (pipelineCI.multisamplingInfo_ != nullptr) {
				multisampleInfo = std::make_shared<Vulkan::Pipeline::MultisampleInfo>();
				multisampleInfo->samplesCount_ = ToVulkanType(pipelineCI.multisamplingInfo_->samplesCount_);
			}

			Vulkan::Pipeline::CreateInfo createInfo{
				.physicalDevice_ = objects_.physicalDevice_,
				.LD_ = objects_.LD_,
				.renderPass_ = renderPass,//(pipelineCI.name_ != "ImGui Pipeline") ? (render_->renderPasses_[0].renderPass_) : (render_->renderPasses_[2].renderPass_),//objects_.renderPass_,
				.pushConstants_ = vkPushConstantRanges,
				.descriptorSetLayouts_ = DSLs,
				.vertexShader_ = std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					objects_.LD_,
					Common::pointer_cast<Vulkan::Shader>(pipelineCI.vertexShader_)->GetSpirv()
					}),
				.geometryShader_ = (pipelineCI.geometryShader_ != nullptr) ? (std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					objects_.LD_,
					Common::pointer_cast<Vulkan::Shader>(pipelineCI.geometryShader_)->GetSpirv()
					})) : (nullptr),
				.fragmentShader_ = std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					createInfo.LD_,
					Common::pointer_cast<Vulkan::Shader>(pipelineCI.fragmentShader_)->GetSpirv()
					}),
				.depthTestInfo_ = depthTestData,
				.colorAttachmentSize_ = objects_.swapChain_->GetSize(),
				.colorAttachmentFormat_ = objects_.swapChain_->GetFormat().format,
				.multisampleInfo_ = multisampleInfo,
				.subpassIndex_ = 0,
				.vertexInfo_ = vertexInfo,
				.topology_ = ToVulkanType(pipelineCI.topologyType_),
				.frontFace_ = ToVulkanType(pipelineCI.frontFace_),
				.cullMode_ = ToVulkanType(pipelineCI.cullMode_),
				.dynamicStates_ = {  VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR }
			};

			const Pipeline::Id pipelineId = pipelineIdsGenerator_.Generate();

			idPipeline_[pipelineId] = std::make_shared<Vulkan::Pipeline>(createInfo);

			return pipelineId;
		}

		virtual ComputePipeline::Id CreateComputePipeline(const ComputePipeline::CI& pipelineCI) override {

			std::vector<std::shared_ptr<DescriptorSetLayout>> DSLs;
			for (auto& binding : pipelineCI.shaderBindings_) {
				std::vector<VkDescriptorSetLayoutBinding> bindings;
				VkDescriptorSetLayoutBinding vBinding{
					binding.binding_,
					ToVulkanType(binding.type_),
					1,
					static_cast<VkFlags>(ToVulkanType(binding.stage_)),
					nullptr
				};
				bindings.push_back(vBinding);

				auto DSL = std::make_shared<DescriptorSetLayout>(
					DescriptorSetLayout::CreateInfo{
						binding.name_,
						objects_.LD_,
						bindings
					});
				DSLs.push_back(DSL);
			}

			std::vector<VkPushConstantRange> vkPushConstantRanges;
			for (auto ralPushConstant : pipelineCI.pushConstants_) {

				VkPushConstantRange vkPushConstantRange{
					ToVulkanType(ralPushConstant.shaderStage_),
					ralPushConstant.offset_,
					ralPushConstant.size_

				};

				vkPushConstantRanges.push_back(vkPushConstantRange);
			}

			Vulkan::ComputePipeline::CreateInfo createInfo{
				.name_ = pipelineCI.name_,
				.physicalDevice_ = objects_.physicalDevice_,
				.LD_ = objects_.LD_,
				.pushConstants_ = vkPushConstantRanges,
				.descriptorSetLayouts_ = DSLs,
				.computeShader_ = std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
					objects_.LD_,
					Common::pointer_cast<Vulkan::Shader>(pipelineCI.computeShader_)->GetSpirv()
					}),
			};

			const ComputePipeline::Id pipelineId = computePipelineIdsGenerator_.Generate();

			idComputePipeline_[pipelineId] = std::make_shared<Vulkan::ComputePipeline>(createInfo);

			return pipelineId;
		}

		virtual RP::Id CreateRenderPass(const RenderPass::CI& rpCI) override {

			auto createAttachment = [](RAL::Driver::RP::AttachmentUsage attachmentUsage) {

				Vulkan::RP::Attachment swapChainAttachment{
					.format = ToVulkanType(attachmentUsage.format_),
					.samples = ToVulkanType(attachmentUsage.samplesCount_),
					.loadOp = ToVulkanType(attachmentUsage.loadOperation_),
					.storeOp = ToVulkanType(attachmentUsage.storeOperation_),
					.initialLayout = ToVulkanType(attachmentUsage.initialState_),
					.finalLayout = ToVulkanType(attachmentUsage.finalState_),
					
				};

				return swapChainAttachment;
				};

			//Attachments. Need to save attachments order.
			std::vector<Vulkan::RP::Attachment> vkAttachments;
			for (const auto& ralAttachment : rpCI.attachments_) {
				vkAttachments.push_back(createAttachment(ralAttachment));

			}

			std::vector<Vulkan::RP::Subpass::Dependency> subpassDependecies;
			{
				Vulkan::RP::Subpass::Dependency inExternalToFirstSubpassDependency{
					.srcSubpass_ = VK_SUBPASS_EXTERNAL,
					.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstSubpass_ = 0,
					.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
					.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT,
				};
				subpassDependecies.push_back(inExternalToFirstSubpassDependency);

				if (rpCI.subpasses_.size() > 1) {
					for (Common::UInt32 i = 1; i < rpCI.subpasses_.size(); i++) {

						Vulkan::RP::Subpass::Dependency dependency{
						.srcSubpass_ = i - 1,
						.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
						.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
						.dstSubpass_ = i,
						.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
						.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT,
						};

						subpassDependecies.push_back(dependency);
					}
				}

				Vulkan::RP::Subpass::Dependency lastSubpassToOutExternalDependency{
					.srcSubpass_ = static_cast<Common::UInt32>(rpCI.subpasses_.size()) - 1,
					.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstSubpass_ = VK_SUBPASS_EXTERNAL,
					.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
					.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT
				};


				subpassDependecies.push_back(lastSubpassToOutExternalDependency);
			}

			std::vector<Vulkan::RP::Subpass> vkSubpasses;
			{
				for (const auto& subpass : rpCI.subpasses_) {

					std::vector<VkAttachmentReference> colorAttachmentRefs;
					for (const auto& index : subpass.colorAttachments_) {
						colorAttachmentRefs.push_back(VkAttachmentReference{
							.attachment = index,
							.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
							});
					}


					std::vector<VkAttachmentReference> inputAttachmentRefs;
					for (const auto& index : subpass.inputAttachments_) {
						inputAttachmentRefs.push_back(VkAttachmentReference{
							.attachment = index,
							.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
							});
					}


					Vulkan::RP::Subpass vkSubpass{
						.inputAttachments_ = { inputAttachmentRefs },
						.colorAttachments_ = { colorAttachmentRefs },
						.resolveAttachment_
						= (subpass.resolveAttachment_ != Common::Limits<Common::UInt32>::Max())
						? (std::make_shared<VkAttachmentReference>(subpass.resolveAttachment_,VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)) : (nullptr),
						.depthStencilAttachment_
						= (subpass.depthStencilAttachment_ != Common::Limits<Common::UInt32>::Max())
						? (std::make_shared<VkAttachmentReference>(subpass.depthStencilAttachment_, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)) : (nullptr)
					};
					vkSubpasses.push_back(vkSubpass);
				}
			}


			Vulkan::RenderPass2::CreateInfo rp2ci{
				.name_ = rpCI.name_,
				.LD_ = objects_.LD_,
				.attachments_ = vkAttachments,
				.subpasses_ = vkSubpasses,
				.dependencies_ = subpassDependecies

			};

			auto renderPass = std::make_shared<RenderPass2>(rp2ci);

			const RenderPass::Id rpId = renderPassIdsGenerator_.Generate();

#pragma region Assert
			ASSERT_FMSG(!idRenderPass_.contains(rpId), "");
#pragma endregion

			idRenderPass_[rpId] = renderPass;

			return rpId;
		}

		virtual RP::AttachmentSet::Id CreateAttachmentSet(const RP::AttachmentSet::CI& attachmentSetCI) override {

			std::vector<std::shared_ptr<Vulkan::Texture>> textures;

			for (RAL::Driver::Texture::Id textureId : attachmentSetCI.textures_) {
				auto texture = GetTextureById(textureId);
				textures.push_back(texture);
			}

			auto renderPass = idRenderPass_[attachmentSetCI.rpId_];

			FrameBuffer::CreateInfo createInfo;
			{
				createInfo.LD_ = objects_.LD_;

				for (auto texture : textures) {
					createInfo.attachments_.push_back(texture->GetImageView());
				}

				createInfo.extent_ = { attachmentSetCI.size_.x, attachmentSetCI.size_.y };
				createInfo.renderPass_ = renderPass;
			}
			std::shared_ptr<FrameBuffer> frameBuffer = std::make_shared<FrameBuffer>(createInfo);

			const auto newFrameBufferId = framebufferIdsGenerator_.Generate();
			idFrameBuffer_[newFrameBufferId] = frameBuffer;

			return newFrameBufferId;

		}

		void StartRender() override {

			if (currentFrame_ != 0) {
				currentFrame_->WaitForQueueIdle();
			}

			if (objects_.frameContexts_.empty()) {
				return;
			}

			if (objects_.frameContexts_.empty()) {
				return;
			}
			currentFrame_ = objects_.frameContexts_[currentFrame];
			image_ = GetNextImage(currentFrame_->imageAvailableSemaphore_);



			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.LD_ = objects_.LD_;
				commandBufferCreateInfo.commandPool_ = objects_.commandPool_;
				commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			}
			GCB_ = std::make_shared<CommandBuffer>(commandBufferCreateInfo);


			GCB_->Begin();

			VkMemoryBarrier barrier{
				.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
				.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,    // Запись с хоста
				.dstAccessMask = VK_ACCESS_SHADER_READ_BIT   // Чтение в шейдере
			};

			vkCmdPipelineBarrier(
				GCB_->GetHandle(),
				VK_PIPELINE_STAGE_HOST_BIT,              // srcStageMask: Ждем завершения записи хоста
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,     // dstStageMask: Блокируем вершинный шейдер
				0,
				1, // memoryBarrierCount
				&barrier,
				0, nullptr, // bufferMemoryBarrierCount
				0, nullptr  // imageMemoryBarrierCount
			);

		}



		virtual void BeginRenderPass(
			RP::Id renderPassId,
			RP::AttachmentSet::Id attachmentsId,
			const std::vector<RP::ClearValue>& clearValues,
			std::pair<Common::Int32, Common::Int32> offset,
			std::pair<Common::UInt32, Common::UInt32> area) override {

			Common::DiscardUnusedParameter(offset);
			if (GCB_ == nullptr) {
				return;
			}

			auto renderPass = GetRenderPassById(renderPassId);
			auto framebuffer = GetFrameBufferById(attachmentsId);

			std::vector<VkClearValue> vkClearValues;
			{
				for (const auto& clearValue : clearValues) {
					VkClearValue vkClearValue{0};
					{
						vkClearValue.color.float32[0] = clearValue.color_.float32[0];
						vkClearValue.color.float32[1] = clearValue.color_.float32[1];
						vkClearValue.color.float32[2] = clearValue.color_.float32[2];
						vkClearValue.color.float32[3] = clearValue.color_.float32[3];
						vkClearValue.depthStencil.depth = clearValue.depthStencil_.depth_;
					}
					vkClearValues.push_back(vkClearValue);
				}
			}
			GCB_->BeginRenderPass(renderPass, framebuffer, { offset.first, offset.second }, { area.first, area.second }, vkClearValues);
		}

		virtual void NextSubpass() override {
			GCB_->NextSubpass();
		}

		virtual void SetViewport(
			Common::UInt32 x,
			Common::UInt32 y,
			Common::UInt32 width,
			Common::UInt32 height) override {

			if (GCB_ == nullptr) {
				return;
			}

			const VkViewport viewport{
				.x = (float)x,
				.y = (float)y,
				.width = static_cast<float>(width),
				.height = static_cast<float>(height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f
			};
			GCB_->SetViewport(viewport);

		}

		virtual void SetScissor(
			Common::Int32 x,
			Common::Int32 y,
			Common::UInt32 width,
			Common::UInt32 height) override {

			if (GCB_ == nullptr) {
				return;
			}

			const VkRect2D scissor{
				.offset = { x, y },
				.extent = { width, height }
			};

			GCB_->SetScissor(scissor);

		}



		virtual void BindPipeline(RAL::Driver::Pipeline::Id pipelineId) override {

			if (GCB_ == nullptr) {
				return;
			}

			GCB_->BindPipeline(idPipeline_[pipelineId]);
			//contexts_.back().pipeline_ = idPipeline_[pipelineId];
		}


		virtual void BindVertexBuffer(
			VertexBuffer::Id VBId,
			Common::UInt64 offset) override {

			if (GCB_ == nullptr) {
				return;
			}

			GCB_->BindBuffer(GetVB(VBId), offset);
		}

		virtual void BindIndexBuffer(
			IndexBuffer::Id IBId,
			Common::UInt64 offset) override {

			if (GCB_ == nullptr) {
				return;
			}


			GCB_->BindBuffer(GetIB(IBId), offset);
		}

		virtual void Bind(RAL::Driver::Pipeline::Id pipelineId, Common::UInt32 firstResourceIndex, const std::vector<Resource::Id>& resourceIds) override {

			if (GCB_ == nullptr) {
				return;
			}

			std::vector<std::shared_ptr<DescriptorSet>> dss{ };

			for (Resource::Id resourceId : resourceIds) {
				std::shared_ptr<Vulkan::Driver::ResourceSet> resource = resources_[resourceId];

#pragma region Assert
				ASSERT_FMSG(resource != nullptr, "Attempt to bind resource with incorrect id.");
#pragma endregion

				std::shared_ptr<DescriptorSet> ds = resource->dss_[currentFrame];
				dss.push_back(ds);
			}

			auto pipeline = idPipeline_[pipelineId];

			GCB_->BindDescriptorSets(
				pipeline, firstResourceIndex, dss);
		}

		virtual void PushConstants(
			RAL::Driver::Pipeline::Id pipelineId,
			RAL::Driver::Shader::Stage stage,
			Common::Size sizeInBytes,
			void* data) override {

			if (GCB_ == nullptr) {
				return;
			}
			auto pipeline = idPipeline_[pipelineId];

			GCB_->PushConstants(pipeline->GetLayout(), stage, sizeInBytes, data);
		}

		virtual void DrawIndexed(
			Common::Size indicesNumber,
			Common::UInt32 firstInstance,
			Common::UInt32 instanceCount) override {
			/*	VkQueryPoolCreateInfo queryPoolInfo = {
		.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO,
		.queryType = VK_QUERY_TYPE_OCCLUSION,
		.queryCount = 1,
				};
				vkCreateQueryPool(*objects_.LD_, &queryPoolInfo, nullptr, &queryPool);*/

			if (GCB_ == nullptr) {
				return;
			}

			GCB_->DrawIndexed(indicesNumber, firstInstance, instanceCount);
		}

		virtual void Draw(Common::Size verticesNumber) override {
			if (GCB_ == nullptr) {
				return;
			}

			GCB_->Draw(verticesNumber);
		}

		virtual void EndRenderPass() override {

			if (GCB_ == nullptr) {
				return;
			}

			GCB_->EndRenderPass();

		}

		virtual void Show(RAL::Driver::Texture::Id textureId) override {
			//image_->
			if (GCB_ == nullptr) {
				return;
			}
			auto image = objects_.swapChain_->GetImages()[image_->index_];
			auto textureToShow = GetTextureById(textureId);

			VkOffset3D srcOffset[2];
			srcOffset[0] = { 0, 0, 0 };
			srcOffset[1] = { (int)textureToShow->GetWidth(), (int)textureToShow->GetHeight(), 1 };

			VkOffset3D dstOffset[2];
			dstOffset[0] = { 0, 0, 0 };
			dstOffset[1] = { (int)objects_.swapChain_->GetExtent().width, (int)objects_.swapChain_->GetExtent().height, 1 };

			//Image with final render from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			GCB_->ImageMemoryBarrier(
				textureToShow->GetImage()->GetHandle(), 0, 1,
				VK_IMAGE_ASPECT_COLOR_BIT,

				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,

				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

 			GCB_->ImageMemoryBarrier(image, 0, 1,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

			GCB_->ImageBlit(
				textureToShow->GetImage()->GetHandle(),
				srcOffset,//textureToShow->GetSize(),
				0,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,

				image,
				dstOffset,
				0,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			GCB_->ImageMemoryBarrier(image, 0, 1,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

			GCB_->ImageMemoryBarrier(
				textureToShow->GetImage()->GetHandle(), 0, 1,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
		}

		void EndRender() override {

			if (GCB_ == nullptr) {
				return;
			}

			//for (auto& context : contexts_) {
			//	CB_->ExecuteCommands(context.commandBuffer_);
			//}

			//CB_->EndRenderPass();
			GCB_->End(); 


			objects_.commandBuffers_[currentFrame] = std::move(GCB_);
			image_->commandBuffer_ = objects_.commandBuffers_[currentFrame];

			currentFrame_->SetImage(image_);
			BEGIN_PROFILE("Wait for render finish.");
			currentFrame_->WaitForRenderToImageFinish();
			END_PROFILE();
			BEGIN_PROFILE("Submit new frame.");
			currentFrame_->Render();
			END_PROFILE();
			BEGIN_PROFILE("Show image.");
			currentFrame_->ShowImage();
			END_PROFILE();


			currentFrame = (currentFrame + 1) % concurrentFramesNumber;


			contexts_.clear();

		}

		void WaitRenderEnd() override {
			BEGIN_PROFILE("Wait for render end.");
			currentFrame_->WaitForQueueIdle();
			END_PROFILE();
		}

		[[nodiscard]]
		std::shared_ptr<ImageContext> GetNextImage(std::shared_ptr<Semaphore> imageAvailableSemaphore) noexcept {

			uint32_t imageIndex;
			VK_CALL(vkAcquireNextImageKHR(
				objects_.LD_->GetHandle(),
				objects_.swapChain_->GetHandle(),
				UINT64_MAX,
				imageAvailableSemaphore->GetNative(),
				VK_NULL_HANDLE,
				&imageIndex),
				"Error while getting next swap chain image.");

			std::shared_ptr<ImageContext> imageContext = objects_.imageContexts_[imageIndex];
			imageContext->index_ = imageIndex;
			imageContext->commandBuffer_ = objects_.commandBuffers_[currentFrame];

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

				VK_CALL(vkQueueSubmit(queue, 1, &submitInfo, fenceExecutionFinished->GetHandle()),
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
				presentInfo.pSwapchains = &objects_.swapChain_->GetHandle();
				presentInfo.pImageIndices = &imageIndex;
				presentInfo.pResults = nullptr;

				VK_CALL(vkQueuePresentKHR(objects_.LD_->GetPresentQueue(), &presentInfo),
					"Error while showing image.");
			}
		}

		//Compute pipeline

		void StartCompute() override {

			CommandBuffer::CreateInfo commandBufferCreateInfo;
			{
				commandBufferCreateInfo.LD_ = objects_.LD_;
				commandBufferCreateInfo.commandPool_ = objects_.commandPool_;
				commandBufferCreateInfo.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			}
			CCB_ = std::make_shared<CommandBuffer>(commandBufferCreateInfo);


			CCB_->Begin();
			CCB_->BeginDebug("Compute pipeline");
		}


		virtual void BindComputePipeline(RAL::Driver::ComputePipeline::Id pipelineId) override {

			if (CCB_ == nullptr) {
				return;
			}

			CCB_->BindPipeline(idComputePipeline_[pipelineId]);
			//contexts_.back().pipeline_ = idPipeline_[pipelineId];
		}

		virtual void ComputeBind(RAL::Driver::Pipeline::Id pipelineId, Common::UInt32 firstResourceIndex, const std::vector<Resource::Id>& resourceIds) override {

			if (CCB_ == nullptr) {
				return;
			}

			std::vector<std::shared_ptr<DescriptorSet>> dss{ };

			for (Resource::Id resourceId : resourceIds) {
				std::shared_ptr<Vulkan::Driver::ResourceSet> resource = resources_[resourceId];

#pragma region Assert
				ASSERT_FMSG(resource != nullptr, "Attempt to bind resource with incorrect id.");
#pragma endregion

				std::shared_ptr<DescriptorSet> ds = resource->dss_[currentFrame];
				dss.push_back(ds);
			}

			auto pipeline = idComputePipeline_[pipelineId];

			ASSERT_MSG(!dss.empty(), "Attempt to bind empty array with Descriptor Sets.");
			CCB_->BindComputeDescriptorSets(
				pipeline, firstResourceIndex, dss);
		}

		virtual void ComputePushConstants(
			RAL::Driver::Pipeline::Id pipelineId,
			Common::Size sizeInBytes, void* data) override {

			if (CCB_ == nullptr) {
				return;
			}
			auto pipeline = idComputePipeline_[pipelineId];

			CCB_->PushConstants(pipeline->GetLayout(), RAL::Driver::Shader::Stage::ComputeShader, sizeInBytes, data);
		}

		void Dispatch(Common::Size groupCountX, Common::Size groupCountY, Common::Size groupCountZ) override {

			if (CCB_ == nullptr) {
				return;
			}
			CCB_->Dispatch(groupCountX, groupCountY, groupCountZ);
		}

		void EndCompute() override {

			CCB_->EndDebug();
			CCB_->End();

			Fence::CreateInfo fenceCI {
				.LD_ = objects_.LD_
			};
			
			auto computeEndedFence = std::make_shared<Fence>(fenceCI);

			computeEndedFence->Reset();

			CCB_->Submit(objects_.LD_->GetComputeQueue(), computeEndedFence);


			computeEndedFence->Wait();


			//CCB_->WaitIdle();

		}

		//Compute pipeline

		// UNIFORM BUFFER
		[[nodiscard]]
		virtual UniformBuffer::Id CreateUniformBuffer(const UniformBuffer::CreateInfo& createInfo) override {

			switch (createInfo.type_) {
			case UniformBuffer::Type::Const: {
				Vulkan::UB::CreateInfo ubci{
					.PD_ = objects_.physicalDevice_,
					.LD_ = objects_.LD_,
					.sizeInBytes_ = createInfo.size_
				};
				auto uniformBuffer = std::make_shared<Vulkan::UniformBuffer>(ubci);
				uniformBuffer->Allocate();

				Common::Id id = UBsIdsGenerator_.Generate();
				UBs_[id] = std::vector<std::shared_ptr<Vulkan::UniformBuffer>>{ uniformBuffer };
				return id;
				break;
			}
			case UniformBuffer::Type::Mutable: {
				//We should have multiple buffers, because multiple frames may be in flight at the same time and
				//we don't want to update the buffer in preparation of the next frame while a previous one is still reading from it!
				//Thus, we need to have as many uniform buffers as we have frames in flight,
				//and write to a uniform buffer that is not currently being read by the GPU
				std::vector<std::shared_ptr<Vulkan::UniformBuffer>> UBs;
				Vulkan::UB::CreateInfo ubci{
					.PD_ = objects_.physicalDevice_,
					.LD_ = objects_.LD_,
					.sizeInBytes_ = createInfo.size_
				};
				for (Common::Index i = 0; i < concurrentFramesNumber; i++) {
					auto UB = std::make_shared<Vulkan::UniformBuffer>(ubci);
					UB->Allocate();
					UBs.push_back(UB);
				}
				Common::Id id = UBsIdsGenerator_.Generate();
				UBs_[id] = std::move(UBs);
				return id;
				break;
			}
			default: {
				ASSERT_FAIL_MSG("Invalid type of uniform buffer.");
				return Common::Id::Invalid();
			}
			};

		}

		virtual Common::Size GetUBSizeInBytes(UB::Id ubid) override {
			return GetUniformBuffer(ubid)->GetSizeInBytes();
		}

		std::shared_ptr<Vulkan::UniformBuffer> GetUniformBuffer(UniformBuffer::Id UBId) {
			auto UB = UBs_[UBId];
			if (UB.size() == 1) {
				return UBs_[UBId][0];
			}
			else {
				return UBs_[UBId][currentFrame];
			}
		}

		void RemoveUniformBuffer(UniformBuffer::Id UBId) {
			UBs_.erase(UBId);
		}


		[[nodiscard]]
		virtual void FillUniformBuffer(UniformBuffer::Id UBId, void* data) override {
			std::vector<std::shared_ptr<Vulkan::UniformBuffer>>& ub = UBs_[UBId];
			if (ub.size() == 1) {
				ub[0]->Write(0, data, ub[0]->GetSizeInBytes());
			}
			else {
				ASSERT_FMSG(ub.size() == concurrentFramesNumber, "Incorrect number of ubs.");
				ub[currentFrame]->Write(0, data, ub[currentFrame]->GetSizeInBytes());
			}
		}

		// UNIFORM BUFFER

		// STORAGE BUFFER
		[[nodiscard]]
		virtual StorageBuffer::Id CreateStorageBuffer(const StorageBuffer::CreateInfo& createInfo) override {

			Vulkan::SB::CreateInfo sbci{
				.PD_ = objects_.physicalDevice_,
				.LD_ = objects_.LD_,
				.sizeInBytes_ = createInfo.size_
			};

			auto storageBuffer1 = std::make_shared<Vulkan::StorageBuffer>(sbci);

			storageBuffer1->Allocate();

			auto storageBuffer2 = std::make_shared<Vulkan::StorageBuffer>(sbci);

			storageBuffer2->Allocate();

			Common::Id id = SBsIdsGenerator_.Generate();
			SBs_[id] = std::vector<std::shared_ptr<Vulkan::StorageBuffer>>{ storageBuffer1, storageBuffer2 };
			return id;

		}

		virtual Common::Size GetSBSizeInBytes(SB::Id sbid) override {
			return GetStorageBuffer(sbid)->GetSizeInBytes();
		}

		std::shared_ptr<Vulkan::StorageBuffer> GetStorageBuffer(StorageBuffer::Id SBId) {
			auto SB = SBs_[SBId];
			return SB[0/*(currentFrame == 0) ? (1) : (0)*/];
		}

		void RemoveStorageBuffer(StorageBuffer::Id SBId) {
			SBs_[SBId] = {};
		}


		[[nodiscard]]
		//TODO:  rename this and childs
		virtual void StorageBufferWrite(
			StorageBuffer::Id SBId,
			Common::Size offsetInBytes,
			void* data,
			Common::Size bytesNumber) override {
			ASSERT(data != nullptr);
			ASSERT(bytesNumber > 0);

			auto storageBuffer = GetStorageBuffer(SBId);
			//std::vector<std::shared_ptr<Vulkan::StorageBuffer>>& sb = SBs_[SBId];
			storageBuffer->Write(offsetInBytes, data, bytesNumber, objects_.commandPool_);

		}

		//TODO: rename this and childs
		virtual void StorageBufferRead(
			StorageBuffer::Id SBId,
			Common::Size offset,
			Common::Size size,
			void* data) override {
			auto storageBuffer = GetStorageBuffer(SBId);
			storageBuffer->Read(offset, data, size, objects_.commandPool_);
		}

		// STORAGE BUFFER

		//VERTEX BUFFER

		std::vector<std::shared_ptr<HostVisibleVertexBuffer>> CreateVBForEachFrameInFlight(const VertexBuffer::CreateInfo2& createInfo) {


			switch (createInfo.type_) {
			case VertexBuffer::Type::Const: {

				HostVisibleVertexBuffer::CreateInfo vertexBufferCreateInfo{};
				{
					vertexBufferCreateInfo.LD_ = objects_.LD_;
					vertexBufferCreateInfo.physicalDevice_ = objects_.physicalDevice_;
					vertexBufferCreateInfo.commandPool_ = objects_.commandPool_;
					vertexBufferCreateInfo.verticesNumber_ = createInfo.verticesNumber_;
					vertexBufferCreateInfo.vertexSize_ = createInfo.vertexSize_;//VertexTypeToSize(createInfo.vertexType_);
				}
				auto allocatedVertexBuffer = std::make_shared<HostVisibleVertexBuffer>(vertexBufferCreateInfo);

				allocatedVertexBuffer->Allocate();
				return std::vector<std::shared_ptr<HostVisibleVertexBuffer>>{ allocatedVertexBuffer };
				break;
			}
			case VertexBuffer::Type::Mutable: {
				//We should have multiple buffers, because multiple frames may be in flight at the same time and
				//we don't want to update the buffer in preparation of the next frame while a previous one is still reading from it!
				//Thus, we need to have as many uniform buffers as we have frames in flight,
				//and write to a uniform buffer that is not currently being read by the GPU
				std::vector<std::shared_ptr<HostVisibleVertexBuffer>> VBs;
				for (Common::Index i = 0; i < concurrentFramesNumber; i++) {
					HostVisibleVertexBuffer::CreateInfo vertexBufferCreateInfo{};
					{
						vertexBufferCreateInfo.LD_ = objects_.LD_;
						vertexBufferCreateInfo.physicalDevice_ = objects_.physicalDevice_;
						vertexBufferCreateInfo.commandPool_ = objects_.commandPool_;
						vertexBufferCreateInfo.verticesNumber_ = createInfo.verticesNumber_;
						vertexBufferCreateInfo.vertexSize_ = createInfo.vertexSize_; //VertexTypeToSize(createInfo.vertexType_);
					}
					auto allocatedVertexBuffer = std::make_shared<HostVisibleVertexBuffer>(vertexBufferCreateInfo);

					allocatedVertexBuffer->Allocate();

					auto VB = std::make_shared<HostVisibleVertexBuffer>(vertexBufferCreateInfo);
					VB->Allocate();
					VBs.push_back(VB);
				}
				return VBs;
				break;
			}
			default: {
				ASSERT_FAIL_MSG("Invalid type of uniform buffer.");
				return {};
			}
			};

		}

		virtual VertexBuffer::Id CreateVertexBuffer(const VertexBuffer::CreateInfo1& createInfo) override {

			RAL::Driver::VertexBuffer::CreateInfo2 vbci{
				.verticesNumber_ = createInfo.verticesNumber_,
				.vertexSize_ = VertexTypeToSize(createInfo.vertexType_),
				.type_ = createInfo.type_
			};

			auto vbs = CreateVBForEachFrameInFlight(vbci);

			const Common::Id id = UBsIdsGenerator_.Generate();
			VBs_[id] = std::move(vbs);
			return id;
		}

		virtual void FillVertexBuffer(VertexBuffer::Id id, Common::Size offset, const void* vertices, Common::Size verticesNumber) override {

			std::vector<std::shared_ptr<HostVisibleVertexBuffer>>& vb = VBs_[id];
			if (vb.size() == 1) {
				vb[0]->FillData(offset, vertices, verticesNumber, objects_.commandPool_);
			}
			else {
				ASSERT_FMSG(vb.size() == concurrentFramesNumber, "Incorrect number of vbs.");
				vb[currentFrame]->FillData(offset, vertices, verticesNumber, objects_.commandPool_);
			}
		}

		std::shared_ptr<HostVisibleVertexBuffer> GetVB(VertexBuffer::Id VBId) {
			auto VB = VBs_[VBId];
			if (VB.size() == 1) {
				return VBs_[VBId][0];
			}
			else {
				return VBs_[VBId][currentFrame];
			}
		}
		virtual void ResizeVertexBuffer(VertexBuffer::Id vbid, Common::Size verticesNumber) override {
			ASSERT_FMSG(
				VBs_.find(vbid) != VBs_.end(),
				"Attempt to destroy vertexbuffer that doesnt exist.");

			auto vbs = VBs_[vbid];

			RAL::Driver::VertexBuffer::CreateInfo2 vbci{
				.verticesNumber_ = verticesNumber,
				.vertexSize_ = vbs[0]->GetVertexSize(),
				.type_ = (vbs.size() == 1) ? (RAL::Driver::VertexBuffer::Type::Const) : (RAL::Driver::VertexBuffer::Type::Mutable)
			};
			auto newvbs = CreateVBForEachFrameInFlight(vbci);

			DestroyVertexBuffer(vbid);

			VBs_[vbid] = std::move(newvbs);
		}
		[[nodiscard]]
		virtual void DestroyVertexBuffer(VertexBuffer::Id VBId) override {
			ASSERT_FMSG(
				VBs_.find(VBId) != VBs_.end(),
				"Attempt to destroy vertexbuffer that doesnt exist.");

			auto vbs = VBs_[VBId];

			if (vbs.size() == 1) {
				VBsToDestroy_[currentFrame].push_back(vbs[0]);
			}
			else {

				for (
					Common::Index frameInFlightIndex = 0;
					frameInFlightIndex < vbs.size();
					frameInFlightIndex++) {
					VBsToDestroy_[frameInFlightIndex].push_back(vbs[frameInFlightIndex]);
				}
			}
			VBs_.erase(VBId);

		}

		virtual Common::Size GetVBSizeInBytes(VertexBuffer::Id VBId) override {
			auto vbs = VBs_[VBId];
			return vbs[0]->GetSizeInBytes();
		}

		//VERTEX BUFFER

		//INDEX BUFFER
		std::vector<std::shared_ptr<HostVisibleIndexBuffer>> CreateIBForEachFrameInFlight(const IndexBuffer::CreateInfo2& createInfo) {

			switch (createInfo.type_) {
			case IndexBuffer::Type::Const: {
				//no need copies of buffer if it is const.
				HostVisibleIndexBuffer::CreateInfo indexBufferCreateInfo{};
				{
					indexBufferCreateInfo.LD_ = objects_.LD_;
					indexBufferCreateInfo.physicalDevice_ = objects_.physicalDevice_;
					indexBufferCreateInfo.commandPool_ = objects_.commandPool_;
					indexBufferCreateInfo.indicesNumber_ = createInfo.indicesNumber_;
					indexBufferCreateInfo.indexSize_ = createInfo.indexSize_;
				}
				auto allocatedIndexBuffer = std::make_shared<HostVisibleIndexBuffer>(indexBufferCreateInfo);

				allocatedIndexBuffer->Allocate();

				const IndexBuffer::Id id = IBsIdsGenerator_.Generate();

				return std::vector<std::shared_ptr<HostVisibleIndexBuffer>>{ allocatedIndexBuffer };

				break;
			}
			case IndexBuffer::Type::Mutable: {
				//We should have multiple buffers, because multiple frames may be in flight at the same time and
				//we don't want to update the buffer in preparation of the next frame while a previous one is still reading from it!
				//Thus, we need to have as many uniform buffers as we have frames in flight,
				//and write to a uniform buffer that is not currently being read by the GPU
				std::vector<std::shared_ptr<HostVisibleIndexBuffer>> IBs;
				for (Common::Index i = 0; i < concurrentFramesNumber; i++) {

					HostVisibleIndexBuffer::CreateInfo indexBufferCreateInfo{};
					{
						indexBufferCreateInfo.LD_ = objects_.LD_;
						indexBufferCreateInfo.physicalDevice_ = objects_.physicalDevice_;
						indexBufferCreateInfo.commandPool_ = objects_.commandPool_;
						indexBufferCreateInfo.indicesNumber_ = createInfo.indicesNumber_;
						indexBufferCreateInfo.indexSize_ = createInfo.indexSize_;
					}
					auto IB = std::make_shared<HostVisibleIndexBuffer>(indexBufferCreateInfo);

					IB->Allocate();
					IBs.push_back(IB);
				}
				return IBs;
				break;
			}
			default: {
				ASSERT_FAIL_MSG("Invalid type of index buffer.");
				return {};
			}
			};
		}


		virtual IndexBuffer::Id CreateIndexBuffer(const IndexBuffer::CreateInfo1& createInfo) override {

			const IndexBuffer::CreateInfo2 ibci{
				.indicesNumber_ = createInfo.indicesNumber_,
				.indexSize_ = IndexTypeToSize(createInfo.indexType_),
				.type_ = createInfo.type_
			};

			auto ibs = CreateIBForEachFrameInFlight(ibci);

			const IndexBuffer::Id id = IBsIdsGenerator_.Generate();

			IBs_[id] = ibs;

			return id;
		}

		virtual void FillIndexBuffer(IndexBuffer::Id id, Common::Size offset, const void* indices, Common::Size indicesNumber) override {

			std::vector<std::shared_ptr<HostVisibleIndexBuffer>>& ib = IBs_[id];
			if (ib.size() == 1) {
				ib[0]->FillData(offset, indices, indicesNumber, objects_.commandPool_);
			}
			else {
				ASSERT_FMSG(ib.size() == concurrentFramesNumber, "Incorrect number of vbs.");
				ib[currentFrame]->FillData(offset, indices, indicesNumber, objects_.commandPool_);
			}
		}

		virtual void ResizeIndexBuffer(IndexBuffer::Id ibid, Common::Size indicesNumber) override {
			ASSERT_FMSG(
				IBs_.find(ibid) != IBs_.end(),
				"Attempt to destroy vertexbuffer that doesnt exist.");

			auto ibs = IBs_[ibid];

			DestroyIndexBuffer(ibid);
			RAL::Driver::IndexBuffer::CreateInfo2 ibci{
				.indicesNumber_ = indicesNumber,
				.indexSize_ = ibs[0]->GetIndexSize(),
				.type_ = (ibs.size() == 1) ? (RAL::Driver::IndexBuffer::Type::Const) : (RAL::Driver::IndexBuffer::Type::Mutable)
			};
			auto newibs = CreateIBForEachFrameInFlight(ibci);
			IBs_[ibid] = std::move(newibs);
		}

		virtual void DestroyIndexBuffer(IndexBuffer::Id IBId) override {
			ASSERT_FMSG(
				IBs_.find(IBId) != IBs_.end(),
				"Attempt to destroy vertexbuffer that doesnt exist.");

			auto ibs = IBs_[IBId];

			if (ibs.size() == 1) {
				IBsToDestroy_[currentFrame].push_back(ibs[0]);
			}
			else {
				for (
					Common::Index frameInFlightIndex = 0;
					frameInFlightIndex < concurrentFramesNumber;
					frameInFlightIndex++) {
					IBsToDestroy_[frameInFlightIndex].push_back(ibs[frameInFlightIndex]);
				}
			}
			IBs_.erase(IBId);
		}

		virtual Common::Size GetIBSizeInBytes(IndexBuffer::Id IBId) override {
			auto ibs = IBs_[IBId];
			return ibs[0]->GetSizeInBytes();
		}


		std::shared_ptr<HostVisibleIndexBuffer> GetIB(IndexBuffer::Id IBId) {
			auto IB = IBs_[IBId];
			if (IB.size() == 1) {
				return IBs_[IBId][0];
			}
			else {
				return IBs_[IBId][currentFrame];
			}
		}

		//INDEX BUFFER

		[[nodiscard]]
		virtual RAL::Driver::Texture::Id CreateTexture(const RAL::Driver::Texture::CreateInfo1& createInfo) override {

			VkImageUsageFlags usages = 0;
			for (RAL::Driver::Texture::Usage usage : createInfo.usages_) {
				usages = usages | ToVulkanType(usage);
			}

			VkPipelineStageFlags stages = 0;
			for (RAL::Driver::Pipeline::Stage stage : createInfo.targetPipelineStages_) {
				stages = stages | ToVulkanType(stage);
			}

			Vulkan::Texture::CreateInfo1 textureCreateInfo{
				.name_ = createInfo.name_,
				.format_ = ToVulkanType(createInfo.format_),
				.data_ = createInfo.data_,
				.size_ = createInfo.size_,
				.targetLayout_ = ToVulkanType(createInfo.targetState_),
				.targetAccess_ = ToVulkanType(createInfo.targetAccess_),
				.targetPipelineStage_ = stages,
				.usages_ = usages,
				.mipLevels_ = createInfo.mipLevels_,
				.samplesCount_ = ToVulkanType(createInfo.samplesCount_),
				.PD_ = objects_.physicalDevice_,
				.LD_ = objects_.LD_,
				.commandPool_ = objects_.commandPool_
			};
			auto texture = std::make_shared<Vulkan::Texture>(std::move(textureCreateInfo));

			const RAL::Driver::Texture::Id textureId = textures_.size();
			textures_[textureId] = texture;
			return textureId;
		}


		[[nodiscard]]
		virtual bool IsTextureExist(RAL::Driver::Texture::Id textureId) const noexcept {
			return textures_.contains(textureId);
		}

	public:



		[[nodiscard]]
		Common::Size VertexTypeToSize(VertexType vertexType) {
			switch (vertexType) {
			case  VertexType::VF3: {
				return sizeof(Vertex3f);
				break;
			}
			case  VertexType::VF3_NF3_TF2: {
				return sizeof(Vertex3fnt);
				break;
			}
			case  VertexType::VF3_NF3_TF2_BIDUB4_WUB4: {
				return sizeof(Vertex3fntbw);
				break;
			}
			case  VertexType::VF2_TF2_CF3: {
				return sizeof(Vertex2ftc);
				break;
			}
			case VertexType::VF3_CF3: {
				return sizeof(Vertex3fc);
				break;
			}
			case VertexType::VF3_NF3_CF4: {
				return sizeof(Vertex3fnc);
				break;
			}
			default: {
				ASSERT_FAIL_MSG("Unsupported vertex type.");
				return 0;
			}
			};
		}

		[[nodiscard]]
		Common::Size IndexTypeToSize(IndexType indexType) {
			switch (indexType) {
			case  IndexType::UI16: {
				return sizeof(Common::UInt16);
				break;
			}
			case  IndexType::UI32: {
				return sizeof(Common::UInt32);
				break;
			}
			default: {
				ASSERT_FAIL_MSG("Unsupported index type.");
				return 0;
			}
			};
		}

		//Drawing.
	private:

		struct DrawingContext {
			std::shared_ptr<CommandBuffer> commandBuffer_ = nullptr;
			std::shared_ptr<Pipeline> pipeline_ = nullptr;

			struct CreateInfo {
				std::shared_ptr<LogicDevice> LD_ = nullptr;
				std::shared_ptr<CommandPool> commandPool_ = nullptr;
			};
			using CI = CreateInfo;

			DrawingContext(const CI& ci) {

				const CommandBuffer::CreateInfo cbci{
					.LD_ = ci.LD_,
					.commandPool_ = ci.commandPool_,
					.level_ = VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY
				};

				commandBuffer_ = std::make_shared<CommandBuffer>(cbci);

			}
		};

		std::vector<DrawingContext> contexts_;

	public:

		struct ResourceSet {
			using Id = Common::Id;
			struct CreateInfo {
				std::array<std::shared_ptr<DescriptorSet>, concurrentFramesNumber> dss_;
			};

			using CI = CreateInfo;

			ResourceSet(const CreateInfo& ci) :
				dss_{ ci.dss_ } {

			}

			std::array<std::shared_ptr<DescriptorSet>, concurrentFramesNumber> dss_;
		};
		using RS = ResourceSet;


		std::map<Common::Id, std::shared_ptr<ResourceSet>> resources_;
		Common::IdGenerator resourcesIdsGenerator_;

		virtual RAL::Driver::RS::Id CreateResourceSet(const RAL::Driver::RS::CI1& ci) override {

			std::vector<VkDescriptorSetLayoutBinding> bindings;


			for (Common::Index i = 0; i < ci.bindingsNumber_; i++) {
				const RAL::Driver::ResourceSet::Binding& binding = ci.bindings_[i];


				VkDescriptorType type{};
				if (!binding.ubid_.IsInvalid()) {
					type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				}
				else if (!binding.textureId_.IsInvalid()) {
					type = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				}
				else if (!binding.sbid_.IsInvalid()) {
					type = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				}

				VkDescriptorSetLayoutBinding inputAttachmentBinding{
					static_cast<decltype(VkDescriptorSetLayoutBinding::binding)>(binding.binding_),
					type,
					1,
					static_cast<VkFlags>(ToVulkanType(binding.stage_)),
					nullptr
				};
				bindings.push_back(inputAttachmentBinding);
			}


			auto dsl = std::make_shared<DescriptorSetLayout>(
				DescriptorSetLayout::CreateInfo{
					"No name",
					objects_.LD_,
					std::vector{ bindings }
				});

			DescriptorSet::CreateInfo DSCreateInfo;
			{
				DSCreateInfo.DP_ = objects_.DP_;
				DSCreateInfo.DSL_ = dsl;
				DSCreateInfo.LD_ = objects_.LD_;
			}

			std::array<std::shared_ptr<DescriptorSet>, concurrentFramesNumber > dss;

			for (Common::Index i = 0; i < concurrentFramesNumber; i++) {

				auto ds = std::make_shared<DescriptorSet>(DSCreateInfo);
				dss[i] = ds;

			}

			for (Common::Index concurrentFrameIndex = 0; concurrentFrameIndex < concurrentFramesNumber; concurrentFrameIndex++) {
				std::vector<DS::UpdateDescriptorInfo> updateDSsInfo;
				for (Common::Index i = 0; i < ci.bindingsNumber_; i++) {
					const RAL::Driver::ResourceSet::Binding& binding = ci.bindings_[i];
					DescriptorSet::UpdateDescriptorInfo updateInfo;
					{
						updateInfo.binding_ = binding.binding_;

						if (!binding.ubid_.IsInvalid()) {
							updateInfo.type_ = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
							auto& ubsPerFrame = UBs_[binding.ubid_];
							std::shared_ptr<Vulkan::UniformBuffer> ub = ubsPerFrame[concurrentFrameIndex];
							updateInfo.bufferInfo_.buffer_ = ub;
							updateInfo.bufferInfo_.offset_ = binding.offset_;
							updateInfo.bufferInfo_.range_ = binding.size_;
						}
						else if (!binding.textureId_.IsInvalid()) {
							updateInfo.type_ = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							auto texture = textures_[binding.textureId_];
							updateInfo.imageInfo_.imageLayout_ = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							updateInfo.imageInfo_.sampler_ = texture->GetSampler();
							updateInfo.imageInfo_.imageView_ = texture->GetImageView();
						}
						else if (!binding.sbid_.IsInvalid()) {
							updateInfo.type_ = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
							std::shared_ptr<Vulkan::StorageBuffer> sb = GetStorageBuffer(binding.sbid_);//SBs_[binding.sbid_][0];
							updateInfo.bufferInfo_.buffer_ = sb;
							updateInfo.bufferInfo_.offset_ = binding.offset_;
							updateInfo.bufferInfo_.range_ = binding.size_;
						}
					}

					updateDSsInfo.push_back(updateInfo);

				}

				dss[concurrentFrameIndex]->Update(updateDSsInfo);
			}
			const RS::Id rsId = resourcesIdsGenerator_.Generate();
			const RS::CI rsci{
				.dss_ = dss
			};



			resources_[rsId] = std::make_shared<RS>(rsci);

			return rsId;
		}

		virtual RAL::Driver::Resource::Id CreateResource(const RAL::Driver::Resource::CI2& ci) override {
			const RAL::Driver::RS::CI1 rsci{
				.bindings_ = { ci },
				.bindingsNumber_ = 1
			};
			return CreateResourceSet(rsci);
		}
		//
		//		[[deprecated]]
		//		[[nodiscard]]
		//		virtual Common::Id DrawMesh(
		//			const std::string& pipelineName,
		//			VertexBuffer::Id VBId,
		//			IndexBuffer::Id IBId,
		//			const std::vector<RAL::Driver::ShaderBinding::Data>& bindingsData) override {
		//
		//#pragma region Assert
		//			const RAL::Driver::PipelineDescription& pipelineDesc = createInfo_.namePipelineDescriptions_[pipelineName];
		//			ASSERT_FMSG(pipelineDesc.shaderBindings_.size() == bindingsData.size(), "Shaders bindings layout and data are different.");
		//			for (Common::Index i = 0; i < pipelineDesc.shaderBindings_.size(); i++) {
		//				const RAL::Driver::ShaderBinding::Layout& layout = pipelineDesc.shaderBindings_[i];
		//				const RAL::Driver::ShaderBinding::Data& data = bindingsData[i];
		//				ASSERT_FMSG(layout.stage_ == data.stage_, "Shaders bindings layout and data are different.");
		//				ASSERT_FMSG(layout.type_ == data.type_, "Shaders bindings layout and data are different.");
		//			}
		//#pragma endregion
		//
		//			std::shared_ptr<Vulkan::Pipeline> pipeline = namePipeline_[pipelineName];
		//
		//			std::vector<Mesh::ShaderBinding> shaderBindings;
		//
		//			for (Common::Index i = 0; i < bindingsData.size(); i++) {
		//
		//				const RAL::Driver::ShaderBinding::Data& bindingData = bindingsData[i];
		//
		//				if (bindingData.type_ == RAL::Driver::ShaderBinding::Type::Uniform) {
		//
		//					DS::CreateInfo DSCreateInfo;
		//					{
		//						DSCreateInfo.DP_ = objects_.DP_;
		//						DSCreateInfo.DSL_ = pipeline->GetLayout()->GetDSLs()[i];
		//						DSCreateInfo.LD_ = objects_.LD_;
		//					}
		//
		//					auto ds = std::make_shared<DS>(DSCreateInfo);
		//
		//					auto ub = GetUniformBuffer(bindingData.uniformBufferId_);
		//
		//					ds->UpdateBufferWriteConfiguration(
		//						ub,
		//						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		//						0, 0, ub->GetSizeInBytes());
		//
		//					Mesh::ShaderBinding shaderBinding{
		//						.ubId_ = bindingData.uniformBufferId_,
		//						.ds_ = ds
		//					};
		//					shaderBindings.push_back(shaderBinding);
		//				}
		//				if (bindingData.type_ == RAL::Driver::ShaderBinding::Type::Sampler) {
		//					Mesh::ShaderBinding shaderBinding{
		//						.textureId_ = bindingData.textureId_
		//					};
		//					shaderBindings.push_back(shaderBinding);
		//				}
		//			}
		//
		//			Mesh::CreateInfo meshCreateInfo{
		//				.VBId_ = VBId,
		//				.IBId_ = IBId,
		//				.shaderBindings_ = shaderBindings,
		//				.pipelineName_ = pipelineName
		//			};
		//			auto mesh = std::make_shared<Mesh>(meshCreateInfo);
		//			const Mesh::Id meshId = shapes2IdsGenerator_.Generate();
		//			meshs_[meshId] = mesh;
		//			return meshId;
		//		}
		//		[[deprecated]]
		//		virtual void ResumeMeshDrawing(Common::Id shapeId) override {
		//			meshs_[shapeId]->ResumeDrawing();
		//		}
		//		[[deprecated]]
		//		virtual void StopMeshDrawing(Common::Id shapeId) override {
		//			meshs_[shapeId]->StopDrawing();
		//		}
		//		[[deprecated]]
		//		virtual void RemoveMeshFromDrawing(Common::Id shapeId) override {
		//			auto mesh = meshs_[shapeId];
		//
		//			meshs_.erase(shapeId);
		//		}

	private:

		//std::map<Mesh::Id, std::shared_ptr<Mesh>> meshs_;
		//Common::IdGenerator shapes2IdsGenerator_;
		//std::map<std::string, std::shared_ptr<Vulkan::Pipeline>> namePipeline_;

		Common::IdGenerator renderPassResourceIdsGenerator_;
		std::map<RP::Id, std::shared_ptr<Vulkan::RenderPass2::AttachmentData>> idRenderPassResource_;

		Common::IdGenerator renderPassIdsGenerator_;
		std::map<RP::Id, std::shared_ptr<Vulkan::RenderPass2>> idRenderPass_;

		Common::IdGenerator framebufferIdsGenerator_;
		std::map<RP::Id, std::shared_ptr<Vulkan::FrameBuffer>> idFrameBuffer_;

		Common::IdGenerator pipelineIdsGenerator_;
		std::map<Pipeline::Id, std::shared_ptr<Vulkan::Pipeline>> idPipeline_;

		Common::IdGenerator computePipelineIdsGenerator_;
		std::map<ComputePipeline::Id, std::shared_ptr<Vulkan::ComputePipeline>> idComputePipeline_;

	private:

		[[nodiscard]]
		std::shared_ptr<Vulkan::Texture> GetTextureById(RAL::Driver::Texture::Id id) noexcept {
			return textures_[id];
		}

		[[nodiscard]]
		std::shared_ptr<RenderPass2> GetRenderPassById(RAL::Driver::RP::Id id) noexcept {
			return idRenderPass_[id];
		}

		[[nodiscard]]
		std::shared_ptr<FrameBuffer> GetFrameBufferById(RAL::Driver::RP::AttachmentSet::Id id) noexcept {
			return idFrameBuffer_[id];
		}

	private:

		using FrameInFlightIndex = Common::Index;

		Objects objects_;

		std::map<Common::Id, std::shared_ptr<Vulkan::Texture>> textures_;
		Common::IdGenerator texturesIdsGenerator_;

		std::map<Common::Id, std::vector<std::shared_ptr<Vulkan::UniformBuffer>>> UBs_;
		Common::IdGenerator UBsIdsGenerator_;

		std::map<Common::Id, std::vector<std::shared_ptr<Vulkan::UBDS>>> UBDSs_;
		Common::IdGenerator UBDSsIdsGenerator_;

		//std::map<Common::Id, std::vector<std::shared_ptr<Vulkan::StorageBuffer>>> SBs_;

		Common::IdData< std::vector<std::shared_ptr<Vulkan::StorageBuffer>>> SBs_;
		Common::IdGenerator SBsIdsGenerator_;

		std::map<Common::Id, std::vector<std::shared_ptr<Vulkan::SBDS>>> SBDSs_;
		Common::IdGenerator SBDSsIdsGenerator_;

		std::map<Common::Id, std::vector<std::shared_ptr<HostVisibleVertexBuffer>>> VBs_;
		Common::IdGenerator VBsIdsGenerator_;
		std::map<FrameInFlightIndex, std::vector<std::shared_ptr<HostVisibleVertexBuffer>>> VBsToDestroy_;

		std::map<Common::Id, std::vector<std::shared_ptr<HostVisibleIndexBuffer>>> IBs_;
		Common::IdGenerator IBsIdsGenerator_;
		std::map<FrameInFlightIndex, std::vector<std::shared_ptr<HostVisibleIndexBuffer>>> IBsToDestroy_;



	public:




		////pack of render passes, framebuffer and attachments
		//class RenderPacket {
		//public:


		//	struct GBufferInfo {
		//		std::shared_ptr<Image> image_ = nullptr;
		//		std::shared_ptr<ImageView> imageView_ = nullptr;
		//		std::shared_ptr<Sampler> sampler_ = nullptr;
		//		std::shared_ptr<DSL> dsl_ = nullptr;
		//		std::shared_ptr<DS> ds_ = nullptr;
		//	};

		//	struct RenderPassContext {

		//		struct Attachment {
		//			enum class Type {
		//				Depth,
		//				Undefined
		//			};
		//		};
		//		std::shared_ptr<MultisamplingData> multisamplingData_ = nullptr;
		//		std::shared_ptr<DepthTestData> depthStencilData_ = nullptr;

		//		std::vector<std::shared_ptr<FrameBuffer>> frameBuffers_;
		//		std::shared_ptr<RenderPass2> renderPass_ = nullptr;
		//	};

		//	struct CreateInfo {
		//		std::shared_ptr<PhysicalDevice> PD_ = nullptr;
		//		std::shared_ptr<LogicDevice> LD_ = nullptr;
		//		std::shared_ptr<DescriptorPool> DP_ = nullptr;
		//		std::shared_ptr<SwapChain> swapChain_ = nullptr;
		//		Common::Size framesInFlight_ = 0;
		//	};
		//	std::shared_ptr<GBufferInfo> gBufferInfo_ = nullptr;

		//	RenderPacket(const CreateInfo& ci) {

		//		//Rendered buffer
		//		{

		//			{
		//				gBufferInfo_ = std::make_shared<GBufferInfo>();

		//				Image::CreateInfo gBufferCreateInfo;
		//				{
		//					gBufferCreateInfo.physicalDevice_ = ci.PD_;
		//					gBufferCreateInfo.LD_ = ci.LD_;
		//					gBufferCreateInfo.name_ = "G-Buffer";
		//					gBufferCreateInfo.format_ = ci.swapChain_->GetFormat().format;
		//					gBufferCreateInfo.size_ = ci.swapChain_->GetSize();
		//					gBufferCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
		//					gBufferCreateInfo.usage_ = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		//					gBufferCreateInfo.samplesCount_ = VK_SAMPLE_COUNT_1_BIT;
		//				}

		//				gBufferInfo_->image_ = std::make_shared<AllocatedImage>(gBufferCreateInfo);
		//				gBufferInfo_->imageView_ = CreateImageViewByImage(ci.LD_, gBufferInfo_->image_, VK_IMAGE_ASPECT_COLOR_BIT, 1);

		//				Sampler::CreateInfo samplerCreateInfo;
		//				{
		//					samplerCreateInfo.LD_ = ci.LD_;
		//					samplerCreateInfo.magFilter_ = VK_FILTER_LINEAR;
		//					samplerCreateInfo.minFilter_ = VK_FILTER_LINEAR;
		//					samplerCreateInfo.maxAnisotropy_ = 1.0;//ci.PD_->GetProperties().limits.maxSamplerAnisotropy;
		//					samplerCreateInfo.mipLevels_ = 1;
		//				}

		//				auto sampler = std::make_shared<Sampler>(samplerCreateInfo);

		//				gBufferInfo_->sampler_ = sampler;

		//				VkDescriptorSetLayoutBinding inputAttachmentBinding{
		//					0,
		//					ToVulkanType(RAL::Driver::Shader::Binding::Type::Sampler),
		//					1,
		//					static_cast<VkFlags>(ToVulkanType(RAL::Driver::Shader::Stage::FragmentShader)),
		//					nullptr
		//				};

		//				auto DSL = std::make_shared<DescriptorSetLayout>(
		//					DescriptorSetLayout::CreateInfo{
		//						"Rendered buffer",
		//						ci.LD_,
		//						std::vector{ inputAttachmentBinding }
		//					});
		//				gBufferInfo_->dsl_ = DSL;

		//				DS::CreateInfo DSCreateInfo;
		//				{
		//					DSCreateInfo.DP_ = ci.DP_;
		//					DSCreateInfo.DSL_ = DSL;
		//					DSCreateInfo.LD_ = ci.LD_;
		//				}

		//				auto ds = std::make_shared<DS>(DSCreateInfo);

		//				ds->UpdateImageWriteConfiguration(
		//					gBufferInfo_->imageView_,
		//					VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		//					sampler,
		//					VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		//					0);

		//				gBufferInfo_->ds_ = ds;

		//			}
		//		}


		//		//First render pass
		//		{

		//			RenderPassContext rpContext{};


		//			//DEPTH BUFFER
		//			{
		//				rpContext.depthStencilData_ = std::make_shared<DepthTestData>();
		//				{
		//					Image::CreateInfo depthImageCreateInfo;
		//					{
		//						depthImageCreateInfo.physicalDevice_ = ci.PD_;
		//						depthImageCreateInfo.LD_ = ci.LD_;
		//						depthImageCreateInfo.name_ = "Depth image",
		//							depthImageCreateInfo.format_ = VK_FORMAT_D32_SFLOAT;
		//						depthImageCreateInfo.size_ = ci.swapChain_->GetSize();
		//						depthImageCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
		//						depthImageCreateInfo.usage_ = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		//						depthImageCreateInfo.samplesCount_ = VK_SAMPLE_COUNT_1_BIT;/*objects_.physicalDevice_->GetMaxUsableSampleCount();*/
		//					}
		//					rpContext.depthStencilData_->image_ = std::make_shared<AllocatedImage>(depthImageCreateInfo);
		//					rpContext.depthStencilData_->imageView_ = CreateImageViewByImage(ci.LD_, rpContext.depthStencilData_->image_, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
		//				}
		//			}


		//			//Depth attachment
		//			Vulkan::RP::Attachment depthAttachment{
		//				.format = rpContext.depthStencilData_->image_->GetFormat(),
		//				.samples = VK_SAMPLE_COUNT_1_BIT,
		//				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		//				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		//				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		//				.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		//			};
		//			VkAttachmentReference depthAttachmentRef{
		//				.attachment = 0,
		//				.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		//			};

		//			//Rendered buffer
		//			Vulkan::RP::Attachment renderedAttachment{
		//				.format = ci.swapChain_->GetFormat().format,
		//				.samples = VK_SAMPLE_COUNT_1_BIT,
		//				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		//				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		//				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		//				.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			};

		//			VkAttachmentReference renderedAttachmentRef{
		//				.attachment = 1,
		//				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			};

		//			Vulkan::RP::Subpass::Dependency inExternalToFirstSubpass{
		//				.srcSubpass_ = VK_SUBPASS_EXTERNAL,
		//				.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		//				.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		//				.dstSubpass_ = 0,
		//				.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		//				.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT,
		//			};

		//			Vulkan::RP::Subpass::Dependency firstSubpassToOutExternal{
		//				.srcSubpass_ = 0,
		//				.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		//				.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		//				.dstSubpass_ = VK_SUBPASS_EXTERNAL,
		//				.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		//				.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT,
		//			};


		//			Vulkan::RP::Subpass renderSubpassDesc{
		//				.inputAttachments_ = {  },
		//				.colorAttachments_ = { renderedAttachmentRef },
		//				.resolveAttachment_ = {  },
		//				.depthStencilAttachment_ = std::make_shared<VkAttachmentReference>(depthAttachmentRef)
		//			};

		//			RenderPass2::CreateInfo rp2ci{
		//				.name_ = "Main pass",
		//				.LD_ = ci.LD_,
		//				.attachments_ = {
		//					depthAttachment,
		//					renderedAttachment
		//				},
		//				.subpasses_ = {
		//					renderSubpassDesc,
		//				},
		//				.dependencies_ = {
		//					inExternalToFirstSubpass,
		//					firstSubpassToOutExternal
		//				},

		//			};

		//			auto renderPass = std::make_shared<RenderPass2>(rp2ci);

		//			std::vector<std::shared_ptr<FrameBuffer>> frameBuffers;
		//			{
		//				VkExtent2D extent = ci.swapChain_->GetExtent();
		//				for (const auto& imageView : ci.swapChain_->GetImageViews()) {
		//					Common::DiscardUnusedParameter(imageView);
		//					FrameBuffer::CreateInfo createInfo;
		//					{
		//						createInfo.LD_ = ci.LD_;
		//						createInfo.attachments_.push_back(rpContext.depthStencilData_->imageView_);
		//						createInfo.attachments_.push_back(gBufferInfo_->imageView_);
		//						createInfo.extent_ = extent;
		//						createInfo.renderPass_ = renderPass;
		//					}
		//					auto frameBuffer = std::make_shared<FrameBuffer>(createInfo);
		//					frameBuffers.push_back(frameBuffer);
		//				}
		//				OS::LogInfo("/render/vulkan/driver/", "Frame buffers created successfuly.");
		//			}

		//			rpContext.frameBuffers_ = std::move(frameBuffers);
		//			rpContext.renderPass_ = renderPass;

		//			renderPasses_.push_back(std::move(rpContext));
		//		}


		//		//second render pass
		//		{
		//			RenderPassContext rpContext{ };


		//			//Multisampling
		//			{
		//				rpContext.multisamplingData_ = std::make_shared<MultisamplingData>();
		//				{
		//					Image::CreateInfo multisamplingCreateInfo;
		//					{
		//						multisamplingCreateInfo.physicalDevice_ = ci.PD_;
		//						multisamplingCreateInfo.LD_ = ci.LD_;
		//						multisamplingCreateInfo.name_ = "Multisampling image";
		//						multisamplingCreateInfo.format_ = ci.swapChain_->GetFormat().format;
		//						multisamplingCreateInfo.size_ = ci.swapChain_->GetSize();
		//						multisamplingCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
		//						multisamplingCreateInfo.usage_ = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		//						multisamplingCreateInfo.samplesCount_ = ci.PD_->GetMaxUsableSampleCount();
		//					}
		//					rpContext.multisamplingData_->image_ = std::make_shared<AllocatedImage>(multisamplingCreateInfo);
		//					rpContext.multisamplingData_->imageView_ = CreateImageViewByImage(ci.LD_, rpContext.multisamplingData_->image_, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		//				}
		//			}

		//			Vulkan::RP::Attachment multisampleAttachment{
		//				.format = ci.swapChain_->GetFormat().format,
		//				.samples = ci.PD_->GetMaxUsableSampleCount(),
		//				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		//				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		//				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		//				.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			};

		//			VkAttachmentReference multisampleAttachmentRef{
		//				.attachment = 0,
		//				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			};

		//			//swap chain
		//			Vulkan::RP::Attachment swapChainAttachment{
		//				.format = ci.swapChain_->GetFormat().format,
		//				.samples = VK_SAMPLE_COUNT_1_BIT,
		//				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		//				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		//				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		//				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		//			};
		//			VkAttachmentReference swapChainAttachmentRef{
		//				.attachment = 1,
		//				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			};

		//			//Rendered buffer
		//			Vulkan::RP::Attachment renderedAttachment{
		//				.format = ci.swapChain_->GetFormat().format,
		//				.samples = VK_SAMPLE_COUNT_1_BIT,
		//				.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		//				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		//				.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		//				.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			};

		//			VkAttachmentReference renderedAttachmentSubpass2Ref{
		//				.attachment = 2,
		//				.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		//			};

		//			Vulkan::RP::Subpass::Dependency inExternalToFirstSubpassDependency{
		//				.srcSubpass_ = VK_SUBPASS_EXTERNAL,
		//				.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		//				.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		//				.dstSubpass_ = 0,
		//				.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		//				.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT,
		//			};

		//			Vulkan::RP::Subpass::Dependency firstSubpassToOutExternalDependency_{
		//				.srcSubpass_ = 0,
		//				.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		//				.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		//				.dstSubpass_ = VK_SUBPASS_EXTERNAL,
		//				.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		//				.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT
		//			};

		//			Vulkan::RP::Subpass postProcessSubpassDesc{
		//				.inputAttachments_ = { renderedAttachmentSubpass2Ref },
		//				.colorAttachments_ = { multisampleAttachmentRef },
		//				.resolveAttachment_ = std::make_shared<VkAttachmentReference>(swapChainAttachmentRef),//std::make_shared<VkAttachmentReference>(swapChainAttachmentRef),
		//				.depthStencilAttachment_ = nullptr
		//			};

		//			Vulkan::RenderPass2::CreateInfo rp2ci{
		//				.name_ = "Post process pass",
		//				.LD_ = ci.LD_,
		//				.attachments_ = {
		//					multisampleAttachment,
		//					swapChainAttachment,
		//					renderedAttachment
		//				},
		//				.subpasses_ = {
		//					postProcessSubpassDesc
		//				},
		//				.dependencies_ = {
		//					inExternalToFirstSubpassDependency,
		//					firstSubpassToOutExternalDependency_
		//				},

		//			};

		//			auto renderPass = std::make_shared<RenderPass2>(rp2ci);

		//			std::vector<std::shared_ptr<FrameBuffer>> frameBuffers;
		//			{
		//				VkExtent2D extent = ci.swapChain_->GetExtent();
		//				for (const auto& imageView : ci.swapChain_->GetImageViews()) {
		//					FrameBuffer::CreateInfo createInfo;
		//					{
		//						createInfo.LD_ = ci.LD_;
		//						createInfo.attachments_.push_back(rpContext.multisamplingData_->imageView_);
		//						//createInfo.attachments_.push_back(depthStencilData_->imageView_);
		//						createInfo.attachments_.push_back(imageView);
		//						createInfo.attachments_.push_back(gBufferInfo_->imageView_);


		//						createInfo.extent_ = extent;
		//						createInfo.renderPass_ = renderPass;
		//					}
		//					auto frameBuffer = std::make_shared<FrameBuffer>(createInfo);
		//					frameBuffers.push_back(frameBuffer);
		//				}
		//				OS::LogInfo("/render/vulkan/driver/", "Frame buffers created successfuly.");
		//			}

		//			rpContext.frameBuffers_ = std::move(frameBuffers);
		//			rpContext.renderPass_ = renderPass;

		//			renderPasses_.push_back(std::move(rpContext));
		//		}


		//		//ui render pass
		//		{
		//			RenderPassContext rpContext{ };

		//			//swap chain
		//			Vulkan::RP::Attachment swapChainAttachment{
		//				.format = ci.swapChain_->GetFormat().format,
		//				.samples = VK_SAMPLE_COUNT_1_BIT,
		//				.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		//				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		//				.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		//				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		//			};
		//			VkAttachmentReference swapChainAttachmentRef{
		//				.attachment = 0,
		//				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			};

		//			////Rendered buffer
		//			//RP::Attachment renderedAttachment{
		//			//	.format = ci.swapChain_->GetFormat().format,
		//			//	.samples = VK_SAMPLE_COUNT_1_BIT,
		//			//	.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
		//			//	.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		//			//	.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		//			//	.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		//			//};

		//			//VkAttachmentReference renderedAttachmentSubpass2Ref{
		//			//	.attachment = 1,
		//			//	.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		//			//};

		//			Vulkan::RP::Subpass::Dependency inExternalToFirstSubpassDependency{
		//				.srcSubpass_ = VK_SUBPASS_EXTERNAL,
		//				.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		//				.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		//				.dstSubpass_ = 0,
		//				.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		//				.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT,
		//			};

		//			Vulkan::RP::Subpass::Dependency firstSubpassToOutExternalDependency_{
		//				.srcSubpass_ = 0,
		//				.srcStageMask_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		//				.srcAccessMask_ = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		//				.dstSubpass_ = VK_SUBPASS_EXTERNAL,
		//				.dstStageMask_ = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		//				.dstAccessMask_ = VK_ACCESS_SHADER_READ_BIT
		//			};

		//			Vulkan::RP::Subpass postProcessSubpassDesc{
		//				.inputAttachments_ = { /*renderedAttachmentSubpass2Ref*/ },
		//				.colorAttachments_ = { swapChainAttachmentRef },
		//				.resolveAttachment_ = nullptr,
		//				.depthStencilAttachment_ = nullptr
		//			};

		//			Vulkan::RenderPass2::CreateInfo rp2ci{
		//				.name_ = "UI pass",
		//				.LD_ = ci.LD_,
		//				.attachments_ = {
		//					swapChainAttachment,
		//					/*renderedAttachment*/
		//				},
		//				.subpasses_ = {
		//					postProcessSubpassDesc
		//				},
		//				.dependencies_ = {
		//					inExternalToFirstSubpassDependency,
		//					firstSubpassToOutExternalDependency_
		//				},

		//			};

		//			auto renderPass = std::make_shared<RenderPass2>(rp2ci);

		//			std::vector<std::shared_ptr<FrameBuffer>> frameBuffers;
		//			{
		//				VkExtent2D extent = ci.swapChain_->GetExtent();
		//				for (const auto& swapChainImageView : ci.swapChain_->GetImageViews()) {
		//					FrameBuffer::CreateInfo createInfo;
		//					{
		//						createInfo.LD_ = ci.LD_;
		//						createInfo.attachments_.push_back(swapChainImageView);
		//						//createInfo.attachments_.push_back(gBufferInfo_->imageView_);


		//						createInfo.extent_ = extent;
		//						createInfo.renderPass_ = renderPass;
		//					}
		//					auto frameBuffer = std::make_shared<FrameBuffer>(createInfo);
		//					frameBuffers.push_back(frameBuffer);
		//				}
		//				OS::LogInfo("/render/vulkan/driver/", "Frame buffers created successfuly.");
		//			}

		//			rpContext.frameBuffers_ = std::move(frameBuffers);
		//			rpContext.renderPass_ = renderPass;

		//			renderPasses_.push_back(std::move(rpContext));
		//		}


		//	}

		//	void ForEachRenderPass(std::function<void(RenderPassContext&)>&& processRenderPass) {
		//		for (RenderPassContext& renderPassContext : renderPasses_) {
		//			processRenderPass(renderPassContext);
		//		}
		//	}

		//	std::vector<RenderPassContext> renderPasses_;
		//};


		//std::unique_ptr<RenderPacket> render_ = nullptr;
	};

}