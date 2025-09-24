#include <Render.Vulkan.Driver.Instance.hpp>

#include <Render.Vulkan.Driver.PhysicalDevice.hpp>

namespace Render::Vulkan {

	Instance::Instance(const CreateInfo& info) noexcept {

		const Extensions enabledExtensions = GetEnabledExtensions();
		{
			OS::LogInfo("/render/vulkan/driver", "Vulkan enabled extensions:");
			for (const Extension& extension : enabledExtensions) {
				OS::LogInfo("/render/vulkan/driver", extension.GetRawName());
			}
		}
		{
			OS::LogInfo("/render/vulkan/driver", "Checking are required extensions enabled.");
			for (const Extension& requiredExtension : info.requiredExtensions_) {
				ASSERT_FMSG(enabledExtensions.Contains(requiredExtension), "Required extension {} is not enabled.", requiredExtension.GetRawName());
			}
			OS::LogInfo("/render/vulkan/driver", "All required extensions are enabled.");
		}

		VkInstanceCreateInfo createInfo{};
		{	//GLSL Shaders DebugPrintf feature ON:
			{
				// Populate the VkValidationFeaturesEXT
				VkValidationFeaturesEXT validationFeatures = {};
				validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
				validationFeatures.enabledValidationFeatureCount = 1;

				VkValidationFeatureEnableEXT enabledValidationFeatures[1] = {
					VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
				validationFeatures.pEnabledValidationFeatures = enabledValidationFeatures;

				// Then add the VkValidationFeaturesEXT to the VkInstanceCreateInfo
				validationFeatures.pNext = createInfo.pNext;
				createInfo.pNext = &validationFeatures;
			}

			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			
			VkApplicationInfo appInfo{};
			{
				appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
				appInfo.pApplicationName = "OksEngine";
				appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.pEngineName = "No Engine";
				appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
				appInfo.apiVersion = VK_API_VERSION_1_0;
			}

			createInfo.pApplicationInfo = &appInfo;

			const auto extensionNames = info.requiredExtensions_.GetRawNames();
			createInfo.enabledExtensionCount = static_cast<Common::UInt32>(extensionNames.size());
			createInfo.ppEnabledExtensionNames = extensionNames.data();

			const auto rawNames = info.requiredValidationLayers_.GetRawNames();
			createInfo.enabledLayerCount = static_cast<Common::UInt32>(rawNames.size());
			createInfo.ppEnabledLayerNames = rawNames.data();
			VkInstance instance = VK_NULL_HANDLE;
			VkCall(vkCreateInstance(&createInfo, nullptr, &instance), 
				"Error while creating instance.");
			SetHandle(instance);
		}
		OS::LogInfo("/render/vulkan/driver", "Instance created successfuly.");
	}

	[[nodiscard]]
	std::vector<std::shared_ptr<PhysicalDevice>> Instance::GetPhysicalDevices() noexcept {

		uint32_t physicalDevicesCount = 0;
		vkEnumeratePhysicalDevices(GetHandle(), &physicalDevicesCount, nullptr);

		std::vector<VkPhysicalDevice> devices(physicalDevicesCount, VK_NULL_HANDLE);
		vkEnumeratePhysicalDevices(GetHandle(), &physicalDevicesCount, devices.data());

		std::vector<std::shared_ptr<PhysicalDevice>> physicalDevices;
		for (VkPhysicalDevice physicalDeviceHandle : devices) {
			PhysicalDevice::CreateInfo createInfo;
			{
				createInfo.handle_ = physicalDeviceHandle;
			}
			physicalDevices.push_back(std::make_shared<PhysicalDevice>(createInfo));
		}

		return physicalDevices;
	}


	[[nodiscard]]
	Extensions Instance::GetEnabledExtensions() noexcept {

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		Extensions extensionNames{};
		for (const auto& extensionProperties : extensions) {
			extensionNames.AddExtension(extensionProperties.extensionName);
		}

		return extensionNames;
	}

	void Instance::Destroy() noexcept {
		ASSERT_FMSG(GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkInstance.");
		vkDestroyInstance(GetHandle(), nullptr);
		SetHandle(VK_NULL_HANDLE);
		OS::LogError("/render/vulkan/driver", "Instance destroyed successfuly.");
	}

	Instance::~Instance() noexcept {
		Destroy();
	}

}
