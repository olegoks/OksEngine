#include <filesystem>

#include <OS.CommandLineParameters.hpp>

#include <ECSGenerator2.Generator.hpp>
#include <ECSGenerator2.CodeGenerator.hpp>
#include <System/ECSGenerator2.SystemParser.hpp>
#include <ECSGenerator2.ParsedECSFile.hpp>

#include <OksEngine.Config.hpp>

#include <Resource.hpp>

namespace ECSGenerator2 {

	std::shared_ptr<ParsedECSFile> ParseEcsFile(std::filesystem::path path, Resources::ResourceData& ecsFileData) {

		::Lua::Script script{ std::string{ ecsFileData.GetData<Common::Byte>(), ecsFileData.GetSize()} };

		::Lua::Context context;
		context.LoadScript(script);
		luabridge::LuaRef ecsFile = luabridge::getGlobal(context.state_, "_G");

		//Get all global tables in the script.
		std::vector<std::string> tableNames;
		for (auto it = luabridge::pairs(ecsFile).begin(); !it.isNil(); ++it) {
			luabridge::LuaRef key = it.key();
			luabridge::LuaRef value = it.value();

			if (value.isTable() && key.isString()) {
				tableNames.push_back(key.tostring());
			}
		}

		std::vector<std::shared_ptr<ParsedSystem>> parsedSystems;

		for (const std::string& globalName : tableNames) {
			if (globalName.ends_with("Component")) {
				luabridge::LuaRef componentRef = ecsFile[globalName];

			}
			if (globalName.ends_with("System")) {
				luabridge::LuaRef systemRef = ecsFile[globalName];
				auto parsedSystem = ParseSystem(systemRef, path, globalName);

			}
		}

		ParsedECSFile::CreateInfo pEcsFileCI{
			.path_ = path,
			.systems_ = parsedSystems
		};

		auto parsedEcsFile = std::make_shared<ParsedECSFile>(pEcsFileCI);

		return parsedEcsFile;
	}
}

int main(int argc, char** argv) {

	OS::CommandLineParameters parameters{ argc, argv };


	OS::InitializeLogger(
		parameters.GetArgc(),
		parameters.GetArgv());

	OS::AssertMessage(argc > 1, "");
	const std::vector<std::string_view> workDirsArgv = parameters.GetValue("-workDir");

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
	resourceSystem.AddResource(std::filesystem::path{ configFilePath[0] }.filename().string(), std::filesystem::path{configFilePath[0]}, "Root");
	resourceSystem.LoadResource("Root" / std::filesystem::path{configFilePath[0]}.filename());
	OS::Assert(configFilePath.size() == 1);
	Resources::ResourceData configResourceData = resourceSystem.GetResourceData(root / std::filesystem::path(configFilePath[0]).filename());
	auto config = std::make_shared<OksEngine::ConfigFile>(std::string{ configResourceData.GetData<Common::Byte>(), configResourceData.GetSize() });

	auto generator = std::make_shared<ECSGenerator2::CodeStructureGenerator>();

	ECSGenerator2::ProjectContext::CreateInfo pcci{
		.config = config,
		.workDirs_ = workDirs
	};

	auto projectContext = std::make_shared<ECSGenerator2::ProjectContext>(pcci);

	for (const ECSFileInfo& ecsFileInfo : ecsFileInfos) {
		Resources::ResourceData resourceData = resourceSystem.GetResourceData(ecsFileInfo.resourceSystemPath_);
		auto ecsFile = ECSGenerator2::ParseEcsFile(ecsFileInfo.filesystemPath_, resourceData);
		/*for (auto ecsFile : ecsFiles) {
			projectContext->AddEcsFile(ecsFile);
		}*/
	}


	auto codeGenerator = std::make_shared<ECSGenerator2::CodeGenerator>();


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

