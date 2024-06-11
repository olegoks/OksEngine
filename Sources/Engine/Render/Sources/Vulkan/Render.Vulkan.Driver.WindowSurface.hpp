#pragma once

#include <Render.Vulkan.Abstraction.hpp>

#include <RAL.Driver.hpp>

namespace Render::Vulkan {

	class [[nodiscard]] WindowSurface final : public Abstraction<VkSurfaceKHR> {
	public:

		struct CreateInfo {
			std::shared_ptr<class Instance> instance_ = nullptr;
			RAL::RenderSurface renderSurface_;
		};

		WindowSurface(const CreateInfo& createInfo) noexcept;
		~WindowSurface() noexcept { Destroy(); }

	private:

		void Destroy() noexcept;

	private:

		std::shared_ptr<class Instance> instance_ = nullptr;

	};

}
