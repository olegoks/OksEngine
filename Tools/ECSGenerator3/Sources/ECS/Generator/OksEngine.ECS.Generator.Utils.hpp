#include <ECS/Generator/auto_OksEngine.ECS.Generator.hpp>
#include <ECS/File/OksEngine.ECS.File.Utils.hpp>

namespace ECS::Generator {

#define ECS__FILE__TABLE__GET_TABLE_PARENTS(entityId)\
			[this](ECS2::Entity::Id tableEntityId)->std::vector<ECS2::Entity::Id> {\
				ASSERT(IsComponentExist<File::Table::Tag>(tableEntityId));\
				if (!IsComponentExist<File::Table::ParentTableEntityId>(tableEntityId)) {\
					return {};\
				}\
				else {\
					ECS2::Entity::Id parentTableEntityId = GetComponent<File::Table::ParentTableEntityId>(tableEntityId)->id_;\
					std::vector<ECS2::Entity::Id> parentTables;\
					std::function<void(ECS2::Entity::Id)> processParent =\
						[this, &parentTables, &processParent]\
						(ECS2::Entity::Id parentTableEntityId) {\
						if (IsComponentExist<File::Table::ParentTableEntityId>(parentTableEntityId)) {\
							processParent(GetComponent<File::Table::ParentTableEntityId>(parentTableEntityId)->id_);\
						}\
						parentTables.push_back(parentTableEntityId);\
						};\
					processParent(parentTableEntityId);\
					return parentTables;\
				}\
			}(entityId)


// auto_{project_name}.Project
#define ECS__GENERATOR__GET_PROJECT_INCLUDE_FILE_NAME(project)\
	[this](ECS2::Entity::Id projectEntityId){\
	return "auto_" + \
		std::filesystem::path{ GetComponent<OksEngine::ECS::Project::Path>(projectEntityId)->path_ }.stem().string() + ".Project";\
	}(project)


#define ECS__GENERATOR__GET_PROJECT_INCLUDE_FILE_INCLUDE_PATH(project)\
	[this](ECS2::Entity::Id projectEntityId){\
	std::string fileName = ECS__GENERATOR__GET_PROJECT_INCLUDE_FILE_NAME(projectEntityId);\
	return std::filesystem::path{fileName};\
	}(project)


// auto_{module_name}.Module
#define ECS__GENERATOR__GET_MODULE_INCLUDE_FILE_NAME(moduleId)\
	[this](ECS2::Entity::Id moduleEntityId){\
	return "auto_" + \
		std::filesystem::path{ GetComponent<OksEngine::ECS::Module::Path>(moduleEntityId)->path_ }.stem().string() + ".Module";\
	}(moduleId)

#define ECS__GENERATOR__GET_MODULE_INCLUDE_FILE_INCLUDE_PATH(project, moduleId)\
	[this](ECS2::Entity::Id projectEntityId, ECS2::Entity::Id moduleEntityId){\
	const std::filesystem::path modulePath = GetComponent<OksEngine::ECS::Module::Path>(moduleEntityId)->path_;\
	std::filesystem::path moduleIncludePath = GetComponent<OksEngine::ECS::Module::IncludePath>(moduleEntityId)->path_;\
	std::filesystem::path moduleRelativePath = modulePath.parent_path().lexically_relative(std::filesystem::path{ GetComponent<OksEngine::ECS::Project::Path>(projectEntityId)->path_ }.parent_path());\
	std::filesystem::path fileIncludePath = moduleRelativePath.lexically_relative(moduleIncludePath.relative_path());\
	return fileIncludePath;\
	}(project, moduleId)



}