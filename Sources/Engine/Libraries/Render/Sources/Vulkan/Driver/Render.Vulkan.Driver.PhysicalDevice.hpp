#pragma once

#include <Render.Vulkan.Abstraction.hpp>
#include <Render.Vulkan.Driver.Queue.hpp>

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

			ASSERT_FMSG(GetHandle() != VK_NULL_HANDLE,
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
		bool SupportsMipMaps(VkFormat format) const noexcept {
			const VkFormatProperties formatProperties = GetFormatProperties(format);
			return formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
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
		VkFormatProperties GetFormatProperties(VkFormat format) const noexcept {
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
			ASSERT_FAIL_MSG("Suitable device memory was not found.");
			return Common::Limits<uint32_t>::Max();
		}

		[[nodiscard]]
		VkPhysicalDeviceProperties GetProperties() noexcept {

			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(GetHandle(), &deviceProperties);

			return deviceProperties;
		}

		[[nodiscard]]
		VkSampleCountFlagBits GetMaxUsableSampleCount() {
			VkPhysicalDeviceProperties physicalDeviceProperties = GetProperties();
			VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
			if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
			if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
			if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
			if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
			if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
			if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

			return VK_SAMPLE_COUNT_1_BIT;
		}


		[[nodiscard]]
		VkPhysicalDeviceMemoryProperties GetMemoryProperties() const noexcept {

			VkPhysicalDeviceMemoryProperties memoryProperties;
			vkGetPhysicalDeviceMemoryProperties(GetHandle(), &memoryProperties);

 			return memoryProperties;
		}

		[[nodiscard]]
		QueueFamilies GetGraphicsQueueFamilies() const noexcept {

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
		QueueFamilies GetComputeQueueFamilies() const noexcept {

			const QueueFamilies queueFamilies = GetQueueFamilies();
			QueueFamilies graphicsQueueFamilies;
			for (const QueueFamily& queueFamily : queueFamilies) {
				if (queueFamily.properties_.queueFlags & VK_QUEUE_COMPUTE_BIT) {
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
		QueueFamilies GetQueueFamilies() const noexcept {

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


		//[[nodiscard]]
		//QueueFamilies GetQueueFamilies2() const noexcept {

		//	uint32_t queueFamilyCount = 0;
		//	vkGetPhysicalDeviceQueueFamilyProperties(GetHandle(), &queueFamilyCount, nullptr);

		//	std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamilyCount);
		//	vkGetPhysicalDeviceQueueFamilyProperties(GetHandle(), &queueFamilyCount, queueFamiliesProperties.data());

		//	QueueFamilies queueFamilies;
		//	for (Common::Index i = 0; i < queueFamiliesProperties.size(); i++) {

		//		QueueFamily2::CreateInfo qfci{
		//		};

		//		QueueFamily2 queueFamily;
		//		{
		//			queueFamily.index_ = static_cast<Common::UInt32>(i);
		//			queueFamily.properties_ = queueFamiliesProperties[i];
		//		}
		//		queueFamilies.AddQueueFamily(queueFamily);
		//	}

		//	return queueFamilies;
		//}



		[[nodiscard]]
		const char* GetName() const noexcept { return properties_.deviceName; }

	private:

		VkPhysicalDeviceProperties properties_{ 0 };
		VkPhysicalDeviceFeatures features_{ 0 };
	};
}