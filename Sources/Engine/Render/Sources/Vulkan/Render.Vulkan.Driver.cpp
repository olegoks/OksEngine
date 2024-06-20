
#include <Render.Vulkan.Driver.hpp>

namespace Render::Vulkan {

	[[nodiscard]]
	PhysicalDevice::Formats Driver::GetAvailablePhysicalDeviceSurfaceFormats(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

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
	PhysicalDevice::PresentModes Driver::GetAvailablePresentModes(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

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
	VkPresentModeKHR Driver::ChoosePresentMode(const PhysicalDevice::PresentModes& availablePresentModes) noexcept {

		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	[[nodiscard]]
	std::pair<QueueFamily, QueueFamily> Driver::ChooseQueueFamilies(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

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
	QueueFamilies Driver::GetPresentQueueFamilies(std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<WindowSurface> windowSurface) noexcept {

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

	[[nodiscard]]
	std::shared_ptr<PhysicalDevice> Driver::ChoosePhysicalDevice(const std::vector<std::shared_ptr<PhysicalDevice>>& availablePhysicalDevices, std::shared_ptr<WindowSurface> windowSurface, const Extensions& requiredExtensions) noexcept {

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
				OS::LogInfo("render/vulkan/driver", { "Found sutable device {}.", physicalDevice->GetName() });
				return physicalDevice;
			}
		}

		OS::AssertFailMessage("Failed to find a suitable GPU!");
		return nullptr;
	}
	
}