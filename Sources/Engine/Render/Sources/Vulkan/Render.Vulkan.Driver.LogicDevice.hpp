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
		};

		LogicDevice(const CreateInfo& createInfo) noexcept {

			OS::AssertMessage(createInfo.physicalDevice_ != nullptr, "Attempt create logic device using nullptr physical device.");

			VkDeviceCreateInfo logicDeviceCreateInfo{};
			{
				logicDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				const float queuePriority = 1.0f;
				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				{
					VkDeviceQueueCreateInfo queueCreateInfo{};
					{
						queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
						queueCreateInfo.queueFamilyIndex = createInfo.graphicsQueueFamily_.index_;
						queueCreateInfo.queueCount = 1;

						queueCreateInfo.pQueuePriorities = &queuePriority;
					}
					queueCreateInfos.push_back(queueCreateInfo);

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
				}
				{
					logicDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
					logicDeviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
				}

				const VkPhysicalDeviceFeatures deviceFeatures = createInfo.physicalDevice_->GetFeatures();
				logicDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;

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
				VkCall(vkCreateDevice(createInfo.physicalDevice_->GetHandle(), &logicDeviceCreateInfo, nullptr, &logicDevice), 
					"Error while creating logic device");
				SetHandle(logicDevice);
			}

			{
				vkGetDeviceQueue(GetHandle(), createInfo.graphicsQueueFamily_.index_, 0, &graphicsQueue_);
				OS::AssertMessage(graphicsQueue_ != VK_NULL_HANDLE, "Error while getting qraphics queue family handle.");
			}
			{
				vkGetDeviceQueue(GetHandle(), createInfo.presentQueueFamily_.index_, 0, &presentQueue_);
				OS::AssertMessage(presentQueue_ != VK_NULL_HANDLE, "Error while getting  present queue family handle.");
			}

		}

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
		VkQueue graphicsQueue_;
		VkQueue presentQueue_;
	};
}