#pragma once

#include <string>
#include <memory>

#include <ECSGenerator.ParsedECSFile.hpp>
#include <OksEngine.Config.hpp>

namespace ECSGenerator {



	class ProjectContext {
	public:

		inline static const std::string includeDirectory_ = "Sources";

		struct CreateInfo {
			std::shared_ptr<OksEngine::Config> config = nullptr;
		};

		ProjectContext(const CreateInfo& createInfo) 
			: config_{ createInfo.config } {

		}

		void AddEcsFile(std::shared_ptr<ParsedECSFile> ecsFile) {

			nameEcsFile_[ecsFile->GetName()] = ecsFile;

			const auto parentPath = ecsFile->GetPath().parent_path();

			if (!oneCategoryEcsFiles_.contains(parentPath)) {
				oneCategoryEcsFiles_[parentPath] = ComponentsSystems{};
			}

			if (ecsFile->GetType() == ParsedECSFile::Type::Component) {
				allComponentsSystems_.components_.push_back(ecsFile);

				oneCategoryEcsFiles_[parentPath].components_.push_back(ecsFile);
				
				return;
			}
			else if (ecsFile->GetType() == ParsedECSFile::Type::System) {
				allComponentsSystems_.systems_.push_back(ecsFile);
				oneCategoryEcsFiles_[parentPath].systems_.push_back(ecsFile);
				return;
			}
			OS::NotImplemented();

		}

		auto GetConfig() {
			return config_;
		}

		using ProcessComponentEcsFile = std::function<bool(std::shared_ptr<ParsedECSFile>)>;
		using ProcessSystemEcsFile = std::function<bool(std::shared_ptr<ParsedECSFile>)>;

		void ForEachComponentEcsFile(ProcessComponentEcsFile&& processComponentEcsFile) {
			for (auto componentEcsFile : allComponentsSystems_.components_) {
				const bool stop = !processComponentEcsFile(componentEcsFile);
				if (stop) {
					break;
				}
			}
		}

		void ForEachSystemEcsFile(ProcessSystemEcsFile&& processSystemEcsFile) {
			for (auto systemEcsFile : allComponentsSystems_.systems_) {
				const bool stop = !processSystemEcsFile(systemEcsFile);
				if (stop) {
					break;
				}
			}			
		}
		std::shared_ptr<ParsedECSFile> GetEcsFileByName(const std::string& name) {

#pragma region Assert
			OS::AssertMessage(nameEcsFile_.contains(name), "");
#pragma endregion

			return nameEcsFile_[name];
		}

		struct ComponentsSystems {
			std::vector<std::shared_ptr<ParsedECSFile>> components_;
			std::vector<std::shared_ptr<ParsedECSFile>> systems_;
		};

		std::map<std::string, std::shared_ptr<ParsedECSFile>> nameEcsFile_;

		std::map<std::filesystem::path, ComponentsSystems> oneCategoryEcsFiles_;
		std::shared_ptr<OksEngine::Config> config_ = nullptr;
		ComponentsSystems allComponentsSystems_;
	};

    
}