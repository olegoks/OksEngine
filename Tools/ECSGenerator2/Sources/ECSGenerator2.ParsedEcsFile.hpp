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

		using ProcessSystem = std::function<void(std::shared_ptr<ParsedSystem>)>;

		void ForEachSystem(ProcessSystem&& processSystem) {
			for (Common::Index i = 0; i < ci_.systems_.size(); i++) {
				std::shared_ptr<ParsedSystem> system = ci_.systems_[i];
				processSystem(system);
			}
		}

		[[nodiscard]]
		const std::filesystem::path GetPath() const noexcept {
			return ci_.path_;
		}

	private:

		CreateInfo ci_;
	};


}