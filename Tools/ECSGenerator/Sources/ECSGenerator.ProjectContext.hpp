#pragma once

#include <ECSGenerator.Component.hpp>
#include <ECSGenerator.System.hpp>

namespace ECSGenerator {



	class ProjectContext {
	public:

		inline static const std::string includeDirectory_ = "Sources";

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

		using ProcessComponentEcsFile = std::function<void(std::shared_ptr<ParsedComponentECSFile>)>;
		using ProcessSystemEcsFile = std::function<void(std::shared_ptr<ParsedSystemECSFile>)>;

		void ForEachComponentEcsFile(ProcessComponentEcsFile&& processComponentEcsFile) {
			for (auto componentEcsFile : allComponentsSystems_.components_) {
				processComponentEcsFile(std::dynamic_pointer_cast<ParsedComponentECSFile>(componentEcsFile));
			}
		}

		void ForEachSystemEcsFile(ProcessSystemEcsFile&& processSystemEcsFile) {
			for (auto systemEcsFile : allComponentsSystems_.systems_) {
				processSystemEcsFile(std::dynamic_pointer_cast<ParsedSystemECSFile>(systemEcsFile));
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

		ComponentsSystems allComponentsSystems_;
	};

    
}