#pragma once

#include <memory>

module OksEngine.RAL;
import OksEngine.Render.Vulkan.API;

namespace RAL {

	std::shared_ptr<API> CreateAPIObject() {
		return std::make_shared<Render::Vulkan::API>();
	}

}
