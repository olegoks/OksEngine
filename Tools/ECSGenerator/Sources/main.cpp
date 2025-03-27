#include <filesystem>

#include <ECSGenerator.System.hpp>
#include <ECSGenerator.Component.hpp>
#include <ECSGenerator.System.hpp>
#include <ECSGenerator.Generator.hpp>

namespace ECSGenerator {



	std::shared_ptr<ParsedECSFile> ParseEcsFile(std::filesystem::path path, Resources::ResourceData& ecsFileData) {

		::Lua::Script script{ std::string{ ecsFileData.GetData<Common::Byte>(), ecsFileData.GetSize()} };

		::Lua::Context context;
		context.LoadScript(script);
		if (!context.GetGlobalAsRef("component").isNil()) {
			auto parsedComponentEcsFile = ParseComponentEcsFile(path, context);
			return parsedComponentEcsFile;
		}

		if (!context.GetGlobalAsRef("system").isNil()) {
			auto parsedSystemEcsFile = ECSGenerator::ParseSystemEcsFile(path, context);
			return parsedSystemEcsFile;
		}
	}
}

int main(int argc, char** argv) {

	OS::CommandLineParameters parameters{ argc, argv };


	OS::InitializeLogger(
		parameters.GetArgc(),
		parameters.GetArgv());

	OS::AssertMessage(argc > 1, "");
	const std::string_view workDir = parameters.GetValue("-workDir");
	std::vector<std::filesystem::path> workDirs{ workDir };
	Resources::ResourceSystem resourceSystem;
	resourceSystem.SetRoots(workDirs);

	struct ECSFileInfo {
		std::filesystem::path resourceSystemPath_;
		std::filesystem::path filesystemPath_;
	};

	std::vector<ECSFileInfo> ecsFileInfos;
	std::filesystem::path dotSystemPath_;
	const std::filesystem::path root = "Root";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(workDir)) {
		if (std::filesystem::is_regular_file(entry)) {
			const bool ecsFile = entry.path().extension() == ".ecs";
			const bool dotFile = entry.path().extension() == ".dot";
			if (dotFile) {
				const std::filesystem::path name = root / entry.path().filename();
				resourceSystem.LoadResource(name);
				dotSystemPath_ = name;
			}
			if (ecsFile) {
				const std::filesystem::path name = root / entry.path().filename();
				resourceSystem.LoadResource(name);
				ecsFileInfos.push_back(ECSFileInfo{
						.resourceSystemPath_ = name,
						.filesystemPath_ = entry.path()
					});
			}
		}
	}

	auto generator = std::make_shared<ECSGenerator::CodeStructureGenerator>();

	auto projectContext = std::make_shared<ECSGenerator::ProjectContext>();

	for (const ECSFileInfo& ecsFileInfo : ecsFileInfos) {
		Resources::ResourceData resourceData = resourceSystem.GetResourceData(ecsFileInfo.resourceSystemPath_);
		auto ecsFile = ECSGenerator::ParseEcsFile(ecsFileInfo.filesystemPath_, resourceData);
		projectContext->AddEcsFile(ecsFile);
	}


	auto codeGenerator = std::make_shared<ECSGenerator::CodeGenerator>();


	//Generate system .cpp files if need.
	projectContext->ForEachSystemEcsFile(
		[&](std::shared_ptr<ECSGenerator::ParsedSystemECSFile> systemEcsFile) {
			ECSGenerator::SystemFileStructureGenerator::CreateInfo ci{
				.includeDirectory_ = projectContext->includeDirectory_
			};
			ECSGenerator::SystemFileStructureGenerator generator{ ci };
			auto file = generator.GenerateSystemRealization(projectContext, systemEcsFile);

			if (!std::filesystem::exists(file.first)) {
				auto codeFile = codeGenerator->GenerateCode(file.second);



				OS::TextFile cxxECSCodeFile{ file.first };
				cxxECSCodeFile.Create();
				cxxECSCodeFile << codeFile->code_.code_;
			}

		});


	auto files = generator->GenerateECSCXXFilesStructure(projectContext);


	ECSGenerator::SystemCallsGraphDescriptionGenerator dotGenerator;
	auto* graph = dotGenerator.CreateSystemsGraph(projectContext);
	auto clusters = dotGenerator.FindClusters(graph);
	auto runSystemFile = generator->GenerateRunSystemsFiles(clusters, projectContext);
	auto dotFile = dotGenerator.GenerateGraphText(graph, projectContext);

	files.push_back(runSystemFile[0]);
	files.push_back(runSystemFile[1]);

	for (auto file : files) {
		if (file.first.filename().string() == "auto_OksEngine.RunSystems.cpp") {
			//__debugbreak();
		}
		auto codeFile = codeGenerator->GenerateCode(file.second);



		OS::TextFile cxxECSCodeFile{ file.first };
		cxxECSCodeFile.Create();
		cxxECSCodeFile << codeFile->code_.code_;
	}

	return 0;
}

