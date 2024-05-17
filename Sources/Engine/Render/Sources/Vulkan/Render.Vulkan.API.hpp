#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.hpp>
#include <Render.Vulkan.Driver.hpp>

namespace Render::Vulkan {

	class API {
	public:

		std::shared_ptr<Driver> CreateDriver(const Driver::CreateInfo& createInfo) const override {
	
			Driver::CreateInfo debugCreateInfo = createInfo;
			return std::make_shared<Vulkan::Driver>(debugCreateInfo);

		}

	};

}