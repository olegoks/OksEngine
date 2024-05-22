#pragma once

#include <RAL.API.hpp>
#include <Vulkan/Render.Vulkan.API.hpp>

namespace RAL {
	
	std::shared_ptr<RAL::API> CreateAPI() {
		return std::make_shared<Render::Vulkan::API>();
	}

}
