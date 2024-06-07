#pragma once

#include <vulkan/vulkan.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>
#include <OS.Logger.hpp>
#include <OS.Assert.hpp>

namespace Render::Vulkan {


	class Instance {
	public:

		struct CreateInfo {
			Extensions requiredExtensions_;
			ValidationLayers requiredValidationLayers_;
		};

		Instance() noexcept = delete;

		Instance(const CreateInfo& info) noexcept {

			//vk::Instance
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
					OS::AssertMessage(enabledExtensions.Contains(requiredExtension), { "Required extension %s is not enabled.", requiredExtension.GetRawName() });
				}
				OS::LogInfo("/render/vulkan/driver", "All required extensions are enabled.");
			}

			VkInstanceCreateInfo createInfo{};
			{
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
				createInfo.enabledExtensionCount = extensionNames.size();
				createInfo.ppEnabledExtensionNames = extensionNames.data();

				const auto rawNames = info.requiredValidationLayers_.GetRawNames();
				createInfo.enabledLayerCount = 0; //<uint32_t>(rawNames.size());
				createInfo.ppEnabledLayerNames = rawNames.data();

				VkCall(vkCreateInstance(&createInfo, nullptr, &instance_), "Error while creating instance.");
			}
			OS::LogInfo("/render/vulkan/driver", "Instance created successfuly.");

		}

		[[nodiscard]]
		const VkInstance& GetNative() const noexcept { return instance_; }


		[[nodiscard]]
		std::vector<std::shared_ptr<PhysicalDevice>> GetPhysicalDevices() noexcept {

			uint32_t physicalDevicesCount = 0;
			vkEnumeratePhysicalDevices(GetNative(), &physicalDevicesCount, nullptr);

			std::vector<VkPhysicalDevice> devices(physicalDevicesCount, VK_NULL_HANDLE);
			vkEnumeratePhysicalDevices(GetNative(), &physicalDevicesCount, devices.data());

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
		static Extensions GetEnabledExtensions() noexcept {

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

		~Instance() {
			Destroy();
		}

	private:

		void Destroy() noexcept {
			OS::AssertMessage(GetNative() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkInstance.");
			vkDestroyInstance(GetNative(), nullptr);
			SetNative(VK_NULL_HANDLE);
			OS::LogError("/render/vulkan/driver", "Instance destroyed successfuly.");
		}

		void SetNative(VkInstance instance) noexcept {

			OS::Assert(	(instance != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
						((instance == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			instance_ = instance;
		}

	private:
		VkInstance instance_ = VK_NULL_HANDLE;
	};
}