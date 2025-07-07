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
		.fileExtensions_ = { ".ecs", ".dot", ".lua" }
	};
	Resources::ResourceSystem resourceSystem{ rsci };
	resourceSystem.SetRoots(workDirs);

	struct ECSFileInfo {
		std::filesystem::path resourceSystemPath_;
		std::filesystem::path filesystemPath_;
	};

	std::vector<ECSFileInfo> ecsFileInfos;
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


	//Parse ecs files.
	std::vector<std::shared_ptr<ECSGenerator2::ParsedECSFile>> parsedECSFiles;
	ECSGenerator2::Parser ecsFileParser;

	for (const ECSFileInfo& ecsFileInfo : ecsFileInfos) {

		Resources::ResourceData resourceData = resourceSystem.GetResourceData(ecsFileInfo.resourceSystemPath_);
		const std::string ecsFileText{ resourceData.GetData<Common::Byte>(), resourceData.GetSize() };

		auto ecsFile = ecsFileParser.Parse(ecsFileInfo.filesystemPath_, ecsFileText);

		parsedECSFiles.push_back(ecsFile);
	}

	//GENERATE CODE STRUCTURE.

	ECSGenerator2::CodeStructureGenerator::CreateInfo csgci{
		.ecsFiles_ = parsedECSFiles
	};

	ECSGenerator2::CodeStructureGenerator codeStructureGenerator{ csgci };


	//Generate code structure for ecs files.
	std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::File>> structureFiles;
	{
		for (auto parsedECSFile : parsedECSFiles) {

			auto generatedFiles = codeStructureGenerator.Generate(
				includeDirArgv,
				parsedECSFile);

			for (auto generateFile : generatedFiles) {
				const auto ecsHppPath = parsedECSFile->GetPath().parent_path() / ("auto_OksEngine." + parsedECSFile->GetPath().filename().stem().string() + ".hpp");
				structureFiles[ecsHppPath] = generateFile;
			}

		}
	}

	//Generate code structure for auto_OksEngine.ECS.hpp files
	std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::File>> hppECSFiles;
	{
		hppECSFiles = codeStructureGenerator.GenerateDirECSIncludeHppFiles(includeDirArgv, includeDirArgv);
	}

	//Generate Parse entity code structure.

	auto parseEntityCodeStructure = codeStructureGenerator.GenerateParseEntityHppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::File>> pathToParsedEntity{ 
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.ParseEntity.hpp",
		parseEntityCodeStructure };

	auto serializeEntityCodeStructure = codeStructureGenerator.GenerateSerializeEntityHppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::File>> pathToSerializeEntity{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.SerializeEntity.hpp",
		serializeEntityCodeStructure };

	auto editEntityCodeStructure = codeStructureGenerator.GenerateEditEntityHppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::File>> pathToEditEntity{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.EditEntity.hpp",
		editEntityCodeStructure };

	auto hppRunSystemsCodeStructure = codeStructureGenerator.GenerateRunSystemsHppFile();
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::File>> pathToHppRunSystems{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.RunSystems.hpp",
		hppRunSystemsCodeStructure };

	auto cppRunSystemsCodeStructure = codeStructureGenerator.GenerateRunSystemsCppFile(parsedECSFiles);
	std::pair<
		std::filesystem::path,
		std::shared_ptr<ECSGenerator2::File>> pathToCppRunSystems{
		std::filesystem::path{ includeDirArgv } / "auto_OksEngine.RunSystems.cpp",
		cppRunSystemsCodeStructure };


	//Merge files with  code structure.
	std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::File>> allFilesCodeStructure;

	//allFilesCodeStructure.insert(structureFiles.begin(), structureFiles.end());
	//allFilesCodeStructure.insert(hppECSFiles.begin(), hppECSFiles.end());

	//allFilesCodeStructure.insert(pathToParsedEntity);
	//allFilesCodeStructure.insert(pathToSerializeEntity);
	//allFilesCodeStructure.insert(pathToEditEntity);
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

	//Generate system .cpp files if need.
	//projectContext->ForEachEcsFile(
	//	[&](std::shared_ptr<ECSGenerator2::ParsedECSFile> ecsFile) {
	//		auto systemEcsFile = std::dynamic_pointer_cast<ECSGenerator2::ParsedSystem>(ecsFile);
	//		ECSGenerator2::SystemFileStructureGenerator::CreateInfo ci{
	//			.includeDirectory_ = projectContext->includeDirectory_
	//		};
	//		ECSGenerator2::SystemFileStructureGenerator generator{ ci };
	//		auto file = generator.GenerateSystemRealization(projectContext, systemEcsFile);

	//		if (!std::filesystem::exists(file.first)) {
	//			auto codeFile = codeGenerator->GenerateCode(file.second);



	//			OS::TextFile cxxECSCodeFile{ file.first };
	//			cxxECSCodeFile.Create();
	//			cxxECSCodeFile << codeFile->code_.code_;
	//		}
	//		return true;
	//	});


	//auto files = generator->GenerateECSCXXFilesStructure(projectContext);
	//auto editEntityFile = generator->GenerateEditEntityHppFile(projectContext);
	//files.push_back(editEntityFile);
	//auto parseEntityFile = generator->GenerateParseEntityHppFile(projectContext);
	//files.push_back(parseEntityFile);
	//auto serializeEntityFile = generator->GenerateSerializeEntityHppFile(projectContext);
	//files.push_back(serializeEntityFile);
	//ECSGenerator2::SystemCallsGraphDescriptionGenerator dotGenerator;
	//auto* graph = dotGenerator.CreateSystemsGraph(projectContext);
	//auto clusters = dotGenerator.FindClusters(graph);
	//auto runSystemFile = generator->GenerateRunSystemsFiles(clusters, projectContext);
	//auto dotFile = dotGenerator.GenerateGraphText(graph, projectContext);

	//files.push_back(runSystemFile[0]);
	//files.push_back(runSystemFile[1]);

	//for (auto file : files) {
	//	if (file.first.filename().string() == "auto_OksEngine.RunSystems.cpp") {
	//		//__debugbreak();
	//	}
	//	auto codeFile = codeGenerator->GenerateCode(file.second);



	//	OS::TextFile cxxECSCodeFile{ file.first };
	//	cxxECSCodeFile.Create();
	//	cxxECSCodeFile << codeFile->code_.code_;
	//}

	return 0;
}

