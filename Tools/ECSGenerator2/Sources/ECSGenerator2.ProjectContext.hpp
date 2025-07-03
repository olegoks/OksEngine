#pragma once

#include <string>
#include <memory>

#include <ECSGenerator2.ParsedECSFile.hpp>
#include <OksEngine.Config.hpp>

namespace ECSGenerator2 {

	class ProjectContext {
	public:

		inline static const std::string includeDirectory_ = "Sources";

		struct CreateInfo {
			std::shared_ptr<OksEngine::ConfigFile> config = nullptr;
			std::vector<std::filesystem::path> workDirs_;
		};

		ProjectContext(const CreateInfo& createInfo)
			:
			config_{ createInfo.config },
			ci_{ createInfo } {

		}

		void AddEcsFile(std::shared_ptr<ParsedECSFile> ecsFile) {
			ecsFiles_.push_back(ecsFile);
		}

		auto GetConfig() {
			return config_;
		}

		//using ProcessComponentEcsFile = std::function<bool(std::shared_ptr<ParsedECSFile>, bool isLast)>;
		using ProcessSystemEcsFile = std::function<bool(std::shared_ptr<ParsedECSFile>)>;

		//void ForEachComponentEcsFile(ProcessComponentEcsFile&& processComponentEcsFile) {
		//	for (Common::Index i = 0; i < allComponentsSystems_.components_.size(); i++) {
		//		const bool stop = !processComponentEcsFile(
		//			allComponentsSystems_.components_[i],
		//			(allComponentsSystems_.components_.size() - 1 == i) ? (true) : (false));
		//		if (stop) {
		//			break;
		//		}
		//	}
		//}

		using ProcessECSFile = std::function<bool(std::shared_ptr<ParsedECSFile>, bool)>;
		void ForEachECSFile(ProcessECSFile&& processEcsFile) {
			for (Common::Index i = 0; i < ecsFiles_.size(); i++) {
				auto ecsFile = ecsFiles_[i];
				const bool stop = !processEcsFile(ecsFile, i == ecsFiles_.size() - 1);
				if (stop) {
					break;
				}
			}
		}

//		std::shared_ptr<ParsedECSFile> GetEcsFileByName(const std::string& name) {
//
//#pragma region Assert
//			//OS::AssertMessage(nameEcsFile_.contains(name), "");
//#pragma endregion
//
//			return nameEcsFile_[name];
//		}

		std::shared_ptr<OksEngine::ConfigFile> config_ = nullptr;
		CreateInfo ci_;

		std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles_;
	};


}