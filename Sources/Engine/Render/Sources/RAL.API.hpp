#pragma once

#include <memory>
#include <string>

#include <RAL.Driver.hpp>
#include <RAL.Light.hpp>

namespace RAL {

	class API {
	public:
		virtual std::shared_ptr<Driver> CreateDriver(const Driver::CreateInfo& createInfo) const = 0;
		virtual std::shared_ptr<Camera> CreateCamera(const Camera::CreateInfo& createInfo) const {
		
			return std::make_shared<Camera>(createInfo);
		
		};

		virtual std::shared_ptr<Light> CreateLight(const Light::CreateInfo& createInfo) const {
			return std::make_shared<Light>(createInfo);
		}

		virtual ~API() = default;
	};

	[[nodiscard]]
	std::shared_ptr<API> CreateAPIObject();
}