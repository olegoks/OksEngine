#pragma once

#include <string>
#include <vector>

#include <Common.Identifier.hpp>
#include <RAL.Common.hpp>
#include <Math.Common.hpp>

namespace RAL {

	class Texture {
	public:

		using Id = Common::Id;

		struct CreateInfo {
			std::string name_;
			std::vector<Color4b> pixels_;
			glm::u32vec2 size_{ 0, 0 };
			Common::UInt32 mipLevels_ = 1;
		};

		Texture(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } {
			
		}

		[[nodiscard]]
		const std::vector<Color4b>& GetPixels() const noexcept {
			return createInfo_.pixels_;
		}

		[[nodiscard]]
		const glm::u32vec2& GetSize() const noexcept {
			return createInfo_.size_;
		}

		[[nodiscard]]
		const std::string& GetName() const noexcept {
			return createInfo_.name_;
		}

	private:
		CreateInfo createInfo_;
	};

}
