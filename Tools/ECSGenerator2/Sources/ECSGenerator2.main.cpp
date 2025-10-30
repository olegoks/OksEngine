#include <filesystem>

#include <OS.CommandLineParameters.hpp>

#include <Parser/ECSGenerator2.Parser.hpp>

#include <CodeStructureGenerator/ECSGenerator2.CodeStructureGenerator.hpp>

#include <OksEngine.Config.hpp>

#include <Resource.hpp>

int main(int argc, char** argv) {

	OS::CommandLineParameters parameters{ argc, argv };


	OS::InitializeLogger(
		parameters.GetArgc(),
		parameters.GetArgv());

	ASSERT_FMSG(argc > 1, "");
	const std::vector<std::string_view> workDirsArgv = parameters.GetValue("-workDir");
	const std::string_view includeDirArgv = parameters.GetValue("-includeDir")[0];

	std::vector<std::filesystem::path> workDirs{ };

	for (auto& workDir : workDirsArgv) {
		workDirs.push_back(workDir);
	}

	Resources::ResourceSystem::CreateInfo rsci{
		.fileExtensions_ = { ".ecs", ".dot", ".lua" }
	};
	Resources::ResourceSystem resourceSystem{ rsci };
	resourceSystem.SetRoots(workDirs);

	struct ECSFileInfo {
		std::filesystem::path resourceSystemPath_;
		std::filesystem::path filesystemPath_;
	};

	std::vector<ECSFileInfo> ecsFileInfos;
	{
		std::filesystem::path dotSystemPath_;
		const std::filesystem::path root = "Root";
		resourceSystem.ForEachAddedResource([&](const std::filesystem::path resourceData) ->bool {
			const bool ecsFile = resourceData.extension() == ".ecs"; // Systems and components
			const bool dotFile = resourceData.extension() == ".dot"; //Systems graph
			const bool cfgFile = resourceData.extension() == ".cfg";
			if (dotFile) {
				dotSystemPath_ = resourceData;
			}
			if (ecsFile) {
				ecsFileInfos.push_back(ECSFileInfo{
						.resourceSystemPath_ = resourceData,
						.filesystemPath_ = resourceSystem.GetOSResourcePath(resourceData)
					});
			}
			resourceSystem.LoadResource(resourceData);
			return true;
			});

		std::vector<std::string_view> configFilePath = parameters.GetValue("-cfg");
		resourceSystem.AddResource(std::filesystem::path{ configFilePath[0] }.filename().string(), std::filesystem::path{ configFilePath[0] }, "Root");
		resourceSystem.LoadResource("Root" / std::filesystem::path{ configFilePath[0] }.filename());
		OS::Assert(configFilePath.size() == 1);
		Resources::ResourceData configResourceData = resourceSystem.GetResourceData(root / std::filesystem::path(configFilePath[0]).filename());
		auto config = std::make_shared<OksEngine::ConfigFile>(std::string{ configResourceData.GetData<Common::Byte>(), configResourceData.GetSize() });
	}

	//Parse ecs files.
	std::vector<std::shared_ptr<ECSGenerator2::ParsedECSFile>> parsedECSFiles;
	{
		ECSGenerator2::Parser ecsFileParser;

		for (const ECSFileInfo& ecsFileInfo : ecsFileInfos) {
			Resources::ResourceData resourceData = resourceSystem.GetResourceData(ecsFileInfo.resourceSystemPath_);
			const std::string ecsFileText{ resourceData.GetData<Common::Byte>(), resourceData.GetSize() };
			//PARSING ECS FILE.
			auto ecsFile = ecsFileParser.Parse(ecsFileInfo.filesystemPath_, ecsFileText);
			if (ecsFile != nullptr) {
				parsedECSFiles.push_back(ecsFile);
			}

		}
	}

	//Set pointers to systems, components
	//TODO: move to separate function.
	{

		auto getTableByFullName2 = [](
			const std::vector<std::shared_ptr<ECSGenerator2::ParsedECSFile>> parsedECSFiles,
			std::shared_ptr<ECSGenerator2::ParsedTable> usageTable,
			const std::vector<std::string>& findTable, ECSGenerator2::ParsedTable::Type findTableType) {

				// Component name: "Compute::Pipeline"
				// algorithm :
				//1. Find usage namespaces:
				//	Render -> Compute
				//2. For each namespace go from childless to root and attempt to find need component name.
				//	Failure:
				//	"Global Namespace" -> Render ->	Compute 
				//										|
				//										\/
				//									Compute -> Pipeline 
				//
				//
				//	Success:
				//	"Global Namespace" -> Render -> Compute			
				//							|		
				//						    \/
				//						Compute -> Pipeline


				std::shared_ptr<ECSGenerator2::ParsedTable> foundTable = nullptr;
				//From child to root.
				// 
				auto usageNamespace = usageTable->GetNamespace();

				for (Common::Int64 i = usageNamespace.size(); i >= 0; --i) {

					if (foundTable != nullptr) {
						break;
					}

					std::vector<std::string> currentNamespace;
					for (Common::Index j = 0; j < i; j++) {
						currentNamespace.push_back(usageNamespace[j]);
					}

					for (auto findPart : findTable) {
						currentNamespace.push_back(findPart);
					}

					for (auto parsedECSFile : parsedECSFiles) {

						if (foundTable != nullptr) {
							break;
						}

						if (parsedECSFile->GetName() == "OksEngine.Model") {
							Common::BreakPointLine();
						}
						parsedECSFile->ForEachRootTable([&](std::shared_ptr<ECSGenerator2::ParsedTable> parsedTable) {
							

							if (foundTable != nullptr) {
								return false;
							}

							parsedTable->ForEachTablePath([&](std::vector<std::shared_ptr<ECSGenerator2::ParsedTable>>& path) {

								if (currentNamespace.size() == path.size() && path.back()->GetType() == findTableType) {
									for (Common::Index k = 0; k < currentNamespace.size(); k++) {
										if (currentNamespace[k] != path[k]->GetName()) {
											return true;
										}
									}
									ECSGenerator2::ParsedTablePtr table = path.back();
									foundTable = table;
								}
								else {
									return true;
								}

								});

							return true;

							
							return true;
							});

					}
				}

				ASSERT_FMSG(foundTable != nullptr,
					"Can't find required ECS abstraction {}, in {}",
					findTable.back(),
					usageTable->GetFullName());

				return foundTable;

			};

		for (const auto parsedEcsFile : parsedECSFiles) {

			parsedEcsFile->ForEachComponent([&](ECSGenerator2::ParsedComponentPtr component) {
				
				if (!component->ci_.aliasForName_.empty()) {

					const auto componentName = component->ci_.aliasForName_;
					const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

					component->ci_.aliasFor_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(getTableByFullName2(
						parsedECSFiles,
						component,
						parsedComponentName, ECSGenerator2::ParsedTable::Type::Component));

					ASSERT(component->ci_.aliasFor_ != nullptr);
				}

				return true;
			});

			parsedEcsFile->ForEachArchetype([&](ECSGenerator2::ParsedArchetypePtr parsedArchetype) {
				
				const auto archetypeNamespace = ECSGenerator2::GetArchetypeNamespace(parsedArchetype);
				const auto archetypeName = parsedArchetype->GetName();

				if (archetypeName == "Model") {
					Common::BreakPointLine();
				}
				parsedArchetype->ForEachComponent([&](ECSGenerator2::ParsedArchetype::Component& component, bool isLast) {
					
					const auto componentName = component.name_;
					const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

					component.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
						getTableByFullName2(
							parsedECSFiles, 
							parsedArchetype,
							parsedComponentName, ECSGenerator2::ParsedTable::Type::Component));

					ASSERT(component.ptr_ != nullptr);

					});

				parsedArchetype->ForEachRefArchetype([&](ECSGenerator2::ParsedArchetype::ChildArchetype& archetype, bool isLast) {

					const auto archetypeRefName = archetype.name_;
					const auto parsedArchetypeName = ECSGenerator2::ParseFullName(archetypeRefName);

					archetype.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedArchetype>(
						getTableByFullName2(
							parsedECSFiles,
							parsedArchetype,
							parsedArchetypeName, ECSGenerator2::ParsedTable::Type::Archetype));

					ASSERT(archetype.ptr_ != nullptr);

					});

				return true;
				});

			parsedEcsFile->ForEachSystem([&](ECSGenerator2::ParsedSystemPtr parsedSystem) {

				const auto systemNamespace = ECSGenerator2::GetSystemNamespace(parsedSystem);
				const auto systemName = parsedSystem->GetName();

				parsedSystem->ci_.updateMethod_->ForEachProcessEntity(
					[&](ECSGenerator2::ParsedSystem::ProcessedEntity& entity, bool isLast) {

						entity.ForEachInclude([&](ECSGenerator2::ParsedSystem::Include& include, bool isLast) {

							const auto componentName = include.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							if (parsedSystem->GetName() == "CreatePipeline") {
								Common::BreakPointLine();
							}

							if (componentName == "LuaScript") {
								Common::BreakPointLine();
							}

							include.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles,
									parsedSystem, 
									parsedComponentName,
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});

						entity.ForEachExclude([&](ECSGenerator2::ParsedSystem::Exclude& exclude, bool isLast) {

							const auto componentName = exclude.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							exclude.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles, 
									parsedSystem, 
									parsedComponentName,
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});

						entity.ForEachCreate([&](ECSGenerator2::ParsedSystem::Create& create, bool isLast) {

							const auto componentName = create.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							create.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles, 
									parsedSystem,
									parsedComponentName,
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});

						entity.ForEachRemove([&](ECSGenerator2::ParsedSystem::Remove& remove, bool isLast) {

							const auto componentName = remove.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							remove.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles,
									parsedSystem, 
									parsedComponentName, 
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});

						return true;
					});

				if (parsedSystem->GetName() == "CreateLuaScript") {
					Common::BreakPointLine();
				}

				parsedSystem->ci_.updateMethod_->ForEachRandomAccessEntity(
					[&](ECSGenerator2::ParsedSystem::RandomAccessEntity& entity, bool isLast) {

						//Find archetype
						{
							if (entity.archetype_ != nullptr) {
								const auto archetypeName = entity.archetype_->name_;
								const auto parsedArchetypeName = ECSGenerator2::ParseFullName(archetypeName);
								entity.archetype_->ptr_ = Common::pointer_cast<ECSGenerator2::ParsedArchetype>(
									getTableByFullName2(
										parsedECSFiles,
										parsedSystem,
										parsedArchetypeName,
										ECSGenerator2::ParsedTable::Type::Archetype));
							}
							
						}

						entity.ForEachInclude([&](ECSGenerator2::ParsedSystem::Include& include, bool isLast) {

							const auto componentName = include.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							include.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles,
									parsedSystem, 
									parsedComponentName,
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});


						entity.ForEachCreate([&](ECSGenerator2::ParsedSystem::Create& create, bool isLast) {

							const auto componentName = create.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							create.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles,
									parsedSystem, 
									parsedComponentName,
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});

						entity.ForEachRemove([&](ECSGenerator2::ParsedSystem::Remove& remove, bool isLast) {

							const auto componentName = remove.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							remove.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles, 
									parsedSystem,
									parsedComponentName,
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});

						return true;
					});

				parsedSystem->ci_.updateMethod_->ForEachCreateEntity(
					[&](ECSGenerator2::ParsedSystem::CreatesEntity& entity, bool isLast) {

						entity.ForEachCreate([&](ECSGenerator2::ParsedSystem::Create& create, bool isLast) {

							const auto componentName = create.GetName();

							//at first lets find run after system in namespace of current system.
							const auto parsedComponentName = ECSGenerator2::ParseFullName(componentName);

							create.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedComponent>(
								getTableByFullName2(
									parsedECSFiles, 
									parsedSystem,
									parsedComponentName,
									ECSGenerator2::ParsedTable::Type::Component));

							return true;
							});

						return true;
					});

				if (parsedSystem->ci_.callOrderInfo_ != nullptr) {

					for (auto& runAfterSystem : parsedSystem->ci_.callOrderInfo_->runAfter_) {
						//at first lets find run after system in namespace of current system.
						const auto runAfterName = ECSGenerator2::ParseFullName(runAfterSystem.name_);

						if (runAfterSystem.name_ == "SendWindowKeyboardEvents") {
							Common::BreakPointLine();
						}

						runAfterSystem.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedSystem>(
							getTableByFullName2(
								parsedECSFiles,
								parsedSystem, 
								runAfterName,
								ECSGenerator2::ParsedTable::Type::System));

					}

					for (auto& runBeforeSystem : parsedSystem->ci_.callOrderInfo_->runBefore_) {
						//at first lets find run after system in namespace of current system.
						const auto runBeforeName = ECSGenerator2::ParseFullName(runBeforeSystem.name_);

						if (runBeforeSystem.name_ == "SendWindowKeyboardEvents") {
							Common::BreakPointLine();
						}

						runBeforeSystem.ptr_ = Common::pointer_cast<ECSGenerator2::ParsedSystem>(
							getTableByFullName2(
								parsedECSFiles,
								parsedSystem,
								runBeforeName,
								ECSGenerator2::ParsedTable::Type::System));

					}

				}

				return true;
				});
		}
	}

	//GENERATE CODE STRUCTURE.

	ECSGenerator2::CodeStructureGenerator::CreateInfo csgci{
		.ecsFiles_ = parsedECSFiles
	};

	ECSGenerator2::CodeStructureGenerator codeStructureGenerator{ csgci };


	//Generate code structure for ecs files.
	std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::CodeStructure::File>> structureFiles;
	{
		for (auto parsedECSFile : parsedECSFiles) {

			if (parsedECSFile->GetName() == "OksEngine.Behaviour") {
				Common::BreakPointLine();
			}

			//Generate system hpp file.
			//Always generate .hpp files.
			auto systemHppFile = codeStructureGenerator.GenerateECSFileHppFile(includeDirArgv, parsedECSFile);
			const auto ecsHppPath = parsedECSFile->GetPath().parent_path() / ("auto_" + parsedECSFile->GetPath().filename().stem().string() + ".hpp");
			structureFiles[ecsHppPath] = systemHppFile;

			//Generate system cpp file.
			//Generate .cpp files if they doesn't exist.
			const auto ecsCppPath = parsedECSFile->GetPath().parent_path() / (parsedECSFile->GetPath().filename().stem().string() + ".cpp");

			const bool isFileExist = resourceSystem.IsFileExist(ecsCppPath);
			if (!isFileExist) {
				const bool isContainsSystem = parsedECSFile->IsContainsSystems();
				bool needToRealizeHelpFunctions = false;

				parsedECSFile->ForEachComponent([&](ECSGenerator2::ParsedComponentPtr parsedComponent) {

					needToRealizeHelpFunctions = parsedComponent->IsNeedToImplementHelpFunction();
					if (needToRealizeHelpFunctions) {
						return false;
					}
					return true;
					});
				if (isContainsSystem || needToRealizeHelpFunctions) {
					auto systemCppFile = codeStructureGenerator.GenerateECSFileCppFile(includeDirArgv, parsedECSFile);
					structureFiles[ecsCppPath] = systemCppFile;
				}
			}

		}
	}

	//Generate code structure for auto_OksEngine.ECS.hpp files
	std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::CodeStructure::File>> hppECSFiles;
	{
		hppECSFiles = codeStructureGenerator.GenerateDirECSIncludeHppFiles(includeDirArgv, includeDirArgv);
	}

	//Generate Parse entity code structure.
	auto parseEntityCodeStructure = codeStructureGenerator.GenerateParseEntityHppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::CodeStructure::File>> pathToParsedEntity{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.ParseEntity.hpp",
		parseEntityCodeStructure };

	auto serializeEntityCodeStructure = codeStructureGenerator.GenerateSerializeEntityHppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::CodeStructure::File>> pathToSerializeEntity{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.SerializeEntity.hpp",
		serializeEntityCodeStructure };

	auto editEntityHppCodeStructure = codeStructureGenerator.GenerateEditEntityHppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::CodeStructure::File>> pathToEditHppEntity{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.EditEntity.hpp",
		editEntityHppCodeStructure };

	auto editEntityCppCodeStructure = codeStructureGenerator.GenerateEditEntityCppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::CodeStructure::File>> pathToEditCppEntity{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.EditEntity.cpp",
		editEntityCppCodeStructure };


	auto hppRunSystemsCodeStructure = codeStructureGenerator.GenerateRunSystemsHppFile();
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::CodeStructure::File>> pathToHppRunSystems{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.RunSystems.hpp",
		hppRunSystemsCodeStructure };

	auto cppRunSystemsCodeStructure = codeStructureGenerator.GenerateRunSystemsCppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::CodeStructure::File>> pathToCppRunSystems{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.RunSystems.cpp",
		cppRunSystemsCodeStructure };

	auto hppUtilsCodeStructure = codeStructureGenerator.GenerateUtilsHppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::CodeStructure::File>> pathToHppUtils{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.Utils.hpp",
		hppUtilsCodeStructure };

	//Merge files with  code structure.
	std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::CodeStructure::File>> allFilesCodeStructure;

	allFilesCodeStructure.insert(structureFiles.begin(), structureFiles.end());
	allFilesCodeStructure.insert(hppECSFiles.begin(), hppECSFiles.end());

	allFilesCodeStructure.insert(pathToParsedEntity);
	allFilesCodeStructure.insert(pathToSerializeEntity);
	allFilesCodeStructure.insert(pathToEditHppEntity);
	allFilesCodeStructure.insert(pathToEditCppEntity);
	allFilesCodeStructure.insert(pathToHppRunSystems);
	allFilesCodeStructure.insert(pathToCppRunSystems);
	allFilesCodeStructure.insert(pathToHppUtils);


	//Create files.
	for (auto [path, structureFile] : allFilesCodeStructure) {
		ECSGenerator2::CodeGenerator codeGenerator;
		auto codeFile = codeGenerator.GenerateCode(structureFile);
		auto osFile = std::make_shared<OS::TextFile>(path/*.parent_path() / ("auto_OksEngine." + path.filename().stem().string() + ".hpp")*/);
		osFile->Create();
		*osFile << codeFile->code_.code_;
	}

	return 0;
}

