#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <OS.Logger.hpp>
#include <OS.Assert.hpp>

#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.Instance.hpp>
//
// Render.Vulkan.

namespace Render::Vulkan {

	class Debug {
	public:

		struct CreateInfo {
			ValidationLayers requiredValidationLayers_;
			std::shared_ptr<Instance> instance_ = nullptr;
		};

		Debug(const CreateInfo& createInfo) : instance_{ createInfo.instance_ } {

			const ValidationLayers availableValidationLayers = GetAvailableValidationLayers();
			
			{
				OS::LogInfo("/render/vulkan/driver/debug", "Vulkan enabled layers: ");
				for (const ValidationLayer& validationLayer : availableValidationLayers) {
					OS::LogInfo("/render/vulkan/driver/debug", validationLayer.GetRawName());
				}

				OS::LogInfo("/render/vulkan/driver/debug", "Checking are required validation layers enabled.");
				for (const ValidationLayer& requiredLayer : createInfo.requiredValidationLayers_) {
					[[maybe_unused]]
					const bool isRequiredValidationLayerAvailable = availableValidationLayers.IsContains(requiredLayer);
					ASSERT_FMSG(isRequiredValidationLayerAvailable, "Required validation layer {} is not enabled.", requiredLayer.GetRawName());
				}
				OS::LogInfo("/render/vulkan/driver/debug", "All required validation layers are enabled.");
			}

			VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
			{
				messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				messengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				messengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				messengerCreateInfo.pfnUserCallback = Debug::Callback;
				messengerCreateInfo.pUserData = nullptr; // Optional
			}
			{
				auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(createInfo.instance_->GetHandle(), "vkCreateDebugUtilsMessengerEXT");
				ASSERT_FMSG(func != nullptr, "VK_EXT_debug_utils extension is not enabled.");
				VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
				[[maybe_unused]]
				const VkResult result = func(createInfo.instance_->GetHandle(), &messengerCreateInfo, nullptr, &debugMessenger);
				ASSERT_FMSG(result == VK_SUCCESS, "Error while creating PFN_vkCreateDebugUtilsMessengerEXT.");
				SetNative(debugMessenger);
				OS::LogInfo("/render/vulkan/driver/debug", "Debug messenger was created successfuly.");
			}
		}

		[[nodiscard]]
		static ValidationLayers GetAvailableValidationLayers() noexcept {
			uint32_t layerCount;
			{
				[[maybe_unused]]
				const VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
				ASSERT_FMSG(result == VK_SUCCESS, "Error while getting number of available validation layers.");
			}
			std::vector<VkLayerProperties> availableLayers(layerCount);
			{
				[[maybe_unused]]
				const VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
				ASSERT_FMSG(result == VK_SUCCESS, "Error while getting available validation layers.");
			}

			ValidationLayers availableLayerNames{};
			for (const VkLayerProperties& layer : availableLayers) {
				availableLayerNames.AddLayer(layer.layerName);
			}

			return availableLayerNames;
		}

		static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData) {
			Common::DiscardUnusedParameter(pUserData);
			Common::DiscardUnusedParameter(messageType);
			if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT || messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
				ASSERT_FAIL_MSG(pCallbackData->pMessage);
			} else {
				OS::LogInfo("/render/vulkan/layers errors", pCallbackData->pMessage);
			}

			return VK_FALSE;
		}

		~Debug() noexcept {
			Destroy();
		}
		[[nodiscard]]
		const VkDebugUtilsMessengerEXT& GetNative() const noexcept { return debugMessenger_; }

		[[nodiscard]]
		void SetNative(VkDebugUtilsMessengerEXT debugMessenger) noexcept { 
			OS::Assert((debugMessenger != VK_NULL_HANDLE) && (GetNative() == VK_NULL_HANDLE) ||
				((debugMessenger == VK_NULL_HANDLE) && (GetNative() != VK_NULL_HANDLE)));
			debugMessenger_ = debugMessenger;
		}

	private:

		void Destroy() noexcept {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_->GetHandle(), "vkDestroyDebugUtilsMessengerEXT");
			ASSERT_FMSG(func != nullptr, "VK_EXT_debug_utils extension is not enabled.");
			ASSERT_FMSG(GetNative() != VK_NULL_HANDLE, "Attempt to destroy VK_NULL_HANDLE VkDebugUtilsMessengerEXT.");
			ASSERT_FMSG(instance_->GetHandle() != VK_NULL_HANDLE, "Attempt to destroy VkDebugUtilsMessengerEXT with VK_NULL_HANDLE VkInstance.");
			func(instance_->GetHandle(), GetNative(), nullptr);
			OS::LogError("/render/vulkan/driver", "Debug messenger destroyed.");
		}

	private:
		std::shared_ptr<Instance> instance_ = nullptr;
		VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;
	};
};