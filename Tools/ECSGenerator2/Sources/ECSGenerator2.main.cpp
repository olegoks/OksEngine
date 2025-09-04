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

	OS::AssertMessage(argc > 1, "");
	const std::vector<std::string_view> workDirsArgv = parameters.GetValue("-workDir");
	const std::string_view includeDirArgv = parameters.GetValue("-includeDir")[0];

	std::vector<std::filesystem::path> workDirs{ };

	for (auto& workDir : workDirsArgv) {
		workDirs.push_back(workDir);
	}

	Resources::ResourceSystem::CreateInfo rsci{
		.fileExtensions_ = { ".ecs", ".dot", ".lua", ".gltf"}
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

			if (ecsFileInfo.filesystemPath_.stem() == "OksEngine.Behaviour") {
				Common::BreakPointLine();
			}
			//PARSING ECS FILE.
			auto ecsFile = ecsFileParser.Parse(ecsFileInfo.filesystemPath_, ecsFileText);
			if (ecsFile != nullptr) {
				parsedECSFiles.push_back(ecsFile);
			}

		}
	}



	//Set pointers to systems, components
	{
		//auto findSystem = [](ECSGenerator2::ParsedSystemPtr parsedSystem, const std::string& systemFullName) {

		//	const auto systemParsedFullName = ECSGenerator2::ParseFullName(systemFullName);

		//	std::vector<std::string> systemNamespace;
		//	if (systemParsedFullName.size() > 1) {
		//		for (Common::Index i = 0; i < systemParsedFullName.size() - 1; i++) {
		//			systemNamespace.push_back(systemParsedFullName[i]);
		//		}
		//	}
		//	const std::string systemName = systemParsedFullName.back();
		//	//Find namespace
		//	if (!systemNamespace.empty()) {
		//		std::function<void(ECSGenerator2::ParsedTablePtr)> processParent = [&](ECSGenerator2::ParsedTablePtr parsedTable) {


		//			if (parsedTable->GetName() == systemNamespace[0]) {
		//				//Found namespace 

		//			}

		//			if (parsedTable->parentTable_ != nullptr) {
		//				processParent(parsedTable->parentTable_);
		//			}
		//	};


		//		if (parsedSystem->parentTable_ != nullptr) {
		//			processParent(parsedSystem->parentTable_);
		//		}
		//	};

		//};

		auto mergeArraysPreserveOrder = [](const std::vector<std::string>& arr1,
			const std::vector<std::string>& arr2) {
				std::vector<std::string> result;
				std::unordered_set<std::string> seen;

				// Сначала добавляем все уникальные элементы из первого массива
				for (const auto& str : arr1) {
					if (seen.find(str) == seen.end()) {
						result.push_back(str);
						seen.insert(str);
					}
				}

				// Затем добавляем уникальные элементы из второго массива
				for (const auto& str : arr2) {
					if (seen.find(str) == seen.end()) {
						result.push_back(str);
						seen.insert(str);
					}
				}

				return result;
			};

		auto getTableByFullName = [](
			const std::vector<std::shared_ptr<ECSGenerator2::ParsedECSFile>> parsedECSFiles,
			const std::vector<std::string>& arr1) {

				for (const auto parsedEcsFile : parsedECSFiles) {
					if (parsedEcsFile->GetName() == "OksEngine.KeyboardInputSystems") {
						Common::BreakPointLine();
					}
					auto fullPath = ECSGenerator2::GetTablePathByFullName(parsedEcsFile, arr1);
					if (!fullPath.empty()) {
						return fullPath;
					}
				}

				return ECSGenerator2::ParsedTablesPath{};
			};

		for (const auto parsedEcsFile : parsedECSFiles) {
			parsedEcsFile->ForEachSystem([&](ECSGenerator2::ParsedSystemPtr parsedSystem) {

				const auto systemNamespace = ECSGenerator2::GetSystemNamespace(parsedSystem);
				const auto systemName = parsedSystem->GetName();

				if (parsedSystem->ci_.callOrderInfo_ != nullptr) {

					for (auto& runAfterSystem : parsedSystem->ci_.callOrderInfo_->runAfter_) {
						//at first lets find run after system in namespace of current system.
						const auto runAfterName = ECSGenerator2::ParseFullName(runAfterSystem.name_);
						const auto runAfterSystemFullName = mergeArraysPreserveOrder(systemNamespace, runAfterName);

						if (runAfterSystem.name_ == "SendWindowKeyboardEvents") {
							Common::BreakPointLine();
						}

						//Try to find from namespace of current system and using source system name.
						const auto runAfterSystemTablesFullPathFirst = getTableByFullName(parsedECSFiles, runAfterSystemFullName);
						const auto runAfterSystemTablesFullPathSecond = getTableByFullName(parsedECSFiles, runAfterName);

						if (!runAfterSystemTablesFullPathFirst.empty()) {
							runAfterSystem.ptr_ = std::dynamic_pointer_cast<ECSGenerator2::ParsedSystem>(runAfterSystemTablesFullPathFirst.back());
						}
						else if (!runAfterSystemTablesFullPathSecond.empty()) {
							runAfterSystem.ptr_ = std::dynamic_pointer_cast<ECSGenerator2::ParsedSystem>(runAfterSystemTablesFullPathSecond.back());
						}
						else {
							OS::AssertFailMessage({ "Incorrect run after system name \"{}\" in system {}", runAfterSystem.name_, parsedSystem->GetName() });
						}
					}

					for (auto& runBeforeSystem : parsedSystem->ci_.callOrderInfo_->runBefore_) {
						//at first lets find run after system in namespace of current system.
						const auto runBeforeName = ECSGenerator2::ParseFullName(runBeforeSystem.name_);
						const auto runBeforeSystemFullName = mergeArraysPreserveOrder(systemNamespace, runBeforeName);

						if (runBeforeSystem.name_ == "SendWindowKeyboardEvents") {
							Common::BreakPointLine();
						}

						//Try to find from namespace of current system and using source system name.
						const auto runBeforeSystemTablesFullPathFirst = getTableByFullName(parsedECSFiles, runBeforeSystemFullName);
						const auto runBeforeSystemTablesFullPathSecond = getTableByFullName(parsedECSFiles, runBeforeName);

						if (!runBeforeSystemTablesFullPathFirst.empty()) {
							runBeforeSystem.ptr_ = std::dynamic_pointer_cast<ECSGenerator2::ParsedSystem>(runBeforeSystemTablesFullPathFirst.back());
						}
						else if (!runBeforeSystemTablesFullPathSecond.empty()) {
							runBeforeSystem.ptr_ = std::dynamic_pointer_cast<ECSGenerator2::ParsedSystem>(runBeforeSystemTablesFullPathSecond.back());
						}
						else {
							OS::AssertFailMessage({ "Incorrect run after system name \"{}\" in system {}", runBeforeSystem.name_, parsedSystem->GetName() });
						}
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

