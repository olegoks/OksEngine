#pragma once

#include <memory>
#include <any>

#include <vulkan/vulkan.h>

#include <Render.Vulkan.Driver.Instance.hpp>

#include <RAL.Driver.hpp>

namespace Render::Vulkan {

	class [[nodiscard]] WindowSurface final : public Abstraction<VkSurfaceKHR> {
	public:

		struct CreateInfo {
			std::shared_ptr<Instance> instance_ = nullptr;
			RAL::RenderSurface renderSurface_;
		};

		WindowSurface(const CreateInfo& createInfo) noexcept;

		~WindowSurface() noexcept {

			Destroy();
			
		}

	private:

		void Destroy() noexcept;

	private:

		std::shared_ptr<Instance> instance_ = nullptr;

	};

}
