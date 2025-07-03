#pragma once

#include <System/ECSGenerator2.ParsedSystem.hpp>
#include <Component/ECSGenerator2.ParsedComponent.hpp>

namespace ECSGenerator2 {


	class ParsedECSFile {
	public:

		struct CreateInfo {
			std::filesystem::path path_;
			std::vector<std::shared_ptr<ParsedSystem>> systems_;
			std::vector<std::shared_ptr<ParsedComponent>> components_;
		};

		ParsedECSFile(const CreateInfo& ci) :
			ci_{ ci } {  }

		[[nodiscard]]
		inline const std::filesystem::path GetPath() const noexcept {
			return ci_.path_;
		}

		[[nodiscard]]
		std::string GetName() const noexcept {

			return ci_.path_.filename().stem().string().substr();
		}

	private:

		CreateInfo ci_;
	};

}