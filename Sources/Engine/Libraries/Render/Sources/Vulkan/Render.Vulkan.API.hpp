#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>

#include <Common.hpp>
#include <RAL.hpp>
#include <Render.Vulkan.Driver.hpp>

namespace Render::Vulkan {

	class API : public RAL::API {
	public:

		[[nodiscard]]
		virtual std::shared_ptr<RAL::Driver> CreateDriver(const RAL::Driver::CreateInfo& createInfo) const override {
	
			Driver::CreateInfo debugCreateInfo = createInfo;
			return std::make_shared<Vulkan::Driver>(debugCreateInfo);

		}

		[[nodiscard]]
		virtual std::shared_ptr<RAL::Shader> CreateShader(const RAL::Shader::CreateInfo& createInfo) const override {

			Shader::CreateInfo vulkanShaderCreateInfo{
				.ralCreateInfo_ = createInfo
			};

			return std::make_shared<Shader>(vulkanShaderCreateInfo);
		}



	};

}