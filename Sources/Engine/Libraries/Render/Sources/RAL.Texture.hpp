#pragma once

#include <string>
#include <vector>

#include <RAL.Common.hpp>
#include <Math.Common.hpp>

namespace RAL {

	class Texture {
	public:

		struct CreateInfo {
			std::string name_;
			std::vector<Color4b> pixels_;
			glm::u32vec2 size_{ 0, 0 };
		};

		Texture(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {}

	private:
		CreateInfo createInfo_;
	};

}
