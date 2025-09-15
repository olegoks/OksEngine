#include <Render.Vulkan.Common.hpp>

#include <vulkan/vk_enum_string_helper.h>

namespace Render::Vulkan {
	void VkCall(VkResult nativeAPICallResult, Common::Format&& format) noexcept {
		const bool success = (nativeAPICallResult == VK_SUCCESS);
		if (!success) {
			std::string errorMessage = string_VkResult(nativeAPICallResult);
			errorMessage += ". ";
			errorMessage += format;
			ASSERT_FAIL_MSG(errorMessage.c_str());
		}
	}
}