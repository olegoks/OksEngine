#pragma once

#include <memory>

#include <RAL.API.hpp>
#include <Vulkan/Render.Vulkan.API.hpp>

namespace RAL {

	std::shared_ptr<RAL::API> CreateAPIObject() {
		return std::make_shared<Render::Vulkan::API>();
	}

}
