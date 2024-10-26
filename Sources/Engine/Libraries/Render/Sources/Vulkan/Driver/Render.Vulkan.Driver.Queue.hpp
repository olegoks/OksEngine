
#include <Render.Vulkan.Common.hpp>
#include <Render.Vulkan.Driver.PhysicalDevice.hpp>

namespace Render::Vulkan {



	class QueueFamily2 {
	public:

		using Index = Common::UInt32;

		struct CreateInfo {
			Index index_ = Common::Limits<Index>::Max(); // Index of the queue family for this physics device.
			VkQueueFamilyProperties properties_{ 0 };
		};

		QueueFamily2(const CreateInfo& createInfo) : createInfo_{ createInfo } {}

		[[nodiscard]]
		Index GetIndex() const noexcept { return createInfo_.index_; }

		[[nodiscard]]
		bool SupportsGraphics() const noexcept {
			return createInfo_.properties_.queueFlags & VK_QUEUE_GRAPHICS_BIT;
		}

		CreateInfo createInfo_{};
	};

}