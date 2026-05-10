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
}