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

		struct MultisamplingData {
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

		static VkCullModeFlags ToVulkanType(CullMode cullMode) {
			switch (cullMode) {
			case CullMode::None: {
				return VkCullModeFlagBits::VK_CULL_MODE_NONE;
				break;
			}
			case CullMode::Back: {
				return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
				break;
			}
			case CullMode::Front: {
				return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;
				break;
			}
			case CullMode::FrontAndBack: {
				return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;
				break;
			}
			default:
				OS::AssertFailMessage("Invalid CullMode value used.");
			};
		}

		static VkPrimitiveTopology ToVulkanType(TopologyType topologyType) {
			switch (topologyType) {
			case TopologyType::LineList: {
				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
				break;
			}
			case TopologyType::TriangleList: {
				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				break;
			}
			default:
				OS::AssertFailMessage("Invalid TopologyType value used.");
			};
		}

		static VkFrontFace ToVulkanType(FrontFace frontFace) {
			switch (frontFace) {
			case FrontFace::Clockwise: {
				return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
				break;
			}
			case FrontFace::CounterClockwise: {
				return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
				break;
			}
			default:
				OS::AssertFailMessage("Invalid FrontFace value used.");
			};
		}

		static VkDescriptorType ToVulkanType(ShaderBinding::Type bindingType) {
			switch (bindingType) {
			case ShaderBinding::Type::Sampler: {
				return VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				break;
			}
			case ShaderBinding::Type::Uniform: {
				return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				break;
			}
			default:
				OS::AssertFailMessage("Invalid ShaderBinding::Type value used.");
			};
		}


		static VkShaderStageFlagBits ToVulkanType(ShaderBinding::Stage stage) {
			switch (stage) {
			case ShaderBinding::Stage::VertexShader: {
				return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
				break;
			}
			case ShaderBinding::Stage::FragmentShader: {
				return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
				break;
			}
			default:
				OS::AssertFailMessage("Invalid ShaderBinding::Stage value used.");
			};
		}

		static VkVertexInputBindingDescription GetVertexBindingDescription(VertexType vertexType) {
			switch (vertexType) {
			case VertexType::VF3_NF3_TF2: {
				return Vertex3fnt::GetBindingDescription();
				break;
			}
			case VertexType::VF2_TF2_CF4: {
				return Vertex2ftc::GetBindingDescription();
				break;
			}
			default:
				OS::AssertFailMessage("Invalid VertexType value used.");
			};
		}

		static std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions(VertexType vertexType) {
			switch (vertexType) {
			case VertexType::VF3_NF3_TF2: {
				return Vertex3fnt::GetAttributeDescriptions();
				break;
			}
			case VertexType::VF2_TF2_CF4: {
				return Vertex2ftc::GetAttributeDescriptions();
				break;
			}
			default:
				OS::AssertFailMessage("Invalid VertexType value used.");
			};
		}

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
			const Common::Size descriptorPoolSize = swapChain_->GetImagesNumber() * 1000;
			descriptorPool_ = std::make_shared<DescriptorPool>(logicDevice_, descriptorPoolSize);

			//DEPTH BUFFER
			{
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
						depthImageCreateInfo.samplesCount_ = physicalDevice_->GetMaxUsableSampleCount();
					}
					depthTestData->image_ = std::make_shared<AllocatedImage>(depthImageCreateInfo);
					depthTestData->imageView_ = CreateImageViewByImage(logicDevice_, depthTestData->image_, VK_IMAGE_ASPECT_DEPTH_BIT);
				}
				depthTestData_ = depthTestData;
				//}
			}

			//Multisampling
			{
				auto multisamplingData = std::make_shared<MultisamplingData>();
				{
					Image::CreateInfo multisamplingCreateInfo;
					{
						multisamplingCreateInfo.physicalDevice_ = physicalDevice_;
						multisamplingCreateInfo.logicDevice_ = logicDevice_;
						multisamplingCreateInfo.format_ = swapChain_->GetFormat().format;
						multisamplingCreateInfo.size_ = swapChain_->GetSize();
						multisamplingCreateInfo.tiling_ = VK_IMAGE_TILING_OPTIMAL;
						multisamplingCreateInfo.usage_ = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
						multisamplingCreateInfo.samplesCount_ = physicalDevice_->GetMaxUsableSampleCount();
					}
					multisamplingData->image_ = std::make_shared<AllocatedImage>(multisamplingCreateInfo);
					multisamplingData->imageView_ = CreateImageViewByImage(logicDevice_, multisamplingData->image_, VK_IMAGE_ASPECT_COLOR_BIT);
				}
				multisamplingData_ = multisamplingData;
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
				RPCreateInfo.samplesCount_ = physicalDevice_->GetMaxUsableSampleCount();
				RPCreateInfo.depthTestInfo_ = depthTestInfo;
			}
			renderPass_ = std::make_shared<RenderPass>(RPCreateInfo);


			for (auto& pipeline : info.pipelines2_) {

				std::vector<std::shared_ptr<DescriptorSetLayout>> DSLs;
				for (auto& binding : pipeline.shaderBindings_) {
					std::vector<VkDescriptorSetLayoutBinding> bindings;
					VkDescriptorSetLayoutBinding vBinding{
						binding.binding_,
						ToVulkanType(binding.type_),
						1,
						ToVulkanType(binding.stage_),
						nullptr
					};
					bindings.push_back(vBinding);

					auto DSL = std::make_shared<DescriptorSetLayout>(
						DescriptorSetLayout::CreateInfo{
							"GlobalData",
							logicDevice_,
							bindings
						});
					DSLs.push_back(DSL);
				}


				std::shared_ptr<Vulkan::Pipeline::DepthTestInfo> depthTestData;
				if (pipeline.enableDepthTest_) {
					depthTestData = std::make_shared<Vulkan::Pipeline::DepthTestInfo>();
					depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
				}

				Vulkan::Pipeline::CreateInfo createInfo{
					.physicalDevice_ = physicalDevice_,
					.logicDevice_ = logicDevice_,
					.renderPass_ = renderPass_,
					.pushConstants_ = {},
					.descriptorSetLayouts_ = DSLs,
					.vertexShader_ = std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
						logicDevice_,
						std::dynamic_pointer_cast<Shader>(pipeline.vertexShader_)->GetSpirv()
						}),
					.fragmentShader_ = std::make_shared<ShaderModule>(ShaderModule::CreateInfo{
						createInfo.logicDevice_,
						std::dynamic_pointer_cast<Shader>(pipeline.fragmentShader_)->GetSpirv()
						}),
					.depthTestInfo_ = depthTestData,
					.colorAttachmentSize_ = swapChain_->GetSize(),
					.colorAttachmentFormat_ = swapChain_->GetFormat().format,
					.multisamplingSamplesCount_ = physicalDevice_->GetMaxUsableSampleCount(),
					.vertexInfo_ = Vulkan::Pipeline::VertexInfo{
						GetVertexBindingDescription(pipeline.vertexType_),
						GetVertexAttributeDescriptions(pipeline.vertexType_)

					},
					.topology_ = ToVulkanType(pipeline.topologyType_),
					.frontFace_ = ToVulkanType(pipeline.frontFace_),
					.cullMode_ = ToVulkanType(pipeline.cullMode_)
				};
				namePipeline_[pipeline.name_] = std::make_shared<Vulkan::Pipeline>(createInfo);
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
					createInfo.multisamplingSamplesCount_ = physicalDevice_->GetMaxUsableSampleCount();
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = std::dynamic_pointer_cast<Shader>(imguiPipelineInfo->vertexShader_);
					createInfo.fragmentShader_ = std::dynamic_pointer_cast<Shader>(imguiPipelineInfo->fragmentShader_);
					if (imguiPipelineInfo->enableDepthTest_) {
						auto depthTestData = std::make_shared<LinesPipeline::DepthTestInfo>();
						depthTestData->bufferFormat_ = depthTestData_->image_->GetFormat();
						createInfo.depthTestInfo_ = depthTestData;
					}
				}
				imguiPipeline_ = std::make_shared<ImguiPipeline>(createInfo);
				//namePipeline_[info.imguiPipeline_->name_] = imguiPipeline_;
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
					createInfo.multisamplingSamplesCount_ = physicalDevice_->GetMaxUsableSampleCount();
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = std::dynamic_pointer_cast<Shader>(linesPipelineInfo->vertexShader_);
					createInfo.fragmentShader_ = std::dynamic_pointer_cast<Shader>(linesPipelineInfo->fragmentShader_);
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
					createInfo.multisamplingSamplesCount_ = physicalDevice_->GetMaxUsableSampleCount();
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = std::dynamic_pointer_cast<Shader>(texturedPipelineInfo->vertexShader_);
					createInfo.fragmentShader_ = std::dynamic_pointer_cast<Shader>(texturedPipelineInfo->fragmentShader_);
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
					createInfo.multisamplingSamplesCount_ = physicalDevice_->GetMaxUsableSampleCount();
					createInfo.colorAttachmentSize_ = swapChain_->GetSize();
					createInfo.vertexShader_ = std::dynamic_pointer_cast<Shader>(flatShadedPipelineInfo->vertexShader_);
					createInfo.fragmentShader_ = std::dynamic_pointer_cast<Shader>(flatShadedPipelineInfo->fragmentShader_);
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
						createInfo.colorAttachmentResolve_ = multisamplingData_->imageView_;
					}
					FrameBuffer frameBuffer{ createInfo };
					frameBuffers_.push_back(std::move(frameBuffer));
				}
				OS::LogInfo("/render/vulkan/driver/", "Frame buffers created successfuly.");
			}


			for (Common::Index i = 0; i < frameBuffers_.size(); i++) {
				DescriptorSet::CreateInfo createInfo;
				{
					createInfo.descriptorPool_ = descriptorPool_;
					createInfo.DSL_ = globalDataDSL_;
					createInfo.logicDevice_ = logicDevice_;
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
			OS::LogInfo("/render/vulkan/driver/", "Vulkan driver initialized successfuly.");
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
					const bool enableDepthTest = (depthTestData_ != nullptr);
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
						const auto texture = GetTextureById(shape->GetTexture());
						descriptorSets.push_back(texture->GetDS());
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
						const auto texture = GetTextureById(shape->GetTexture());
						descriptorSets.push_back(texture->GetDS());
						commandBuffer->BindDescriptorSets(imguiPipeline_, descriptorSets);
					}
					commandBuffer->DrawShape(shape);
				}

				for (auto& [id, shape] : shapes2_) {
					commandBuffer->BindPipeline(namePipeline_[shape->GetPipelineName()]);
					commandBuffer->BindShape2(shape);
					{
						std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
						descriptorSets.push_back(shape->GetTransformDescriptorSet());
						const auto texture = GetTextureById(shape->GetTextureId());
						descriptorSets.push_back(texture->GetDS());
						commandBuffer->BindDescriptorSets(namePipeline_[shape->GetPipelineName()], descriptorSets);
					}
					commandBuffer->DrawShape2(shape);
				}


				/*std::vector<Geom::Vertex3fc> lines{
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

				Buffer::DataCopy(vertexStagingBuffer, vertex3fcBuffer, logicDevice_, commandPool_);

				commandBuffer->BindPipeline(linesPipeline_);
				commandBuffer->BindBuffer(vertex3fcBuffer);
				std::vector<std::shared_ptr<DescriptorSet>> descriptorSets{};
				descriptorSets.push_back(globalDataDSs_[i]);
				commandBuffer->BindDescriptorSets(linesPipeline_, descriptorSets);
				commandBuffer->Draw(lines.size());*/

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

		//void RemoveUIShape(Common::Index shapeIndex) {
		//	for (auto& shape : UIShapes_) {
		//		if (shape->GetId() == shapeIndex) {

		//		}
		//	}
		//}


		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const RAL::Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber) override {

			AllocatedVertexBuffer<Vertex3fnc>::CreateInfo vertexBufferCreateInfo{};
			{
				vertexBufferCreateInfo.logicDevice_ = logicDevice_;
				vertexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				vertexBufferCreateInfo.commandPool_ = commandPool_;
				vertexBufferCreateInfo.verticesNumber_ = verticesNumber;
				vertexBufferCreateInfo.vertices_ = (const Vertex3fnc*)vertices;
			}
			auto allocatedVertexBuffer = std::make_shared<AllocatedVertexBuffer<Vertex3fnc>>(vertexBufferCreateInfo);

			AllocatedIndexBuffer<Index16>::CreateInfo indexBufferCreateInfo{};
			{
				indexBufferCreateInfo.logicDevice_ = logicDevice_;
				indexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				indexBufferCreateInfo.commandPool_ = commandPool_;
				indexBufferCreateInfo.indicesNumber_ = indicesNumber;
				indexBufferCreateInfo.indices_ = (const Index16*)indices;
			}
			auto allocatedIndexBuffer = std::make_shared<AllocatedIndexBuffer<Index16>>(indexBufferCreateInfo);

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
				shapeCreateInfo.vertexBuffer_ = allocatedVertexBuffer;
				shapeCreateInfo.indexBuffer_ = allocatedIndexBuffer;
				shapeCreateInfo.transformBuffer_ = transformUniformBuffer;
				shapeCreateInfo.modelDataDescriptorSet = modelTransform;
			}
			auto newShape = std::make_shared<ColoredShape>(shapeCreateInfo);
			coloredShapes_.push_back(newShape);
			return newShape->GetId();
		}


		virtual Common::UInt64 DrawIndexed(
			glm::vec2 scale,
			glm::vec2 translate,
			const RAL::Vertex2ftc* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber,
			RAL::Texture::Id textureId) override {

			AllocatedVertexBuffer<Vertex2ftc>::CreateInfo vertexBufferCreateInfo{};
			{
				vertexBufferCreateInfo.logicDevice_ = logicDevice_;
				vertexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				vertexBufferCreateInfo.commandPool_ = commandPool_;
				vertexBufferCreateInfo.verticesNumber_ = verticesNumber;
				vertexBufferCreateInfo.vertices_ = (const Vertex2ftc*)vertices;
			}
			auto allocatedVertexBuffer = std::make_shared<AllocatedVertexBuffer<Vertex2ftc>>(vertexBufferCreateInfo);

			AllocatedIndexBuffer<Index16>::CreateInfo indexBufferCreateInfo{};
			{
				indexBufferCreateInfo.logicDevice_ = logicDevice_;
				indexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				indexBufferCreateInfo.commandPool_ = commandPool_;
				indexBufferCreateInfo.indicesNumber_ = indicesNumber;
				indexBufferCreateInfo.indices_ = (const Index16*)indices;
			}
			auto allocatedIndexBuffer = std::make_shared<AllocatedIndexBuffer<Index16>>(indexBufferCreateInfo);

			//Texture::CreateInfo textureCreateInfo{};
			//{
			//	textureCreateInfo.name_ = "ImGui texture";
			//	textureCreateInfo.physicalDevice_ = physicalDevice_;
			//	textureCreateInfo.logicDevice_ = logicDevice_;
			//	textureCreateInfo.format_ = VK_FORMAT_R8G8B8A8_UNORM;
			//	textureCreateInfo.size_ = texture->GetSize();
			//	textureCreateInfo.pixels_ = texture->GetPixels/*<RAL::Color4b>*/();
			//	textureCreateInfo.commandPool_ = commandPool_;
			//	textureCreateInfo.descriptorPool_ = descriptorPool_;
			//}
			//auto vkTexture = std::make_shared<Texture>(textureCreateInfo);

			struct Transform {
				glm::vec2 scale_;
				alignas(8) glm::vec2 translate_;
			} transform{ scale, translate };
			const VkDeviceSize bufferSize = sizeof(Transform);
			auto transformUniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, bufferSize);

			transformUniformBuffer->Fill(&transform);
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
				texturedShapeCreateInfo.model_ = glm::mat3{ 1.f };
				texturedShapeCreateInfo.transformBuffer_ = transformUniformBuffer;
				texturedShapeCreateInfo.transformDescriptorSet_ = modelTransform;
				texturedShapeCreateInfo.vertexBuffer_ = allocatedVertexBuffer;
				texturedShapeCreateInfo.indexBuffer_ = allocatedIndexBuffer;
				texturedShapeCreateInfo.textureId_ = textureId;
			}
			auto texturedShape = std::make_shared<UIShape>(texturedShapeCreateInfo);
			UIShapes_.push_back(texturedShape);
			return texturedShape->GetId();
		}

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const RAL::Vertex3fnt* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber,
			RAL::Texture::Id textureId) override {

			AllocatedVertexBuffer<Vertex3fnt>::CreateInfo vertexBufferCreateInfo{};
			{
				vertexBufferCreateInfo.logicDevice_ = logicDevice_;
				vertexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				vertexBufferCreateInfo.commandPool_ = commandPool_;
				vertexBufferCreateInfo.verticesNumber_ = verticesNumber;
				vertexBufferCreateInfo.vertices_ = (const Vertex3fnt*)vertices;
			}
			auto allocatedVertexBuffer = std::make_shared<AllocatedVertexBuffer<Vertex3fnt>>(vertexBufferCreateInfo);

			AllocatedIndexBuffer<Index16>::CreateInfo indexBufferCreateInfo{};
			{
				indexBufferCreateInfo.logicDevice_ = logicDevice_;
				indexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				indexBufferCreateInfo.commandPool_ = commandPool_;
				indexBufferCreateInfo.indicesNumber_ = indicesNumber;
				indexBufferCreateInfo.indices_ = (const Index16*)indices;
			}
			auto allocatedIndexBuffer = std::make_shared<AllocatedIndexBuffer<Index16>>(indexBufferCreateInfo);

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
				texturedShapeCreateInfo.vertexBuffer_ = allocatedVertexBuffer;
				texturedShapeCreateInfo.indexBuffer_ = allocatedIndexBuffer;
				texturedShapeCreateInfo.textureId_ = textureId;
			}
			auto texturedShape = std::make_shared<TexturedShape>(texturedShapeCreateInfo);
			texturedShapes_.push_back(texturedShape);
			return texturedShape->GetId();
		}

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

			AllocatedVertexBuffer<Vertex3fnc>::CreateInfo vertexBufferCreateInfo{};
			{
				vertexBufferCreateInfo.logicDevice_ = logicDevice_;
				vertexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				vertexBufferCreateInfo.commandPool_ = commandPool_;
				vertexBufferCreateInfo.verticesNumber_ = verticesNumber;
				vertexBufferCreateInfo.vertices_ = (const Vertex3fnc*)vertices;
			}
			auto allocatedVertexBuffer = std::make_shared<AllocatedVertexBuffer<Vertex3fnc>>(vertexBufferCreateInfo);

			AllocatedIndexBuffer<Index16>::CreateInfo indexBufferCreateInfo{};
			{
				indexBufferCreateInfo.logicDevice_ = logicDevice_;
				indexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				indexBufferCreateInfo.commandPool_ = commandPool_;
				indexBufferCreateInfo.indicesNumber_ = indicesNumber;
				indexBufferCreateInfo.indices_ = (const Index16*)indices;
			}
			auto allocatedIndexBuffer = std::make_shared<AllocatedIndexBuffer<Index16>>(indexBufferCreateInfo);

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
				createInfo.vertexBuffer_ = allocatedVertexBuffer;
				createInfo.indexBuffer_ = allocatedIndexBuffer;
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


			AllocatedVertexBuffer<Vertex3fnc>::CreateInfo vertexBufferCreateInfo{};
			{
				vertexBufferCreateInfo.logicDevice_ = logicDevice_;
				vertexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				vertexBufferCreateInfo.commandPool_ = commandPool_;
				vertexBufferCreateInfo.verticesNumber_ = verticesNumber;
				vertexBufferCreateInfo.vertices_ = (const Vertex3fnc*)vertices;
			}
			auto allocatedVertexBuffer = std::make_shared<AllocatedVertexBuffer<Vertex3fnc>>(vertexBufferCreateInfo);

			AllocatedIndexBuffer<Index16>::CreateInfo indexBufferCreateInfo{};
			{
				indexBufferCreateInfo.logicDevice_ = logicDevice_;
				indexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				indexBufferCreateInfo.commandPool_ = commandPool_;
				indexBufferCreateInfo.indicesNumber_ = indicesNumber;
				indexBufferCreateInfo.indices_ = (const Index16*)indices;
			}
			auto allocatedIndexBuffer = std::make_shared<AllocatedIndexBuffer<Index16>>(indexBufferCreateInfo);

			ColoredShape::CreateInfo createInfo;
			{
				createInfo.model_ = model_;
				createInfo.vertexBuffer_ = allocatedVertexBuffer;
				createInfo.indexBuffer_ = allocatedIndexBuffer;
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

		[[nodiscard]]
		virtual RAL::Texture::Id CreateTexture(const RAL::Texture::CreateInfo& createInfo) override {

			const Texture::CreateInfo textureCreateInfo{
				.ralCreateInfo_ = createInfo,
				.physicalDevice_ = physicalDevice_,
				.logicDevice_ = logicDevice_,
				.commandPool_ = commandPool_,
				.descriptorPool_ = descriptorPool_,
				.format_ = VK_FORMAT_R8G8B8A8_UNORM
			};
			auto texture = std::make_shared<Texture>(std::move(textureCreateInfo));

			const RAL::Texture::Id textureId = textures_.size();
			textures_[textureId] = texture;
			return textureId;
		}

		//Shape2 realization
	public:



		[[nodiscard]]
		Common::Size VertexTypeToSize(VertexType vertexType) {
			switch (vertexType) {
			case  VertexType::VF3_NF3_TF2: {
				return sizeof(Vertex3fnt);
				break;
			}
			case  VertexType::VF2_TF2_CF4: {
				return sizeof(Vertex2ftc);
				break;
			}
			default: {
				OS::AssertFailMessage("Unsupported vertex type.");
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
			default: {
				OS::AssertFailMessage("Unsupported index type.");
			}
			};
		}

		[[nodiscard]]
		virtual Common::Id AddShapeToDraw(
			const std::string& pipelineName,
			const void* transform_,
			Common::Size transformSize,
			const void* vertices,
			Common::Size verticesNumber,
			VertexType vertexType,
			const void* indices,
			Common::Size indicesNumber,
			IndexType indexType,
			RAL::Texture::Id textureId) override {


			AllocatedVertexBuffer2::CreateInfo vertexBufferCreateInfo{};
			{
				vertexBufferCreateInfo.logicDevice_ = logicDevice_;
				vertexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				vertexBufferCreateInfo.commandPool_ = commandPool_;
				vertexBufferCreateInfo.verticesNumber_ = verticesNumber;
				vertexBufferCreateInfo.vertices_ = (const Vertex2ftc*)vertices;
				vertexBufferCreateInfo.vertexSize_ = VertexTypeToSize(vertexType);
			}
			auto allocatedVertexBuffer = std::make_shared<AllocatedVertexBuffer2>(vertexBufferCreateInfo);

			AllocatedIndexBuffer2::CreateInfo indexBufferCreateInfo{};
			{
				indexBufferCreateInfo.logicDevice_ = logicDevice_;
				indexBufferCreateInfo.physicalDevice_ = physicalDevice_;
				indexBufferCreateInfo.commandPool_ = commandPool_;
				indexBufferCreateInfo.indicesNumber_ = indicesNumber;
				indexBufferCreateInfo.indices_ = indices;
				indexBufferCreateInfo.indexSize_ = IndexTypeToSize(indexType);
			}
			auto allocatedIndexBuffer = std::make_shared<AllocatedIndexBuffer2>(indexBufferCreateInfo);

			auto transformUniformBuffer = std::make_shared<UniformBuffer>(physicalDevice_, logicDevice_, transformSize);
			DescriptorSet::CreateInfo transformDescriptorSetCreateInfo;
			{
				transformDescriptorSetCreateInfo.descriptorPool_ = descriptorPool_;
				transformDescriptorSetCreateInfo.DSL_ = texturedModelPipeline_->GetLayout()->GetDSLs()[1];
				transformDescriptorSetCreateInfo.logicDevice_ = logicDevice_;
			}
			auto modelTransform = std::make_shared<DescriptorSet>(transformDescriptorSetCreateInfo);
			modelTransform->UpdateBufferWriteConfiguration(
				transformUniformBuffer,
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				0, 0, transformUniformBuffer->GetSizeInBytes());

			Shape2::CreateInfo shape2CreateInfo;
			{
				std::memcpy(shape2CreateInfo.transform_, (const char*)transform_, transformSize);
				shape2CreateInfo.transformSize_ = transformSize;
				shape2CreateInfo.transformBuffer_ = transformUniformBuffer;
				shape2CreateInfo.transformDescriptorSet_ = modelTransform;
				shape2CreateInfo.vertexBuffer_ = allocatedVertexBuffer;
				shape2CreateInfo.indexBuffer_ = allocatedIndexBuffer;
				shape2CreateInfo.textureId_ = textureId;
				shape2CreateInfo.pipelineName_ = pipelineName;
			}
			auto shape2 = std::make_shared<Shape2>(shape2CreateInfo);
			shape2->SetTransformMatrix(transform_, transformSize);
			const Shape2::Id shape2Id = shapes2IdsGenerator_.Generate();
			shapes2_[shape2Id] = shape2;
			return shape2Id;

		}

		virtual void ResumeShapeDrawing(Common::Id shapeId) override {
			shapes2_[shapeId]->ResumeDrawing();
		}

		virtual void StopShapeDrawing(Common::Id shapeId) override {
			shapes2_[shapeId]->StopDrawing();
		}

		virtual void RemoveShapeFromDrawing(Common::Id shapeId) override {
			shapes2_.erase(shapeId);
		}

	private:

		std::map<Shape2::Id, std::shared_ptr<Shape2>> shapes2_;
		Common::IdGenerator shapes2IdsGenerator_;
		std::map<std::string, std::shared_ptr<Vulkan::Pipeline>> namePipeline_;
	private:

		[[nodiscard]]
		std::shared_ptr<Texture> GetTextureById(RAL::Texture::Id id) noexcept {
			return textures_[id];
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
		std::shared_ptr<MultisamplingData> multisamplingData_ = nullptr;

		std::map<Common::Id, std::shared_ptr<Texture>> textures_;

	};

}