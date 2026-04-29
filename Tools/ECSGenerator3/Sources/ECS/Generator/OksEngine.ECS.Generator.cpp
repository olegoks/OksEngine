#pragma once
#include <ECS\Generator\auto_OksEngine.ECS.Generator.hpp>
#include <Resources/OksEngine.ResourceSystem.Utils.hpp>
#include <CPP/OksEngine.CPP.Tree.Utils.hpp>

#include <OS.FileSystem.TextFile.hpp>
#include <format>
#include <memory>

namespace OksEngine
{
	namespace ECS
	{
		namespace Generator
		{

#define GET_TABLE_NAMESPACE(entityId, delimiterString, isNamespaceUppercase)\
		[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
			ECS2::ComponentsFilter beginTableCF = GetComponentsFilter(tableEntityId);\
			std::vector<std::string> namespaceStrings;\
			std::function<void(ECS2::Entity::Id)> processParent = \
				[this,\
				&namespaceStrings,\
				&processParent](ECS2::Entity::Id parentEntityId) {\
				ECS2::ComponentsFilter parentTableCF = GetComponentsFilter(parentEntityId);\
				ASSERT(IsComponentExist<File::Table::Tag>(parentEntityId));\
				ASSERT(IsComponentExist<File::Table::Namespace::Tag>(parentEntityId));\
				ASSERT(IsComponentExist<File::Table::Name>(parentEntityId));\
				ASSERT(IsComponentExist<File::Table::ChildTablesEntityIds>(parentEntityId));\
				if (IsComponentExist<File::Table::ParentTableEntityId>(parentEntityId)) {\
					processParent(GetComponent<File::Table::ParentTableEntityId>(parentEntityId)->id_);\
				}\
				std::string tableName = GetComponent<File::Table::Name>(parentEntityId)->name_;\
				namespaceStrings.push_back(tableName);\
				};\
			if (!beginTableCF.IsSet<File::Table::ParentTableEntityId>()) {\
				return std::string{ "" };\
			}\
			else {\
				ECS2::Entity::Id parentTableEntityId = GetComponent<File::Table::ParentTableEntityId>(tableEntityId)->id_;\
				processParent(parentTableEntityId);\
				ASSERT(!namespaceStrings.empty());\
				std::string resultNamespace;\
				for (Common::Index i = 0; i < namespaceStrings.size(); i++) {\
					std::string& string = namespaceStrings[i];\
					if (isUppercase) {\
						/*Change values in namespaceStrings, we will not use it later.*/\
						std::transform(string.begin(), string.end(), string.begin(),\
							[](unsigned char c) { return std::toupper(c); });\
					}\
					resultNamespace += string;\
					if (i != namespaceStrings.size() - 1) {\
						resultNamespace += delimiter;\
					}\
				}\
				return resultNamespace;\
			}\
			}(entityId,delimiterString,isNamespaceUppercase)


#define GET_TABLE_FULL_NAME(entityId, delimiterString, isNamespaceUppercase)\
			[this](ECS2::Entity::Id tableEntityId, const char* delimiter, bool isUppercase) {\
				ASSERT(IsComponentExist<File::Table::Tag>(tableEntityId));\
				ASSERT(IsComponentExist<File::Table::Name>(tableEntityId));\
				std::string tableName = GetComponent<File::Table::Name>(tableEntityId)->name_;\
				if (isUppercase) {\
					std::transform(tableName.begin(), tableName.end(), tableName.begin(),\
						[](unsigned char c) { return std::toupper(c); });\
				}\
				const std::string tableNamespace = GET_TABLE_NAMESPACE(tableEntityId, delimiter, isUppercase);\
				if (tableNamespace != "") {\
					return tableNamespace + delimiter + tableName;\
				}\
				else {\
					return tableName;\
				}\
			}(entityId, delimiterString, isNamespaceUppercase)

			//.ecs file to .hpp file.
			void GenerateTrees::Update(
				ECS2::Entity::Id entity0id, 
				const OksEngine::CommandLineParameters* commandLineParameters0,
				const OksEngine::ECS::ProjectFilePath* projectFilePath0) {


				const auto projectFilePath = std::filesystem::path{ projectFilePath0->path_ };

				std::vector<Common::Byte> data = Resource::LoadFileAndGetContent(projectFilePath.string());

				::Lua::Context context;
				::Lua::Script script{ std::string{ data.data(), data.size() } };
				context.LoadScript(script);

				luabridge::LuaRef ecsTable = luabridge::getGlobal(context.state_, "ECS");

				ASSERT_FMSG(ecsTable.isTable() || ecsTable["Modules"].isTable(), "");

				luabridge::LuaRef modulesTable = ecsTable["Modules"];


				//std::vector<ECS2::Entity::Id> fileStructs;
				//std::vector<ECS2::Entity::Id> fileConstants;
				//std::vector<ECS2::Entity::Id> fileComponents;
				//std::vector<ECS2::Entity::Id> fileSystems;
				//std::vector<ECS2::Entity::Id> fileEnums;
				//std::vector<ECS2::Entity::Id> fileArchetypes;

				//std::function<void(ECS2::Entity::Id)> generateTableCode = [&](ECS2::Entity::Id entityId) {


				//	const ECS2::ComponentsFilter tableComponentsFilter = GetComponentsFilter(entityId);

				//	//Table is namespace.
				//	if (tableComponentsFilter.IsSet<File::Table::Namespace::Tag>()) {
				//		for (ECS2::Entity::Id childTableEntityId : GetComponent<File::Table::ChildTablesEntityIds>(entityId)->entityIds_) {
				//			generateTableCode(childTableEntityId);
				//		}
				//	}
				//	else if (tableComponentsFilter.IsSet<File::Table::System::Tag>()) {
				//		fileSystems.push_back(entityId);
				//	}
				//	else if (tableComponentsFilter.IsSet<File::Table::Component::Tag>()) {
				//		fileComponents.push_back(entityId);
				//	}
				//	else if (tableComponentsFilter.IsSet<File::Table::Struct::Tag>()) {
				//		fileStructs.push_back(entityId);
				//	}
				//	else if (tableComponentsFilter.IsSet<File::Table::Constant::Tag>()) {
				//		fileConstants.push_back(entityId);
				//	}
				//	else if (tableComponentsFilter.IsSet<File::Table::Archetype::Tag>()) {
				//		fileArchetypes.push_back(entityId);
				//	}
				//	else if (tableComponentsFilter.IsSet<File::Table::Enum::Tag>()) {
				//		fileEnums.push_back(entityId);
				//	}
				//	};

				//for (ECS2::Entity::Id tableEntityId : file__Table__EntityIds0->entityIds_) {
				//	generateTableCode(tableEntityId);
				//}


				//std::vector<std::string> componentFullNames;
				//for (ECS2::Entity::Id componentEId : fileComponents) {
				//	componentFullNames.push_back(GET_TABLE_FULL_NAME(componentEId, "__", true));
				//}

				const auto allFiles = GetComponents<ECS__FILE__FILE>();
				const Common::Size allFilesNumber = world_->GetEntitiesNumber<ECS__FILE__FILE>();

				for (Common::Index i = 0; i < allFilesNumber; i++) {

					ECS2::Entity::Id cppFileEntityId = CreateEntity<CPP__FILE__FILE>();

					CreateComponent<CPP::File::Tag>(cppFileEntityId);
					CreateComponent<CPP::File::Type::Hpp>(cppFileEntityId);

					std::vector<ECS2::Entity::Id> includes;
					includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("ECS2.hpp"));
					includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("chrono"));
					includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("tuple"));
					includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("OksEngine.IComponent.hpp"));
					includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("Common.hpp"));
					includes.push_back(CPP__FILE__CREATE_INCLUDE_ENTITY("Common.Bitset.hpp"));

					CreateComponent<CPP::File::Include::EntityIds>(cppFileEntityId, includes);

					const auto* path = std::get<ECS::File::Path*>(allFiles)+ i;

					CreateComponent<CPP::File::Path>(cppFileEntityId, std::filesystem::path{ path->path_ }.parent_path().string());
					CreateComponent<CPP::File::Name>(cppFileEntityId, std::filesystem::path{ path->path_ }.stem().string());
				}



				const auto allComponents = GetComponents<ECS__FILE__TABLE__COMPONENT__COMPONENT>();
				const Common::Size allComponentsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__COMPONENT__COMPONENT>();

				const auto allSystems = GetComponents<ECS__FILE__TABLE__SYSTEM__SYSTEM>();
				const Common::Size allSystemsNumber = world_->GetEntitiesNumber<ECS__FILE__TABLE__SYSTEM__SYSTEM>();



			};

			void GenerateCode::Update(
				ECS2::Entity::Id entity0id, 
				const OksEngine::CommandLineParameters* commandLineParameters0,
				const OksEngine::ECS::ProjectFilePath* projectFilePath0, 
				
				ECS2::Entity::Id entity1id,
				const OksEngine::CPP::File::Tag* cPP__File__Tag1) {

				const ECS2::Entity::Id& fileEntityId = entity1id;

				std::string code;

				const ECS2::ComponentsFilter cppfileCF = GetComponentsFilter(fileEntityId);
				
				if (cppfileCF.IsSet<CPP::File::Type::Hpp>()) {
					code += "#pragma once\n";
				}

				if (cppfileCF.IsSet<CPP::File::Include::EntityIds>()) {

					const auto* includesEntityIds = GetComponent<CPP::File::Include::EntityIds>(fileEntityId);

					for (ECS2::Entity::Id includeEntityId : includesEntityIds->ids_) {

						ASSERT(IsComponentExist<CPP::File::Include::Tag>(includeEntityId));
						ASSERT(IsComponentExist<CPP::File::Include::Path>(includeEntityId));

						const std::string includePath = GetComponent<CPP::File::Include::Path>(includeEntityId)->path_;

						code += std::vformat("#include<{}>\n", std::make_format_args(includePath));
					}

				}

				const auto* path = GetComponent<CPP::File::Path>(fileEntityId);
				const auto* name = GetComponent<CPP::File::Name>(fileEntityId);
				
				auto osFile = std::make_shared<OS::TextFile>(path->path_ + "/auto_" + name->name_ + std::string{ (cppfileCF.IsSet<CPP::File::Type::Hpp>()) ? (".hpp") : (".cpp") });
				osFile->Create();
				*osFile << code;


			}

		}

	} // namespace ECS

} // namespace OksEngine