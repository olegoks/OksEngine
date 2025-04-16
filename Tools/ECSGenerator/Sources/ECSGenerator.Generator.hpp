#pragma once 

#include <ECSGenerator.Common.hpp>
#include <ECSGenerator.ProjectContext.hpp>
#include <ECSGenerator.CodeFile.hpp>
#include <ECSGenerator.SystemsGraph.hpp>


#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <random>  

#include <clang/Format/Format.h>
#include <clang/Tooling/Core/Replacement.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Path.h>


namespace ECSGenerator {




	class CodeStructureGenerator {
	public:

		Code GenerateRunSystemCode(std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
			Code runSystemCode;
			runSystemCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"" + systemEcsFile->GetName() + "\");");
			runSystemCode.Add(systemEcsFile->GetName() + "System(world2);");
			runSystemCode.NewLine();
			runSystemCode.Add("PIXEndEvent();");
			return runSystemCode;
		}

		std::shared_ptr<Function> GenerateRunInitSystemsFunctionPrototype(std::shared_ptr<ProjectContext> projectContext) {



		}

		std::shared_ptr<Function> GenerateRunInitSystemsFunctionRealization(std::shared_ptr<ProjectContext> projectContext) {
			Code runInitSystemsCode;
			runInitSystemsCode.Add(
				"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Start initialize frame\");"
				"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"StartFrame\");"
				"world2->StartFrame();"
				"PIXEndEvent();");

			//Generate code to run systems that process all entities.
			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedECSFile> ecsFile) {
					auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
					if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize) {
						runInitSystemsCode.Add(GenerateRunSystemCode(systemEcsFile));
					}
				});

			runInitSystemsCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"End enitialize frame\");");
			runInitSystemsCode.Add("world2->EndFrame();");
			runInitSystemsCode.Add("PIXEndEvent();");

			//CreateThreads method realization.
			Function::CreateInfo cppRunSystemsFunction{
				.name_ = "RunInitializeSystems",
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "world2" }
				},
				.returnType_ = "void",
				.code_ = runInitSystemsCode,
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto runInitializeSystemsFunctionRealization = std::make_shared<Function>(cppRunSystemsFunction);

			return runInitializeSystemsFunctionRealization;

		}


		using System = std::string;
		using Component = std::string;


		struct Thread {
			std::map<System, std::set<Component>> systems_;
		};


		std::shared_ptr<Function> GenerateCreateThreadRealization(const std::vector<Thread>& threads, std::shared_ptr<ProjectContext> projectContext) {


			//Create threads
			Code cppCreateThreadsCode;
			{

				for (Common::Index i = 0; i < threads.size(); ++i) {
					auto& thread = threads[i];
					const std::string threadIdStr = std::to_string(i);
					cppCreateThreadsCode.AddComment("Thread " + threadIdStr);
					//Add comment: what are components in use.
					std::set<std::string> threadComponents;
					for (auto& system : thread.systems_) {
						for (auto& component : system.second) {
							threadComponents.insert(component);
						}
					}
					for (auto& threadComponent : threadComponents) {
						cppCreateThreadsCode.AddComment(threadComponent);
					}

					cppCreateThreadsCode.Add(std::format(
						"thread{} = std::thread{{ [&](){{ "
						"while(true) {{ "
						"std::unique_lock lock{{ thread{}Mutex }}; "
						"thread{}CV.wait(lock, []() {{ return runSystemThread{}== true; }});"
						"HRESULT r;"
						"r = SetThreadDescription("
						"GetCurrentThread(),"
						"L\"Thread {}\""
						");"
						"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Thread {}\");",
						threadIdStr,
						threadIdStr,
						threadIdStr,
						threadIdStr,
						threadIdStr,
						threadIdStr
					));

					Code runThreadSystems;
					for (auto it = thread.systems_.begin(); it != thread.systems_.end(); it++) {
						const std::string systemName = it->first;
						runThreadSystems.Add(std::format(
							"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
							"{}System(world2);"
							"PIXEndEvent();",
							systemName,
							systemName
						));
					}

					//runThreadSystems.ApplyTab();
					cppCreateThreadsCode.Add(runThreadSystems);
					cppCreateThreadsCode.Add(std::format(
						"PIXEndEvent();"
						"runSystemThread{} = false;"
						"thread{}CV.notify_all();"
						"}}"
						"}} }};",
						threadIdStr,
						threadIdStr
					));
				}

			}

			//CreateThreads method realization.
			Function::CreateInfo cppCreateThreadsFunction{
				.name_ = "CreateThreads",
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "world2" }
				},
				.returnType_ = "void",
				.code_ = cppCreateThreadsCode,
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto cppCreateThreadsFunctionObject = std::make_shared<Function>(cppCreateThreadsFunction);

			return cppCreateThreadsFunctionObject;
		}


		std::pair<std::filesystem::path, std::shared_ptr<File>>
			GenerateRunSystemsHppFile(std::shared_ptr<ProjectContext> projectContext) {

			auto generateRunSystemsFunctionPrototype = []() -> std::shared_ptr<Function> {
				Function::CreateInfo hppRunSystemsFunction{
					.name_ = "RunSystems",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world2"}},
					.returnType_ = "void",
					.code_ = {},
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				return std::make_shared<Function>(hppRunSystemsFunction);
				};

			auto generateCreateThreadsFunctionPrototype = []() -> std::shared_ptr<Function> {
				//Create threads method prototype.
				Function::CreateInfo hppCreateThreadsFunction{
					.name_ = "CreateThreads",
					.parameters_ = { { "std::shared_ptr<ECS2::World>", "world2"} },
					.returnType_ = "void",
					.code_ = {},
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				return std::make_shared<Function>(hppCreateThreadsFunction);
				};

			auto generateRunInitSystemsFunctionPrototype = []() -> std::shared_ptr<Function> {
				//CreateThreads method prototype.
				Function::CreateInfo cppRunSystemsFunction{
					.name_ = "RunInitializeSystems",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world2" }
					},
					.returnType_ = "void",
					.code_ = {},
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				return std::make_shared<Function>(cppRunSystemsFunction);
				};

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");

			namespaceObject->Add(generateRunSystemsFunctionPrototype());
			namespaceObject->Add(generateCreateThreadsFunctionPrototype());
			namespaceObject->Add(generateRunInitSystemsFunctionPrototype());

			File::Includes includes{ };
			includes.paths_.push_back("boost/asio/thread_pool.hpp");
			includes.paths_.push_back("boost/asio/post.hpp");
			includes.paths_.push_back("ECS2.World.hpp");


			//hpp file
			File::CreateInfo fci{
			.isHpp_ = true,
			.includes_ = includes,
			.base_ = namespaceObject
			};

			auto file = std::make_shared<File>(fci);



			//Generate files Paths

			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.hpp");
			FormatPath(systemCppFileFullPath);

			return { systemCppFileFullPath, file };
		}

		std::pair<std::filesystem::path, std::shared_ptr<File>>
			GenerateRunSystemsCppFile(std::vector<std::vector<Agnode_t*>> clusters, std::shared_ptr<ProjectContext> projectContext) {

			//Components and which systems uses them.
			std::map<System, std::set<Component>> componentSystem;
			{
				projectContext->ForEachSystemEcsFile(
					[&](std::shared_ptr<ParsedECSFile> ecsFile) {
						auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
						const std::string systemName = systemEcsFile->GetName();
						for (auto& entity : systemEcsFile->ci_.processesEntities_) {
							entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
								if (componentSystem.find(include.name_) != componentSystem.end()) {
									componentSystem.insert({});
								}
								componentSystem[include.name_].insert(systemName);
								return true;
								});
						}
					});
			}

			//Systems and which components they use
			std::map<std::string, std::set<std::string>> systemComponents;
			{
				for (auto& [component, systems] : componentSystem) {
					for (const auto& system : systems) {
						systemComponents[system].insert(component);
					}
				}
			}

			//Separate systems on threads.
			std::vector<Thread> childThreads;
			Thread mainThread;
			{
				for (auto& clusterSystems : clusters) {
					bool isMainThread = false;
					for (Agnode_t* systemNode : clusterSystems) {
						std::string systemName = agnameof(systemNode);
						projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile) {
							if ("OksEngine.ProcessInput.ecs" == ecsFile->GetPath().filename()) {
								//__debugbreak();
							}
							auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
							if (systemName == systemEcsFile->GetName() && systemEcsFile->ci_.thread_ == ParsedSystemECSFile::Thread::Main) {
								isMainThread = true;
							}
							});

					}
					Thread clusterThread;
					for (Agnode_t* systemNode : clusterSystems) {
						std::string systemName = agnameof(systemNode);

						clusterThread.systems_[systemName] = componentSystem[systemName];

					}
					if (isMainThread) {
						mainThread.systems_.insert(
							clusterThread.systems_.begin(),
							clusterThread.systems_.end());
					}
					else {
						childThreads.push_back(clusterThread);
					}
				}
			}

			//Group threads systems to get less threads.
			{
				const Common::UInt64 threadsNumber = projectContext->GetConfig()->GetValueAs<Common::UInt64>("Multithreading.threadsNumber");

				auto optimize = [](std::vector<Thread>& threads) {

					std::sort(threads.begin(), threads.end(), [](const Thread& first, const Thread& second) {

						return first.systems_.size() > second.systems_.size();

						});
					auto lastIt = --threads.end();
					auto preLastIt = lastIt--;
					for (auto& [system, components] : lastIt->systems_) {
						preLastIt->systems_[system] = components;
					}
					threads.erase(lastIt);
					};

				while (childThreads.size() > threadsNumber) {

					optimize(childThreads);

				}
			}


			using namespace std::string_literals;

			//Generate .CPP.

			//Generate Includes.
			File::Includes cppIncludes{ };
			{
				//Must be first to escape 
				//error C1189: #error:  WinSock.h has already been included
				cppIncludes.paths_.push_back("auto_OksEngine.RunSystems.hpp");

				projectContext->ForEachSystemEcsFile(
					[&](std::shared_ptr<ParsedECSFile> ecsFile) {
						auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
						if ("OksEngine.ProcessInput.ecs" == systemEcsFile->GetPath().filename()) {
							//__debugbreak();
						}
						std::filesystem::path systemIncludePath = GetSubPath(
							systemEcsFile->GetPath().parent_path(),
							projectContext->includeDirectory_,
							ResultRange::FromStartFolderToEnd,
							SearchDirection::FromEndToBegin,
							true);
						cppIncludes.paths_.push_back(systemIncludePath / ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp"));
					});
				cppIncludes.paths_.push_back("pix3.h");
			}

			auto cppNamespaceObject = std::make_shared<Namespace>("OksEngine");

			{

				for (Common::Index i = 0; i < childThreads.size(); ++i) {
					auto& thread = childThreads[i];

					Variable::CreateInfo threadVariable{
						.type_ = "std::thread",
						.name_ = "thread" + std::to_string(i)
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(threadVariable));

					std::mutex thread16Mutex;

					Variable::CreateInfo threadMutexVariable{
						.type_ = "std::mutex",
						.name_ = "thread" + std::to_string(i) + "Mutex"
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(threadMutexVariable));


					Variable::CreateInfo runSystemThreadVariable{
						.type_ = "std::atomic_bool",
						.name_ = "runSystemThread" + std::to_string(i),
						.initValue_ = "false"
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(runSystemThreadVariable));

					Variable::CreateInfo threadCVVariable{
						.type_ = "std::condition_variable",
						.name_ = "thread" + std::to_string(i) + "CV"
					};
					cppNamespaceObject->Add(std::make_shared<Variable>(threadCVVariable));
				}

				Variable::CreateInfo threadCVVariable{
						.type_ = "Common::UInt64",
						.name_ = "frameNumber",
						.initValue_ = "0"
				};
				cppNamespaceObject->Add(std::make_shared<Variable>(threadCVVariable));
			}

			Code cppRunSystemsCode;
			{
				cppRunSystemsCode.Add(
					"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Frame %d\", frameNumber);"
					"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"StartFrame\");"
					"world2->StartFrame();"
					"PIXEndEvent();");

				//Start threads.
				for (Common::Index i = 0; i < childThreads.size(); ++i) {
					auto& thread = childThreads[i];
					std::string threadId = std::to_string(i);
					cppRunSystemsCode.Add(std::format(
						"runSystemThread{} = true;"
						"{{"
						"std::unique_lock lock{{thread{}Mutex}};"
						"thread{}CV.notify_one();"
						"}}",
						threadId,
						threadId,
						threadId
					).c_str());
				}

				//Generate code to run main thread systems.
				{
					Code runMainThreadSystems;
					for (auto it = mainThread.systems_.begin(); it != mainThread.systems_.end(); it++) {
						runMainThreadSystems.Add(std::format(
							"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
							"{}System(world2);"
							"PIXEndEvent();",
							it->first,
							it->first
						));
						auto itCopy = it;
						if (itCopy++ != mainThread.systems_.end()) {
							runMainThreadSystems.NewLine();
						}
					}
					cppRunSystemsCode.Add(runMainThreadSystems);
				}

				//Wait for threads end.
				for (Common::Index i = 0; i < childThreads.size(); ++i) {
					auto& thread = childThreads[i];
					std::string threadId = std::to_string(i);
					cppRunSystemsCode.Add(std::format(
						"{{"
						"std::unique_lock lock{{thread{}Mutex}};"
						"thread{}CV.wait(lock, [&]() {{ return runSystemThread"
						"{} == false;"
						"}});"
						"}}",
						threadId,
						threadId,
						threadId
					));

				}

				//Generate code to run systems that process all entities.
				projectContext->ForEachSystemEcsFile(
					[&](std::shared_ptr<ParsedECSFile> ecsFile) {
						auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
						if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::AllEntities) {

							cppRunSystemsCode.Add(GenerateRunSystemCode(systemEcsFile));
							//cppRunSystemsCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"" + systemEcsFile->GetName() + "\");");
							//cppRunSystemsCode.Add(systemEcsFile->GetName() + "System(world2);");
							//cppRunSystemsCode.NewLine();
							//cppRunSystemsCode.Add("PIXEndEvent();");
						}
					});

				cppRunSystemsCode.Add(
					"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"EndFrame\");"
					"world2->EndFrame();"
					"PIXEndEvent();"
					"PIXEndEvent();"
					"++frameNumber;");
			}

			//CreateThreads method realization.
			Function::CreateInfo cppRunSystemsFunction{
				.name_ = "RunSystems",
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "world2" }
				},
				.returnType_ = "void",
				.code_ = cppRunSystemsCode,
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto cppRunSystemsFunctionObject = std::make_shared<Function>(cppRunSystemsFunction);
			cppNamespaceObject->Add(cppRunSystemsFunctionObject);

			cppNamespaceObject->Add(GenerateRunInitSystemsFunctionRealization(projectContext));
			cppNamespaceObject->Add(GenerateCreateThreadRealization(childThreads, projectContext));


			//cpp file
			File::CreateInfo cppfci{
			.isHpp_ = false,
			.includes_ = cppIncludes,
			.base_ = cppNamespaceObject
			};

			auto cppFile = std::make_shared<File>(cppfci);


			//Generate files Paths

			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.cpp");
			FormatPath(systemCppFileFullPath);

			return { systemCppFileFullPath , cppFile };

		}




		std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>>
			GenerateRunSystemsFiles(std::vector<std::vector<Agnode_t*>> clusters, std::shared_ptr<ProjectContext> projectContext) {

			////Components and which systems uses them.
			//std::map<System, std::set<Component>> componentSystem;
			//projectContext->ForEachSystemEcsFile(
			//	[&](std::shared_ptr<ParsedSystemECSFile> componentEcsFile) {

			//		const std::string systemName = componentEcsFile->GetName();
			//		for (auto& entity : componentEcsFile->ci_.processesEntities_) {
			//			entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {
			//				if (componentSystem.find(include.name_) != componentSystem.end()) {
			//					componentSystem.insert({});
			//				}
			//				componentSystem[include.name_].insert(systemName);
			//				return true;
			//				});
			//		}
			//	});

			//std::map<std::string, std::set<std::string>> systemComponents;

			//for (auto& [component, systems] : componentSystem) {
			//	for (const auto& system : systems) {
			//		systemComponents[system].insert(component);
			//	}
			//}



			//std::vector<Thread> childThreads;

			//std::vector<Thread> mainThreads;

			//for (auto& clusterSystems : clusters) {
			//	bool isMainThread = false;
			//	for (Agnode_t* systemNode : clusterSystems) {
			//		std::string systemName = agnameof(systemNode);
			//		projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
			//			if (systemName == systemEcsFile->GetName() && systemEcsFile->ci_.thread_ == ParsedSystemECSFile::Thread::Main) {
			//				isMainThread = true;
			//			}
			//			});

			//	}
			//	Thread clusterThread;
			//	for (Agnode_t* systemNode : clusterSystems) {
			//		std::string systemName = agnameof(systemNode);

			//		clusterThread.systems_[systemName] = componentSystem[systemName];

			//	}
			//	if (isMainThread) {
			//		mainThreads.push_back(clusterThread);
			//	}
			//	else {
			//		childThreads.push_back(clusterThread);
			//	}
			//}

			////Group threads systems to get less threads.
			//{
			//	const Common::UInt64 threadsNumber = 6;

			//	auto optimize = [](std::vector<Thread>& threads) {

			//		std::sort(threads.begin(), threads.end(), [](const Thread& first, const Thread& second) {

			//			return first.systems_.size() > second.systems_.size();

			//			});
			//		auto lastIt = --threads.end();
			//		auto preLastIt = lastIt--;
			//		for (auto& [system, components] : lastIt->systems_) {
			//			preLastIt->systems_[system] = components;
			//		}
			//		threads.erase(lastIt);
			//		};

			//	while (childThreads.size() > threadsNumber) {

			//		optimize(childThreads);

			//	}
			//}

			//std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> files;

			//using namespace std::string_literals;

			////Generate .CPP.

			////Generate Includes.
			//File::Includes cppIncludes{ };
			////Must be first to escape 
			////error C1189: #error:  WinSock.h has already been included
			//cppIncludes.paths_.push_back("auto_OksEngine.RunSystems.hpp");

			//projectContext->ForEachSystemEcsFile(
			//	[&](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
			//		std::filesystem::path systemIncludePath = GetSubPath(
			//			systemEcsFile->GetPath().parent_path(),
			//			projectContext->includeDirectory_,
			//			ResultRange::FromStartFolderToEnd,
			//			SearchDirection::FromEndToBegin,
			//			true);
			//		cppIncludes.paths_.push_back(systemIncludePath / ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp"));
			//	});
			//cppIncludes.paths_.push_back("pix3.h");

			//auto cppNamespaceObject = std::make_shared<Namespace>("OksEngine");

			//{

			//	for (Common::Index i = 0; i < childThreads.size(); ++i) {
			//		auto& thread = childThreads[i];

			//		Variable::CreateInfo threadVariable{
			//			.type_ = "std::thread",
			//			.name_ = "thread" + std::to_string(i)
			//		};
			//		cppNamespaceObject->Add(std::make_shared<Variable>(threadVariable));

			//		std::mutex thread16Mutex;

			//		Variable::CreateInfo threadMutexVariable{
			//			.type_ = "std::mutex",
			//			.name_ = "thread" + std::to_string(i) + "Mutex"
			//		};
			//		cppNamespaceObject->Add(std::make_shared<Variable>(threadMutexVariable));


			//		Variable::CreateInfo runSystemThreadVariable{
			//			.type_ = "std::atomic_bool",
			//			.name_ = "runSystemThread" + std::to_string(i),
			//			.initValue_ = "false"
			//		};
			//		cppNamespaceObject->Add(std::make_shared<Variable>(runSystemThreadVariable));

			//		Variable::CreateInfo threadCVVariable{
			//			.type_ = "std::condition_variable",
			//			.name_ = "thread" + std::to_string(i) + "CV"
			//		};
			//		cppNamespaceObject->Add(std::make_shared<Variable>(threadCVVariable));
			//	}

			//	Variable::CreateInfo threadCVVariable{
			//			.type_ = "Common::UInt64",
			//			.name_ = "frameNumber",
			//			.initValue_ = "0"
			//	};
			//	cppNamespaceObject->Add(std::make_shared<Variable>(threadCVVariable));
			//}

			//Code cppRunSystemsCode;
			//{
			//	cppRunSystemsCode.Add(
			//		"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Frame %d\", frameNumber);"
			//		"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"StartFrame\");"
			//		"world2->StartFrame();"
			//		"PIXEndEvent();");

			//	//Start threads.
			//	for (Common::Index i = 0; i < childThreads.size(); ++i) {
			//		auto& thread = childThreads[i];
			//		std::string threadId = std::to_string(i);
			//		cppRunSystemsCode.Add(std::format(
			//			"runSystemThread{} = true;"
			//			"{{"
			//			"std::unique_lock lock{{thread{}Mutex}};"
			//			"thread{}CV.notify_one();"
			//			"}}",
			//			threadId,
			//			threadId,
			//			threadId
			//		).c_str());
			//	}

			//	//Generate code to run main thread systems.
			//	for (Common::Index i = 0; i < mainThreads.size(); ++i) {
			//		auto& mainThread = mainThreads[i];
			//		Code runMainThreadSystems;
			//		for (auto it = mainThread.systems_.begin(); it != mainThread.systems_.end(); it++) {
			//			runMainThreadSystems.Add(std::format(
			//				"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
			//				"{}System(world2);"
			//				"PIXEndEvent();",
			//				it->first,
			//				it->first
			//			));
			//			auto itCopy = it;
			//			if (itCopy++ != mainThread.systems_.end()) {
			//				runMainThreadSystems.NewLine();
			//			}
			//		}
			//		cppRunSystemsCode.Add(runMainThreadSystems);
			//	}

			//	//Wait for threads end.
			//	for (Common::Index i = 0; i < childThreads.size(); ++i) {
			//		auto& thread = childThreads[i];
			//		std::string threadId = std::to_string(i);
			//		cppRunSystemsCode.Add(std::format(
			//			"{{"
			//			"std::unique_lock lock{{thread{}Mutex}};"
			//			"thread{}CV.wait(lock, [&]() {{ return runSystemThread"
			//			"{} == false;"
			//			"}});"
			//			"}}",
			//			threadId,
			//			threadId,
			//			threadId
			//		));

			//	}

			//	//Generate code to run systems that process all entities.
			//	projectContext->ForEachSystemEcsFile(
			//		[&](std::shared_ptr<ParsedSystemECSFile> systemEcsFile) {
			//			if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::AllEntities) {

			//				cppRunSystemsCode.Add(GenerateRunSystemCode(systemEcsFile));
			//				//cppRunSystemsCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"" + systemEcsFile->GetName() + "\");");
			//				//cppRunSystemsCode.Add(systemEcsFile->GetName() + "System(world2);");
			//				//cppRunSystemsCode.NewLine();
			//				//cppRunSystemsCode.Add("PIXEndEvent();");
			//			}
			//		});

			//	cppRunSystemsCode.Add(
			//		"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"EndFrame\");"
			//		"world2->EndFrame();"
			//		"PIXEndEvent();"
			//		"PIXEndEvent();"
			//		"++frameNumber;");
			//}

			////CreateThreads method realization.
			//Function::CreateInfo cppRunSystemsFunction{
			//	.name_ = "RunSystems",
			//	.parameters_ = {
			//		{ "std::shared_ptr<ECS2::World>", "world2" }
			//	},
			//	.returnType_ = "void",
			//	.code_ = cppRunSystemsCode,
			//	.isPrototype_ = false,
			//	.inlineModifier_ = false
			//};

			//auto cppRunSystemsFunctionObject = std::make_shared<Function>(cppRunSystemsFunction);
			//cppNamespaceObject->Add(cppRunSystemsFunctionObject);


			//auto runInitializeSystemsRealization = GenerateRunInitSystemsFunctionRealization(projectContext);

			//cppNamespaceObject->Add(runInitializeSystemsRealization);
			//cppNamespaceObject->Add(cppCreateThreadsFunctionObject);


			////cpp file
			//File::CreateInfo cppfci{
			//.isHpp_ = false,
			//.includes_ = cppIncludes,
			//.base_ = cppNamespaceObject
			//};

			//auto cppFile = std::make_shared<File>(cppfci);


			//Generate .HPP.
			//Update method prototype.
			//Function::CreateInfo hppRunSystemsFunction{
			//	.name_ = "RunSystems",
			//	.parameters_ = {
			//		{ "std::shared_ptr<ECS2::World>", "world2"}},
			//	.returnType_ = "void",
			//	.code_ = {},
			//	.isPrototype_ = true,
			//	.inlineModifier_ = false
			//};

			//auto hppRunSystemsFunctionObject = std::make_shared<Function>(hppRunSystemsFunction);

			////Create threads method prototype.
			//Function::CreateInfo hppCreateThreadsFunction{
			//	.name_ = "CreateThreads",
			//	.parameters_ = { { "std::shared_ptr<ECS2::World>", "world2"} },
			//	.returnType_ = "void",
			//	.code_ = {},
			//	.isPrototype_ = true,
			//	.inlineModifier_ = false
			//};

			//auto hppCreateThreadsFunctionObject = GenerateCreateThreadsPrototype(projectContext);//std::make_shared<Function>(hppCreateThreadsFunction);

			//auto hppRunInitializeSystemPrototype = GenerateRunInitSystemsFunctionPrototype(projectContext);

			//auto hppNamespaceObject = std::make_shared<Namespace>("OksEngine");

			//hppNamespaceObject->Add(hppRunInitializeSystemPrototype);
			//hppNamespaceObject->Add(hppCreateThreadsFunctionObject);
			//hppNamespaceObject->Add(hppRunSystemsFunctionObject);

			//File::Includes hppIncludes{ };
			//hppIncludes.paths_.push_back("boost/asio/thread_pool.hpp");
			//hppIncludes.paths_.push_back("boost/asio/post.hpp");
			//hppIncludes.paths_.push_back("ECS2.World.hpp");


			////hpp file
			//File::CreateInfo hppfci{
			//.isHpp_ = true,
			//.includes_ = hppIncludes,
			//.base_ = hppNamespaceObject
			//};

			//auto hppFile = std::make_shared<File>(hppfci);


			////Generate files Paths

			//auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			//std::filesystem::path includeDirFullPath;

			//std::filesystem::path::iterator includeDirIt;
			//for (auto it = randomEcsFilePath.end(); it != randomEcsFilePath.begin(); --it) {
			//	auto folder = *it;
			//	if (folder == projectContext->includeDirectory_) {
			//		includeDirIt = it;
			//		break;
			//	}
			//}

			//for (auto it = randomEcsFilePath.begin(); it != includeDirIt; it++) {
			//	includeDirFullPath /= *it;
			//}

			//includeDirFullPath /= *includeDirIt;

			//std::filesystem::path systemHppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.hpp");
			//FormatPath(systemHppFileFullPath);

			//std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.cpp");
			//FormatPath(systemCppFileFullPath);

			return {
				GenerateRunSystemsHppFile(projectContext),
				GenerateRunSystemsCppFile(clusters, projectContext)
			};
		}


		std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext) {

			std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> files;


			ComponentFileStructureGenerator::CreateInfo ci{
				.includeDirectory_ = projectContext->includeDirectory_
			};

			projectContext->ForEachComponentEcsFile(
				[&](std::shared_ptr<ParsedECSFile> ecsFile) {
					auto componentEcsFile = std::dynamic_pointer_cast<ParsedComponentECSFile>(ecsFile);
					ComponentFileStructureGenerator generator{ ci };
					auto file = generator.GenerateECSCXXFilesStructure(projectContext, componentEcsFile);
					files.push_back(file);
				});

			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedECSFile> ecsFile) {
					auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
					SystemFileStructureGenerator::CreateInfo ci{
						.includeDirectory_ = projectContext->includeDirectory_
					};
					SystemFileStructureGenerator generator{ ci };
					auto file = generator.GenerateECSCXXFilesStructure(projectContext, systemEcsFile);



					files.push_back(file);
				});



			//Generate OksEngine.Components.hpp files For COMPONENTS.
			for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

				//Generate components includes for components in the subdirectory 
				File::Includes includes{};
				std::filesystem::path componentIncludesFilePath;
				for (auto componentParsedEcsFile : componentsSystems.components_) {
					//std::filesystem::path componentFullPath = ;
					componentIncludesFilePath = componentParsedEcsFile->GetPath().parent_path();
					//std::filesystem::path componentIncludePath = GetSubPathFromFolderToEnd(componentFullPath.parent_path(), projectContext->includeDirectory_);
					const std::filesystem::path includePath = GetSubPath(
						componentParsedEcsFile->GetPath().parent_path(),
						projectContext->includeDirectory_,
						ResultRange::FromStartFolderToEnd,
						SearchDirection::FromEndToBegin,
						true);
					//componentIncludePath /= ;
					includes.paths_.push_back(includePath / ("auto_OksEngine." + componentParsedEcsFile->GetName() + ".hpp"));
				}

				//generate Include OksEngine.Components.hpp files from subdirectories.
				for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
					if (path.string().starts_with(categoryPath.string())) {
						if (path == categoryPath) {
							continue;
						}
						auto pathIt = path.begin();
						bool subdirFound = true;
						for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
							if (*it != *pathIt) {
								subdirFound = false;
								break;
							}
							++pathIt;
						}
						if (subdirFound) {
							//Subdirectory found!
							std::filesystem::path subdirPath;
							++pathIt;
							for (auto it = path.begin(); it != pathIt; ++it) {
								subdirPath /= *it;
							}
							//std::filesystem::path componentIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
							const std::filesystem::path componentIncludePath = GetSubPath(
								subdirPath,
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							//componentIncludePath ;
							includes.paths_.push_back(componentIncludePath / ("auto_OksEngine.Components.hpp"));
						}
					}
				}

				File::CreateInfo fci{
					.isHpp_ = true,
					.includes_ = includes,
					.base_ = nullptr
				};

				auto categoryComponentsIncludeFile = std::make_shared<File>(fci);
				std::filesystem::path componentIncludesFullFilePath = componentIncludesFilePath / "auto_OksEngine.Components.hpp";

				std::string componentIncludesFullFilePathString = componentIncludesFullFilePath.string();
				std::replace(componentIncludesFullFilePathString.begin(), componentIncludesFullFilePathString.end(), '\\', '/');
				files.push_back({ std::filesystem::path{ componentIncludesFullFilePathString }, categoryComponentsIncludeFile });
			}



			//Generate OksEngine.Components.hpp files For Systems.
			for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

				if (componentsSystems.systems_.empty()) {
					continue;
				}
				//Generate components includes for components in the subdirectory 
				File::Includes includes{};
				std::filesystem::path systemsIncludesFilePath;
				for (auto systemParsedEcsFile : componentsSystems.systems_) {
					systemsIncludesFilePath = systemParsedEcsFile->GetPath().parent_path();
					//systemsIncludesFilePath = systemFullPath.parent_path();
					//std::filesystem::path systemIncludePath = GetSubPathFromFolderToEnd(systemFullPath.parent_path(), projectContext->includeDirectory_);

					const std::filesystem::path systemIncludePath = GetSubPath(
						systemParsedEcsFile->GetPath().parent_path(),
						projectContext->includeDirectory_,
						ResultRange::FromStartFolderToEnd,
						SearchDirection::FromEndToBegin,
						true);

					//systemIncludePath /= ;
					includes.paths_.push_back(systemIncludePath / ("auto_OksEngine." + systemParsedEcsFile->GetName() + ".hpp"));
				}

				//generate Include OksEngine.Components.hpp files from subdirectories.
				for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
					if (componentsSystems.systems_.empty()) {
						continue;
					}
					if (path.string().starts_with(categoryPath.string())) {
						if (path == categoryPath) {
							continue;
						}
						auto pathIt = path.begin();
						bool subdirFound = true;
						for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
							if (*it != *pathIt) {
								subdirFound = false;
								break;
							}
							++pathIt;
						}
						if (subdirFound) {
							//Subdirectory found!
							std::filesystem::path subdirPath;
							++pathIt;
							for (auto it = path.begin(); it != pathIt; ++it) {
								subdirPath /= *it;
							}
							//std::filesystem::path systemIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
							const std::filesystem::path systemIncludePath = GetSubPath(
								subdirPath,
								projectContext->includeDirectory_,
								ResultRange::FromStartFolderToEnd,
								SearchDirection::FromEndToBegin,
								true);
							//systemIncludePath ;
							includes.paths_.push_back(systemIncludePath / ("auto_OksEngine.Systems.hpp"));
						}
					}
				}

				File::CreateInfo fci{
					.isHpp_ = true,
					.includes_ = includes,
					.base_ = nullptr
				};

				auto categorySystemIncludeFile = std::make_shared<File>(fci);
				std::filesystem::path systemIncludesFullFilePath = systemsIncludesFilePath / "auto_OksEngine.Systems.hpp";

				std::string systemIncludesFullFilePathString = systemIncludesFullFilePath.string();
				std::replace(systemIncludesFullFilePathString.begin(), systemIncludesFullFilePathString.end(), '\\', '/');
				files.push_back({ std::filesystem::path{ systemIncludesFullFilePathString }, categorySystemIncludeFile });
			}

			return files;
		}

	private:
	};




}