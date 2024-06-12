#pragma once

#include <Render.Vulkan.Abstraction.hpp>

namespace Render::Vulkan {

	class PhysicalDevice final : public Abstraction<VkPhysicalDevice> {
	public:

		struct CreateInfo {
			VkPhysicalDevice handle_ = VK_NULL_HANDLE;
		};

		using Formats = std::vector<VkSurfaceFormatKHR>;
		using PresentModes = std::vector<VkPresentModeKHR>;

		PhysicalDevice(const CreateInfo& createInfo) noexcept : 
			Abstraction{ createInfo.handle_ },
			properties_{ GetProperties() }, 
			features_{ GetFeatures() }
		{

			OS::AssertMessage(GetHandle() != VK_NULL_HANDLE,
				"Attempt to create PhysicalDevice using VK_NULL_HANDLE VkPhysicalDevice.");

		}

		[[nodiscard]]
		bool IsDiscrete() const noexcept {
			return (properties_.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		}

		[[nodiscard]]
		bool SupportsGeometryShader() const noexcept {
			return (features_.geometryShader);
		}

		[[nodiscard]]
		Extensions GetAvailableExtensions() noexcept {

			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(GetHandle(), nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(GetHandle(), nullptr, &extensionCount, availableExtensions.data());

			Extensions extensions;
			for (const VkExtensionProperties& extension : availableExtensions) {
				extensions.AddExtension(extension.extensionName);
			}
			return extensions;
		}

		[[nodiscard]]
		VkFormatProperties GetFormatProperties(VkFormat format) noexcept {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(GetHandle(), format, &props);
			return props;
		}

		[[nodiscard]]
		uint32_t GetSuitableMemoryTypeIndex(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
			const VkPhysicalDeviceMemoryProperties memProperties = GetMemoryProperties();
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
				const VkMemoryType memoryType = memProperties.memoryTypes[i];
				if ((typeFilter & (1 << i)) && (memoryType.propertyFlags & properties) == properties) {
					return i;
				}
			}
			OS::AssertFailMessage("Suitable device memory was not found.");
#undef max
			return std::numeric_limits<uint32_t>::max();
		}

		[[nodiscard]]
		VkPhysicalDeviceProperties GetProperties() noexcept {

			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(GetHandle(), &deviceProperties);

			return deviceProperties;
		}

		[[nodiscard]]
		VkPhysicalDeviceMemoryProperties GetMemoryProperties() noexcept {

			VkPhysicalDeviceMemoryProperties memoryProperties;
			vkGetPhysicalDeviceMemoryProperties(GetHandle(), &memoryProperties);

 			return memoryProperties;
		}

		[[nodiscard]]
		QueueFamilies GetGraphicsQueueFamilies() noexcept {

			const QueueFamilies queueFamilies = GetQueueFamilies();
			QueueFamilies graphicsQueueFamilies;
			for (const QueueFamily& queueFamily : queueFamilies) {
				if (queueFamily.properties_.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					graphicsQueueFamilies.AddQueueFamily(queueFamily);
				}
			}
			return graphicsQueueFamilies;
		}

		[[nodiscard]]
		VkPhysicalDeviceFeatures GetFeatures() const noexcept {
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(GetHandle(), &deviceFeatures);
			return deviceFeatures;
		}

		[[nodiscard]]
		QueueFamilies GetQueueFamilies() noexcept {

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(GetHandle(), &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(GetHandle(), &queueFamilyCount, queueFamiliesProperties.data());

			QueueFamilies queueFamilies;
			for (Common::Index i = 0; i < queueFamiliesProperties.size(); i++) {
				QueueFamily queueFamily;
				{
					queueFamily.index_ = static_cast<Common::UInt32>(i);
					queueFamily.properties_ = queueFamiliesProperties[i];
				}
				queueFamilies.AddQueueFamily(queueFamily);
			}

			return queueFamilies;
		}


		[[nodiscard]]
		const char* GetName() const noexcept { return properties_.deviceName; }
			
		//[[nodiscard]]
		//static Formats GetAvailableFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface) noexcept {

		//	OS::AssertMessage(physicalDevice != VK_NULL_HANDLE, "VkPhysicalDevice is VK_NULL_HANDLE");
		//	OS::AssertMessage(windowSurface != VK_NULL_HANDLE, "VkSurfaceKHR is VK_NULL_HANDLE");

		//	uint32_t formatCount;
		//	[[maube_unused]]
		//	const VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &formatCount, nullptr);
		//	OS::AssertMessage(result == VK_SUCCESS, "Error while getting number of available formats.");

		//	std::vector<VkSurfaceFormatKHR> formats;
		//	if (formatCount != 0) {
		//		formats.resize(formatCount);
		//		[[maube_unused]]
		//		const VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &formatCount, formats.data());
		//		OS::AssertMessage(result == VK_SUCCESS, "Error while getting available formats.");
		//	}

		//	return formats;
		//}


		//[[nodiscard]]
		//static PresentModes GetAvailablePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface) noexcept {

		//	OS::AssertMessage(physicalDevice != VK_NULL_HANDLE, "VkPhysicalDevice is VK_NULL_HANDLE");
		//	OS::AssertMessage(windowSurface != VK_NULL_HANDLE, "VkSurfaceKHR is VK_NULL_HANDLE");

		//	std::vector<VkPresentModeKHR> presentModes;

		//	uint32_t presentModeCount;
		//	{
		//		[[maube_unused]]
		//		const VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &presentModeCount, nullptr);
		//		OS::AssertMessage(result == VK_SUCCESS, "Error while getting number of present modes.");
		//	}
		//	if (presentModeCount != 0) {
		//		presentModes.resize(presentModeCount);
		//		[[maube_unused]]
		//		const VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &presentModeCount, presentModes.data());
		//		OS::AssertMessage(result == VK_SUCCESS, "Error while getting number of present modes.");
		//	}

		//	return presentModes;
		//}


		//void ChooseQueueFamilies(VkSurfaceKHR windowSurface) noexcept {

		//	OS::AssertMessage(windowSurface != VK_NULL_HANDLE, "VkSurfaceKHR is VK_NULL_HANDLE");

		//	const QueueFamilies graphicsQueueFamilies = GetGraphicsQueueFamilies();
		//	OS::AssertMessage(!graphicsQueueFamilies.IsEmpty(), "Chosen physical device doesn't have any graphics queue family.");

		//	const QueueFamilies presentQueueFamilies = GetPresentQueueFamilies(GetHandle(), windowSurface);
		//	OS::AssertMessage(!graphicsQueueFamilies.IsEmpty(), "Chosen physical device doesn't have any present queue family.");

		//	for (const QueueFamily& graphicsQueueFamily : graphicsQueueFamilies) {
		//		for (const QueueFamily& presentQueueFamily : presentQueueFamilies) {
		//			if (graphicsQueueFamily.index_ == presentQueueFamily.index_) {
		//				chosenGraphicsQueueFamily_ = graphicsQueueFamily;
		//				chosenPresentQueueFamily_ = presentQueueFamily;
		//				OS::LogInfo("/render/vulkan/driver/physical device", "Found queue family that supports present and graphics commands.");
		//			}
		//		}
		//	}

		//	chosenGraphicsQueueFamily_ = *graphicsQueueFamilies.begin();
		//	chosenPresentQueueFamily_ = *presentQueueFamilies.begin();
		//}

		//void SetNative(VkPhysicalDevice physicalDevice) noexcept {
		//	OS::Assert(	(physicalDevice != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
		//				((physicalDevice == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
		//	handle_ = physicalDevice;
		//}
	private:
		//QueueFamilies graphicsQueueFamilies_;
		//QueueFamilies presentQueueFamilies_;
		//QueueFamily chosenGraphicsQueueFamily_;
		//QueueFamily chosenPresentQueueFamily_;
		VkPhysicalDeviceProperties properties_{ 0 };
		VkPhysicalDeviceFeatures features_{ 0 };
	};
}