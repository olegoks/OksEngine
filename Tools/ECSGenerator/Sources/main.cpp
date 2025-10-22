#include <filesystem>

#include <ECSGenerator.Generator.hpp>
#include <ECSGenerator.CodeGenerator.hpp>

#include <OksEngine.Config.hpp>

namespace ECSGenerator {

	std::vector<std::shared_ptr<ParsedECSFile>> ParseEcsFile(std::filesystem::path path, Resources::ResourceData& ecsFileData) {

		std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles;
		::Lua::Script script{ std::string{ ecsFileData.GetData<Common::Byte>(), ecsFileData.GetSize()} };

		::Lua::Context context;
		context.LoadScript(script);
		if (!context.GetGlobalAsRef("component").isNil()) {
			auto parsedComponentEcsFile = ParseComponentEcsFile(path, context, "");
			ecsFiles.push_back(parsedComponentEcsFile);
		}

		if (!context.GetGlobalAsRef("system").isNil()) {
			auto parsedSystemEcsFile = ECSGenerator::ParseSystemEcsFile(path, context, "");
			ecsFiles.push_back(parsedSystemEcsFile);
		}


		// Получение глобального пространства
		luabridge::LuaRef global = luabridge::getGlobal(context.state_, "_G");

		// Сбор всех глобальных таблиц
		std::vector<std::string> tableNames;
		for (auto it = luabridge::pairs(global).begin(); !it.isNil(); ++it) {
			luabridge::LuaRef key = it.key();
			luabridge::LuaRef value = it.value();

			if (value.isTable() && key.isString()) {
				tableNames.push_back(key.tostring());
			}
		}

		std::vector<std::string> systems;
		std::vector<std::string> components;

		for (const std::string& globalName : tableNames) {
			if (globalName.ends_with("Component")) {
				components.push_back(globalName);
			}
			if (globalName.ends_with("System")) {
				systems.push_back(globalName);
			}
		}

		for (auto& systemTableName : systems){
			auto parsedSystemEcsFile = ECSGenerator::ParseSystemEcsFile(path, context, systemTableName);
			ecsFiles.push_back(parsedSystemEcsFile);
			//ECSGenerator::ParseECSFileTest(path, context);
		}

		for (auto& componentTableName : components) {
			auto parsedComponentEcsFile = ECSGenerator::ParseComponentEcsFile(path, context, componentTableName);
			ecsFiles.push_back(parsedComponentEcsFile);
			//ECSGenerator::ParseECSFileTest(path, context);
		}

		return ecsFiles;
	}
}

int main(int argc, char** argv) {

	OS::CommandLineParameters parameters{ argc, argv };


	OS::InitializeLogger(
		parameters.GetArgc(),
		parameters.GetArgv());

	ASSERT_FMSG(argc > 1, "");
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
	//for (auto& workDir : workDirs) {
	//	for (const auto& entry : std::filesystem::recursive_directory_iterator(workDir)) {
	//		try {
	//			const std::filesystem::path name = root / entry.path().filename();
	//			if (resourceSystem.IsNodeExist(name) && std::filesystem::is_regular_file(entry)) {
	//				const bool ecsFile = entry.path().extension() == ".ecs"; // Systems and components
	//				const bool dotFile = entry.path().extension() == ".dot"; //Systems graph
	//				const bool cfgFile = entry.path().extension() == ".cfg";
	//				if (cfgFile) {
	//					resourceSystem.LoadResource(name);
	//				}
	//				if (dotFile) {

	//					resourceSystem.LoadResource(name);
	//					dotSystemPath_ = name;
	//				}
	//				if (ecsFile) {
	//					const std::filesystem::path name = root / entry.path().filename();
	//					resourceSystem.LoadResource(name);
	//					ecsFileInfos.push_back(ECSFileInfo{
	//							.resourceSystemPath_ = name,
	//							.filesystemPath_ = entry.path()
	//						});
	//				}
	//			}
	//		}
	//		catch (const std::filesystem::filesystem_error& error) {
	//			OS::LogError("ECSGenerator/Loading files", error.what());
	//		}
	//	}
	//}
	std::vector<std::string_view> configFilePath = parameters.GetValue("-cfg");
	OS::Assert(configFilePath.size() == 1);
	//resourceSystem.LoadResource(root / std::filesystem::path(configFilePath[0]).filename());
	Resources::ResourceData configResourceData = resourceSystem.GetResourceData(root / std::filesystem::path(configFilePath[0]).filename());
	auto config = std::make_shared<OksEngine::ConfigFile>(std::string{ configResourceData.GetData<Common::Byte>(), configResourceData.GetSize() });

	auto generator = std::make_shared<ECSGenerator::CodeStructureGenerator>();

	ECSGenerator::ProjectContext::CreateInfo pcci{
		.config = config,
		.workDirs_ = workDirs
	};

	auto projectContext = std::make_shared<ECSGenerator::ProjectContext>(pcci);

	for (const ECSFileInfo& ecsFileInfo : ecsFileInfos) {
		Resources::ResourceData resourceData = resourceSystem.GetResourceData(ecsFileInfo.resourceSystemPath_);
		auto ecsFiles = ECSGenerator::ParseEcsFile(ecsFileInfo.filesystemPath_, resourceData);
		if (!ecsFiles.empty()) {
			for (auto ecsFile : ecsFiles) {
				projectContext->AddEcsFile(ecsFile);
			}
			
		}
	}


	auto codeGenerator = std::make_shared<ECSGenerator::CodeGenerator>();


	//Generate system .cpp files if need.
	projectContext->ForEachSystemEcsFile(
		[&](std::shared_ptr<ECSGenerator::ParsedECSFile> ecsFile) {
			auto systemEcsFile = Common::pointer_cast<ECSGenerator::ParsedSystemECSFile>(ecsFile);
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
			return true;
		});


	auto files = generator->GenerateECSCXXFilesStructure(projectContext);
	auto editEntityFile = generator->GenerateEditEntityHppFile(projectContext);
	files.push_back(editEntityFile);
	auto parseEntityFile = generator->GenerateParseEntityHppFile(projectContext);
	files.push_back(parseEntityFile);
	auto serializeEntityFile = generator->GenerateSerializeEntityHppFile(projectContext);
	files.push_back(serializeEntityFile);
	ECSGenerator::SystemCallsGraphDescriptionGenerator dotGenerator;
	auto* graph = dotGenerator.CreateSystemsGraph(projectContext);
	auto clusters = dotGenerator.FindClusters(graph);
	auto runSystemFile = generator->GenerateRunSystemsFiles(clusters, projectContext);
	//auto dotFile = dotGenerator.GenerateGraphText(graph, projectContext);

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

