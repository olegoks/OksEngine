#pragma once

#include <memory>
#include <string>

#include <RAL.Driver.hpp>
namespace RAL {

	class API {
	public:
		virtual std::shared_ptr<RAL::Driver::Shader> CreateShader(const RAL::Driver::Shader::CreateInfo& createInfo) const = 0;
		virtual std::shared_ptr<RAL::Driver> CreateDriver(const Driver::CreateInfo& createInfo) const = 0;

		virtual ~API() = default;
	};

	[[nodiscard]]
	std::shared_ptr<API> CreateAPI();
}