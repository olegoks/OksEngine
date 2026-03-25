#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Assert.hpp>
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <Render.Vulkan.Driver.WindowSurface.hpp>

namespace Render::Vulkan {

	class LogicDevice final : public Abstraction<VkDevice> {
	public:

		struct CreateInfo {
			std::shared_ptr<PhysicalDevice> physicalDevice_;
			Extensions requiredExtensions_;
			ValidationLayers requiredValidationLayers_;
			QueueFamily presentQueueFamily_;
			QueueFamily graphicsQueueFamily_;
			QueueFamily computeQueueFamily_;
		};

		LogicDevice(const CreateInfo& createInfo) noexcept {

			ASSERT_FMSG(createInfo.physicalDevice_ != nullptr, "Attempt create logic device using nullptr physical device.");

			VkDeviceCreateInfo logicDeviceCreateInfo{};
			{
				logicDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				logicDeviceCreateInfo.pNext = nullptr;
				const float queuePriority = 1.0f;
				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				//TODO: CHECK ALL POSSIBLE VARIATIONS OF queueFamilyIndices to get Logic Device to create only unique queue!!!
				{
					{
						VkDeviceQueueCreateInfo queueCreateInfo{};
						{
							queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
							queueCreateInfo.queueFamilyIndex = createInfo.graphicsQueueFamily_.index_;
							queueCreateInfo.queueCount = 1;

							queueCreateInfo.pQueuePriorities = &queuePriority;
						}
						queueCreateInfos.push_back(queueCreateInfo);
					}
					if (createInfo.graphicsQueueFamily_.index_ != createInfo.presentQueueFamily_.index_) {
						VkDeviceQueueCreateInfo queueCreateInfo{};
						{
							queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
							queueCreateInfo.queueFamilyIndex = createInfo.presentQueueFamily_.index_;
							queueCreateInfo.queueCount = 1;

							queueCreateInfo.pQueuePriorities = &queuePriority;
						}
						queueCreateInfos.push_back(queueCreateInfo);
					}
					if (createInfo.computeQueueFamily_.index_ != createInfo.presentQueueFamily_.index_) {
						VkDeviceQueueCreateInfo queueCreateInfo{};
						{
							queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
							queueCreateInfo.queueFamilyIndex = createInfo.computeQueueFamily_.index_;
							queueCreateInfo.queueCount = 1;

							queueCreateInfo.pQueuePriorities = &queuePriority;
						}
						queueCreateInfos.push_back(queueCreateInfo);
					}
				}
				{
					logicDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
					logicDeviceCreateInfo.queueCreateInfoCount = static_cast<Common::UInt32>(queueCreateInfos.size());
				}

				VkPhysicalDeviceFeatures deviceFeatures = createInfo.physicalDevice_->GetFeatures();

				//Enable only required features.
				{
					deviceFeatures.robustBufferAccess = VK_TRUE;        // Важно для стабильности
					deviceFeatures.samplerAnisotropy = VK_TRUE;        // Можно выключить для минимализма
					deviceFeatures.sampleRateShading = VK_FALSE;        
					deviceFeatures.textureCompressionETC2 = VK_FALSE;   
					deviceFeatures.textureCompressionASTC_LDR = VK_FALSE;
					deviceFeatures.textureCompressionBC = VK_FALSE;     
					deviceFeatures.occlusionQueryPrecise = VK_FALSE;    
					deviceFeatures.pipelineStatisticsQuery = VK_FALSE;  
					deviceFeatures.vertexPipelineStoresAndAtomics = VK_FALSE; 
					deviceFeatures.fragmentStoresAndAtomics = VK_FALSE;
					deviceFeatures.shaderTessellationAndGeometryPointSize = VK_FALSE;
					deviceFeatures.shaderImageGatherExtended = VK_FALSE; 
					deviceFeatures.shaderStorageImageExtendedFormats = VK_FALSE; 
					deviceFeatures.shaderStorageImageMultisample = VK_FALSE;
					deviceFeatures.shaderStorageImageReadWithoutFormat = VK_FALSE;
					deviceFeatures.shaderStorageImageWriteWithoutFormat = VK_FALSE;
					deviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_FALSE; 
					deviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
					deviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
					deviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
					deviceFeatures.shaderClipDistance = VK_FALSE;       
					deviceFeatures.shaderCullDistance = VK_FALSE;       
					deviceFeatures.shaderFloat64 = VK_FALSE;            
					deviceFeatures.shaderInt64 = VK_TRUE;              
					deviceFeatures.shaderInt16 = VK_FALSE;              
					deviceFeatures.shaderResourceResidency = VK_FALSE;  
					deviceFeatures.shaderResourceMinLod = VK_FALSE;     
					deviceFeatures.sparseBinding = VK_FALSE;            
					deviceFeatures.sparseResidencyBuffer = VK_FALSE;    
					deviceFeatures.sparseResidencyImage2D = VK_FALSE;   
					deviceFeatures.sparseResidencyImage3D = VK_FALSE;   
					deviceFeatures.sparseResidency2Samples = VK_FALSE;  
					deviceFeatures.sparseResidency4Samples = VK_FALSE;  
					deviceFeatures.sparseResidency8Samples = VK_FALSE;  
					deviceFeatures.sparseResidency16Samples = VK_FALSE; 
					deviceFeatures.sparseResidencyAliased = VK_FALSE;   
					deviceFeatures.variableMultisampleRate = VK_FALSE;  
					deviceFeatures.inheritedQueries = VK_FALSE;         
					deviceFeatures.multiDrawIndirect = VK_FALSE;        
					deviceFeatures.drawIndirectFirstInstance = VK_FALSE;
					deviceFeatures.depthClamp = VK_FALSE;               
					deviceFeatures.depthBiasClamp = VK_FALSE;           
					deviceFeatures.fillModeNonSolid = VK_FALSE;         
					deviceFeatures.depthBounds = VK_FALSE;              
					deviceFeatures.wideLines = VK_FALSE;                
					deviceFeatures.largePoints = VK_FALSE;              
					deviceFeatures.alphaToOne = VK_FALSE;               
					deviceFeatures.multiViewport = VK_FALSE;            
					deviceFeatures.tessellationShader = VK_FALSE;       
					deviceFeatures.geometryShader = VK_TRUE;           
					deviceFeatures.dualSrcBlend = VK_FALSE;             
					deviceFeatures.logicOp = VK_FALSE;           
					
				}
				logicDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;

				//Dynamic rendering feature.
				VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
				{
					dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
					dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
				}

				ASSERT(logicDeviceCreateInfo.pNext == nullptr);
				logicDeviceCreateInfo.pNext = &dynamicRenderingFeatures;

				//Bindless resources feature.
				VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures{};
				{
					indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
					indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;         // Частично заполненные массивы
					indexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE; // Переменный размер массива
					indexingFeatures.runtimeDescriptorArray = VK_TRUE;                   // Массивы дескрипторов в шейдере
					indexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE; // Не-uniform индексация (для текстур)
					indexingFeatures.descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE;
					indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
					indexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
					
				}
				VkDeviceCreateInfo deviceInfo{};
				dynamicRenderingFeatures.pNext = &indexingFeatures; // Добавляем в цепочку

				std::vector<const char*> rawExtensionsNames = createInfo.requiredExtensions_.GetRawNames();
				{
					logicDeviceCreateInfo.ppEnabledExtensionNames = rawExtensionsNames.data();
					logicDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(rawExtensionsNames.size());
				}

				std::vector<const char*> rawLayerNames = createInfo.requiredValidationLayers_.GetRawNames();
				{
					logicDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(rawLayerNames.size());
					logicDeviceCreateInfo.ppEnabledLayerNames = rawLayerNames.data();
				}

				VkDevice logicDevice = VK_NULL_HANDLE;
				VK_CALL(vkCreateDevice(createInfo.physicalDevice_->GetHandle(), &logicDeviceCreateInfo, nullptr, &logicDevice),
					"Error while creating logic device");
				SetHandle(logicDevice);
			}

			{
				vkGetDeviceQueue(GetHandle(), createInfo.graphicsQueueFamily_.index_, 0, &graphicsQueue_);
				ASSERT_FMSG(graphicsQueue_ != VK_NULL_HANDLE, "Error while getting qraphics queue family handle.");
			}
			{
				vkGetDeviceQueue(GetHandle(), createInfo.presentQueueFamily_.index_, 0, &presentQueue_);
				ASSERT_FMSG(presentQueue_ != VK_NULL_HANDLE, "Error while getting  present queue family handle.");
			}
			{
				vkGetDeviceQueue(GetHandle(), createInfo.computeQueueFamily_.index_, 0, &computeQueue_);
				ASSERT_FMSG(presentQueue_ != VK_NULL_HANDLE, "Error while getting  present queue family handle.");
			}

		}

		[[nodiscard]]
		VkQueue& GetComputeQueue() noexcept { return computeQueue_; }

		[[nodiscard]]
		VkQueue& GetGraphicsQueue() noexcept { return graphicsQueue_; }

		[[nodiscard]]
		VkQueue& GetPresentQueue() noexcept { return presentQueue_; }

		~LogicDevice() noexcept {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			vkDestroyDevice(GetHandle(), nullptr);
		}

	private:
		VkQueue computeQueue_;
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;
	};
}