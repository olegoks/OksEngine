#pragma once

#include <memory>
#include <string>

#include <RAL.Driver.hpp>
#include <RAL.Light.hpp>

namespace RAL {

	class API {
	public:
		virtual std::shared_ptr<RAL::Driver> CreateDriver(const Driver::CreateInfo& createInfo) const = 0;
		virtual std::shared_ptr<RAL::Camera> CreateCamera(const Camera::CreateInfo& createInfo) const {
		
			return std::make_shared<RAL::Camera>(createInfo);
		
		};

		virtual std::shared_ptr<RAL::Light> CreateLight(const RAL::Light::CreateInfo& createInfo) const {
			return std::make_shared<RAL::Light>(createInfo);
		}

		virtual ~API() = default;
	};

	[[nodiscard]]
	std::shared_ptr<API> CreateAPI();
}