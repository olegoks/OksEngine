#pragma once

#include <memory>
#include <vector>
#include <string>

#include <RAL.Driver.hpp>
#include <RAL.API.hpp>

namespace RAL {

	enum class RenderAPI {
		Vulkan
	};

	inline constexpr RenderAPI currentRenderAPI = RenderAPI::Vulkan;

}