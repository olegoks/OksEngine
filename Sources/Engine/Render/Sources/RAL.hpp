#pragma once

#include <memory>
#include <vector>
#include <string>

export module OksEngine.RAL;

export import OksEngine.RAL.Driver;
export import OksEngine.RAL.API;

namespace RAL {

	enum class RenderAPI {
		Vulkan
	};

	inline constexpr RenderAPI currentRenderAPI = RenderAPI::Vulkan;

}