#pragma once

//#include <Render.Vulkan.Driver.PhysicalDevice.hpp>

#include <Render.Vulkan.Abstraction.hpp>

namespace Render::Vulkan {


	class Instance : public Abstraction<VkInstance>{
	public:

		struct CreateInfo {
			Extensions requiredExtensions_;
			ValidationLayers requiredValidationLayers_;
		};

		Instance() noexcept = delete;

		Instance(const CreateInfo& info) noexcept;

		[[nodiscard]]
		std::vector<std::shared_ptr<class PhysicalDevice>> GetPhysicalDevices() noexcept;

		[[nodiscard]]
		static Extensions GetEnabledExtensions() noexcept;

		~Instance() noexcept;

	private:

		void Destroy() noexcept;

	};
}