#pragma once 

#include <ECSGenerator2.Common.hpp>
#include <ECSGenerator2.ProjectContext.hpp>
#include <ECSGenerator2.CodeFile.hpp>

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


namespace ECSGenerator2 {

	class CodeStructureGenerator {
	public:

		using System = std::string;
		using Component = std::string;

		class SystemsOrder {
		public:

			void AddSystemAfter(System system, System afterSystem) {
				for (Common::Index i = 0; i < order_.size(); i++) {
					const System& currentSystem = order_[i];
					if (currentSystem == afterSystem) {
						if (i != order_.size() - 1) {
							order_.insert(order_.begin() + i + 1, system);
						}
						else {
							order_.push_back(system);
						}
					}

				}

			}

			void AddSystemAfter(System system, std::unordered_set<System> afterSystems) {
				std::unordered_set<System> checkedSystems;
				for (Common::Index i = 0; i < order_.size(); i++) {
					const System& currentSystem = order_[i];
					if (afterSystems.contains(currentSystem)) {
						checkedSystems.insert(currentSystem);
						if (checkedSystems.size() == afterSystems.size()) {
							if (i != order_.size() - 1) {
								order_.insert(order_.begin() + i + 1, system);
							}
							else {
								order_.push_back(system);
							}
						}
					}

				}

			}

			void AddSystem(System system, std::unordered_set<System> afterSystems, std::unordered_set<System> beforeSystems) {

				auto findLastAfterSystem = [](
					const std::vector<System>& systems,
					const std::unordered_set<System>& afterSystems) {

						Common::UInt64 bestCandidate = 0;




					};

				auto findFirstBeforeSystem = [](
					const std::vector<System>& systems,
					const std::unordered_set<System>& afterSystems) {

					};

				std::unordered_set<System> checkedSystems;



				for (Common::Index i = 0; i < order_.size(); i++) {
					const System& currentSystem = order_[i];
					if (afterSystems.contains(currentSystem)) {
						checkedSystems.insert(currentSystem);
						if (checkedSystems.size() == afterSystems.size()) {
							if (i != order_.size() - 1) {
								order_.insert(order_.begin() + i + 1, system);
							}
							else {
								order_.push_back(system);
							}
						}
					}

				}

			}

			bool IsSystemAdded(System system) {
				for (Common::Index i = 0; i < order_.size(); i++) {
					const System& currentSystem = order_[i];
					if (currentSystem == system) {
						return true;
					}
				}
				return false;
			}

			std::vector<System> order_;
		};

		struct Thread {

			//std::map<System, std::set<Component>> systems_;
			std::vector<std::shared_ptr<ParsedSystem>> systems_;

			DS::Graph<System> callGraph_;
			SystemsOrder systemsOrder_;
		};


		Code GenerateRunSystemCode(std::shared_ptr<ParsedSystem> systemEcsFile) {
			Code runSystemCode;
			runSystemCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"" + systemEcsFile->GetName() + "\");");
			runSystemCode.Add(systemEcsFile->GetName() + "System(world2);");
			runSystemCode.NewLine();
			runSystemCode.Add("PIXEndEvent();");
			return runSystemCode;
		}


		std::shared_ptr<Function> GenerateRunInitSystemsFunctionRealization(std::vector<std::shared_ptr<ParsedSystem>> parsedSystems) {

			DS::Graph<System> initCallGraph;
			for (auto parsedSystem : parsedSystems) {
				if (parsedSystem->ci_.type_ == ParsedSystem::Type::Initialize) {
					DS::Graph<System>::Node::Id currentSystemNodeId = DS::Graph<System>::Node::invalidId_;
					if (!initCallGraph.IsNodeExist(parsedSystem->GetName())) {
						currentSystemNodeId = initCallGraph.AddNode(parsedSystem->GetName());
					}
					else {
						currentSystemNodeId = initCallGraph.FindNode(parsedSystem->GetName());
					}
					parsedSystem->ci_.callOrderInfo_->ForEachRunAfterSystem([&](const System& afterSystem) {
						DS::Graph<System>::Node::Id afterSystemNodeId = DS::Graph<System>::Node::invalidId_;
						if (!initCallGraph.IsNodeExist(afterSystem)) {
							afterSystemNodeId = initCallGraph.AddNode(afterSystem);
						}
						else {
							afterSystemNodeId = initCallGraph.FindNode(afterSystem);
						}
						initCallGraph.AddLinkFromTo(afterSystemNodeId, currentSystemNodeId);
						return true;
						});

					parsedSystem->ci_.callOrderInfo_->ForEachRunBeforeSystem([&](const System& beforeSystem) {

						DS::Graph<System>::Node::Id beforeSystemNodeId = DS::Graph<System>::Node::invalidId_;
						if (!initCallGraph.IsNodeExist(beforeSystem)) {
							beforeSystemNodeId = initCallGraph.AddNode(beforeSystem);
						}
						else {
							beforeSystemNodeId = initCallGraph.FindNode(beforeSystem);
						}
						initCallGraph.AddLinkFromTo(currentSystemNodeId, beforeSystemNodeId);
						return true;
						});
				}
			}

			//Find systems that root of dependence and generate code.
			auto findRoots = [&](DS::Graph<System>& graph) {
				std::unordered_set<DS::Graph<System>::NodeId> roots;
				graph.ForEachNode([&](
					DS::Graph<System>::NodeId systemNodeId,
					DS::Graph<System>::Node& systemNode
					) {
						if (!systemNode.HasLinksFrom()) {
							roots.insert(systemNodeId);
						}
						return true;
					});

				return roots;
				};

			std::unordered_set<DS::Graph<System>::Node::Id> roots = findRoots(initCallGraph);

			SystemsOrder systemsOrder;
			for (auto rootNodeId : roots) {
				auto rootNode = initCallGraph.GetNode(rootNodeId);
				systemsOrder.order_.push_back(rootNode.GetValue());
			}

			for (auto rootNodeId : roots) {
				auto rootNode = initCallGraph.GetNode(rootNodeId);
				if (rootNode.GetValue() == "System1") {
					//__debugbreak();
				}
				ProcessNode(systemsOrder, initCallGraph, rootNodeId);

			}

			Code runInitSystemsCode;
			runInitSystemsCode.Add(
				"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Start initialize frame\");"
				"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"StartFrame\");"
				"world2->StartFrame();"
				"PIXEndEvent();");

			auto getECSSystemByName = [](std::vector<std::shared_ptr<ParsedSystem>> parsedSystems, const std::string& systemName) {
				
				for (auto parsedSystem : parsedSystems) {
					if (parsedSystem->GetName() == systemName) {
						return parsedSystem;
					}
				}
				
				OS::AssertFail();

				return nullptr;
				
				};

			//Generate code to run systems that process all entities.
			for (auto& systemName : systemsOrder.order_) {
				auto parsedSystem = getECSSystemByName(parsedSystems, systemName);

				runInitSystemsCode.Add(GenerateRunSystemCode(parsedSystem));

				//Check if system creates new components, if yes we need to add them immideatly
				{
					bool isCreatesEntityComponent = false;
					parsedSystem->ci_.updateMethod_->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {
						if (!entity.creates_.empty()) {
							isCreatesEntityComponent = true;
							return false;
						}
						return true;
						});
					if (!isCreatesEntityComponent) {
						for (const auto& randomAccessEntity : parsedSystem->ci_.updateMethod_->randomAccessesEntities_) {
							if (!randomAccessEntity.creates_.empty()) {
								isCreatesEntityComponent = true;
								break;
							}
						}
					}
					isCreatesEntityComponent = isCreatesEntityComponent || !parsedSystem->ci_.updateMethod_->createsEntities_.empty();
					if (isCreatesEntityComponent) {
						runInitSystemsCode.Add("world2->ApplyDelayedRequests();");
					}
				}
			};

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

		std::shared_ptr<Function> GenerateCreateThreadRealization(const std::vector<Thread>& threads) {


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

					//Code runThreadSystems;
					//for (auto it = thread.systems_.begin(); it != thread.systems_.end(); it++) {
					//	const std::string systemName = it->first;
					//	runThreadSystems.Add(std::format(
					//		"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
					//		"{}System(world2);"
					//		"PIXEndEvent();",
					//		systemName,
					//		systemName
					//	));
					//}


					Code runThreadSystems;
					for (Common::Index i = 0; i < thread.systemsOrder_.order_.size(); i++) {
						runThreadSystems.Add(std::format(
							"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
							"{}System(world2);"
							"PIXEndEvent();",
							thread.systemsOrder_.order_[i],
							thread.systemsOrder_.order_[i]
						));
						if (i != thread.systemsOrder_.order_.size() - 1) {
							runThreadSystems.NewLine();
						}
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


		 std::shared_ptr<File> GenerateRunSystemsHppFile() {

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
			includes.paths_.insert("boost/asio/thread_pool.hpp");
			includes.paths_.insert("boost/asio/post.hpp");
			includes.paths_.insert("ECS2.World.hpp");


			//hpp file
			File::CreateInfo fci{
			.isHpp_ = true,
			.includes_ = includes,
			.base_ = namespaceObject
			};

			auto file = std::make_shared<File>(fci);

			return file;

			//auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			//std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			//std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.hpp");
			//FormatPath(systemCppFileFullPath);

			//return { systemCppFileFullPath, file };
		}

		std::shared_ptr<File> GenerateEditEntityHppFile(std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) {

			auto generateAddComponentCode = [](std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) -> Code {

				Code code;

				// Components to add list.
				code.Add("const char* items[] = {");
				code.NewLine();

				for (auto parsedComponent : parsedComponents) {
					code.Add(std::format("{}::GetName(),", parsedComponent->GetName()));
					code.NewLine();
				}

				code.Add("};");

				//ImGui combo box with components list.
				code.Add(
					"static int addComponentIndex = 0;"
					"ImGui::Combo(\"\", &addComponentIndex, items, std::size(items));"
					"ImGui::PushID(\"Add\");"
					"const std::string currentComponent = items[addComponentIndex];"
				);

				for (auto parsedComponent : parsedComponents) {
					code.Add(std::format("if (currentComponent == {}::GetName()) {{", parsedComponent->GetName()));
					code.Add(std::format("Add{}(world.get(), entityId);", parsedComponent->GetName()));
					code.Add("}");
					code.NewLine();
					};

				code.Add("ImGui::PopID();");

				return code;
			};

			auto generateEditComponentCode = [](std::shared_ptr<ParsedComponent> component) -> Code {

				Code code;
				code.Add(std::format("editComponent.template operator()<{}>(world, entityId);", component->GetName()));
				code.NewLine();
				component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {

					if (fieldInfo.GetTypeName() == "ECS2::Entity::Id") {
						code.Add(std::format("if (world->IsComponentExist<{}>(entityId)) {{", component->GetName()));
						code.Add(std::format("auto {} = world->GetComponent<{}>(entityId);",
							component->GetLowerName(), component->GetName()));
						code.Add("ImGui::Indent(20.0f);");
						code.Add(std::format("ImGui::PushID(\"{}\");", component->GetName()));
						code.Add(std::format("EditEntity(world, {}->{}_);", component->GetLowerName(), fieldInfo.GetName()));
						code.Add("ImGui::Unindent(20.0f);");
						code.Add("ImGui::PopID();");
						code.Add("}");
						code.NewLine();
					}

					return true;
					});
				return code;
				};

			auto generateEditEntityFunctionRealization = [&](std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) -> std::shared_ptr<Function> {

				Code code;

				code.Add(
					"const std::string idString = std::to_string(entityId);"
					"ImGui::PushID(idString.c_str());"
					"std::string name;"
					"if (world->IsComponentExist<Name>(entityId)) {"
					"auto* nameComponent = world->GetComponent<Name>(entityId);"
					"name = nameComponent->value_;"
					"}"
					"if (ImGui::CollapsingHeader((\"Id: \" + idString + \"  \"\" + name + \"\" \" + magic_enum::enum_name(world->GetEntityType(entityId)).data()).c_str())) {"
					"ImGui::Indent(20.f);"

					"auto editComponent = []<class ComponentType>(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) {"

					"bool isExist = world->IsComponentExist<ComponentType>(id);"
					"if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist)) {"
					"ComponentType* component = world->GetComponent<ComponentType>(id);"
					"Edit<ComponentType>(component);"
					"ImGui::Spacing();"
					"}"
					"if (!isExist) {"
					"if (world->IsComponentExist<ComponentType>(id)) {"
					"world->RemoveComponent<ComponentType>(id);"
					"}"
					"}"
					"};"
					"{"
					"ImGui::PushID(\"Edit\");");


				//Generate edit components.
				for (auto parsedComponent : parsedComponents) {

					code.Add(generateEditComponentCode(parsedComponent));
					code.NewLine();
					};
				code.Add("ImGui::PopID();");
				code.Add("ImGui::SeparatorText(\"Add component\");");
				code.Add("ImGui::Indent(20.0f);");

				code.Add(generateAddComponentCode(parsedComponents));
				code.Add("ImGui::Unindent(20.0f);");

				code.Add("}");
				code.Add("ImGui::Separator();");
				code.Add("ImGui::Unindent(20.f);");
				code.Add("}");
				code.Add("ImGui::PopID();");

				Function::CreateInfo editEntityFunction{
					.name_ = "EditEntity",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world" },
						{ "ECS2::Entity::Id", "entityId" }
					},
					.returnType_ = "void",
					.code_ = code,
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto editEntityFunctionObject = std::make_shared<Function>(editEntityFunction);

				return editEntityFunctionObject;
				};

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");

			namespaceObject->Add(generateEditEntityFunctionRealization(parsedComponents));

			File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("imgui.h");
			includes.paths_.insert("OksEngine.ECS.hpp");
			includes.paths_.insert("magic_enum/magic_enum.hpp");

			//hpp file
			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<File>(fci);

			return file;

			//auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			//std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			//std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.EditEntity.hpp");
			//FormatPath(systemCppFileFullPath);

			//return { systemCppFileFullPath, file };
		}




		void ProcessNode(
			SystemsOrder& systemsOrder,
			const DS::Graph<System>& graph,
			const DS::Graph<System>::Node::Id& nodeId,
			bool isFrom = false) {

			auto& node = graph.GetNode(nodeId);
			if (node.HasLinks()) {
				isFrom = isFrom;
			}
			if (node.GetValue() == "StartMainMenuBar") {
				isFrom = isFrom;
			}
			if (node.GetValue() == "EndWorldSceneSaving") {
				isFrom = isFrom;
			}
			std::unordered_set<System> systemsFrom;
			node.ForEachLinksFrom([&](DS::Graph<System>::Node::Id fromNodeToId) {
				auto& fromNode = graph.GetNode(fromNodeToId);
				if (!systemsOrder.IsSystemAdded(fromNode.GetValue())) {
					ProcessNode(systemsOrder, graph, fromNodeToId, true);
				}
				systemsFrom.insert(fromNode.GetValue());
				return true;
				});

			if (!systemsFrom.empty() && !systemsOrder.IsSystemAdded(node.GetValue())) {
				systemsOrder.AddSystemAfter(node.GetValue(), systemsFrom);
			}
			if (!isFrom) {
				node.ForEachLinksTo([&](DS::Graph<System>::Node::Id nodeToId) {
					ProcessNode(systemsOrder, graph, nodeToId);
					return true;
					});
			}

		}

		void CalculateSystemsCallOrder(Thread& thread) {

			//Find systems that root of dependence and generate code.
			auto findRoots = [](const Thread& thread) {
				std::unordered_set<DS::Graph<System>::Node::Id> roots;
				thread.callGraph_.ForEachNode([&](
					const DS::Graph<System>::Node::Id systemNodeId,
					const DS::Graph<System>::Node& systemNode
					) {
						if (!systemNode.HasLinksFrom()) {
							roots.insert(systemNodeId);
						}
						return true;
					});
				return roots;
				};

			std::unordered_set<DS::Graph<System>::Node::Id> roots = findRoots(thread);

			SystemsOrder systemsOrder;
			for (auto rootNodeId : roots) {
				auto rootNode = thread.callGraph_.GetNode(rootNodeId);
				thread.systemsOrder_.order_.push_back(rootNode.GetValue());
			}

			for (auto rootNodeId : roots) {
				auto rootNode = thread.callGraph_.GetNode(rootNodeId);
				ProcessNode(thread.systemsOrder_, thread.callGraph_, rootNodeId);

			}

		}

		std::pair<std::filesystem::path, std::shared_ptr<File>>
			GenerateRunSystemsCppFile(/*std::vector<std::vector<Agnode_t*>> clusters,*/ std::vector<std::shared_ptr<ParsedECSFile>> parsedECSFiles) {

			//Components and which systems uses them.
			std::map<System, std::set<Component>> componentSystem;
			{
				for (auto parsedECSFile : parsedECSFiles) {
					parsedECSFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {
						
						const std::string systemName = parsedSystem->GetName();
						for (auto& entity : parsedSystem->ci_.updateMethod_->processesEntities_) {
							entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
								if (componentSystem.find(include.name_) != componentSystem.end()) {
									componentSystem.insert({});
								}
								componentSystem[include.name_].insert(systemName);
								return true;
								});
						}
						
						});
				}
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

			Thread childThread;
			Thread mainThread;
			for (auto parsedECSFile : parsedECSFiles) {
				parsedECSFile->ForEachSystem(
					[&](std::shared_ptr<ParsedSystem> parsedSystem) {
						if (parsedSystem->GetThread() == ParsedSystem::Thread::Child) {
							childThread.systems_.push_back(parsedSystem);
						}
						if (parsedSystem->GetThread() == ParsedSystem::Thread::Main) {
							mainThread.systems_.push_back(parsedSystem);
						}
					});
			}


			//{
			//	for (auto& clusterSystems : clusters) {
			//		bool isMainThread = false;
			//		for (Agnode_t* systemNode : clusterSystems) {
			//			std::string systemName = agnameof(systemNode);
			//			const auto ecsFile = projectContext->GetEcsFileByName(systemName);

			//			auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystem>(ecsFile);
			//			if (systemEcsFile->IsInitializeSystem()) {
			//				continue;
			//			}
			//			if (systemEcsFile->ci_.thread_ == ParsedSystem::Thread::Main) {
			//				isMainThread = true;
			//			}
			//			//projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile) {
			//			//	//if ("OksEngine.ProcessInput.ecs" == ecsFile->GetPath().filename()) {
			//			//	//	//__debugbreak();
			//			//	//}
			//			//	
			//			//	if (systemName == systemEcsFile->GetName() && systemEcsFile->ci_.thread_ == ParsedSystemECSFile::Thread::Main) {
			//			//		
			//			//		return false;
			//			//	}
			//			//	return true;
			//			//	});

			//		}
			//		Thread clusterThread;
			//		std::map<std::string, DS::Graph<std::string>::Node::Id> systemNameToNode;
			//		for (Agnode_t* systemNode : clusterSystems) {
			//			std::string systemName = agnameof(systemNode);

			//			clusterThread.systems_[systemName] = componentSystem[systemName];




			//		}
			//		if (isMainThread) {
			//			/*				mainThread.systems_.insert(
			//								clusterThread.systems_.begin(),
			//								clusterThread.systems_.end());*/
			//			mainThread = clusterThread;

			//		}
			//		else {
			//			childThreads.push_back(clusterThread);
			//		}
			//	}
			//}


			//Create call graph for each thread.
			auto createClusterSystemsCallGraph =
				[&](Thread& thread) {

				for (auto system : thread.systems_) {

					auto getCreateSystemNode = [](DS::Graph<System>& graph, const System& system) {
						DS::Graph<System>::Node::Id nodeId = DS::Graph<System>::Node::invalidId_;
						if (!graph.IsNodeExist(system)) {
							nodeId = graph.AddNode(system);
						} else {
							nodeId = graph.FindNode(system);
						}
						return nodeId;
						};

					DS::Graph<System>::Node::Id currentSystemNodeId = getCreateSystemNode(thread.callGraph_, system->GetName());

					system->ci_.callOrderInfo_->ForEachRunAfterSystem([&](const std::string& afterSystem) {
#pragma region Assert 
						OS::AssertMessage(thread.systems_.contains(afterSystem), "Current thread doesn't contain After System:" + afterSystem);
#pragma endregion		
						DS::Graph<System>::Node::Id afterSystemNodeId = getCreateSystemNode(thread.callGraph_, afterSystem);
						thread.callGraph_.AddLinkFromTo(afterSystemNodeId, currentSystemNodeId);
						return true;
						});

					system->ci_.callOrderInfo_->ForEachRunBeforeSystem([&](const std::string& beforeSystem) {
#pragma region Assert 
						OS::AssertMessage(thread.systems_.contains(beforeSystem), "Current thread doesn't contain Before System:" + beforeSystem);
#pragma endregion		
						DS::Graph<System>::Node::Id beforeSystemNodeId = getCreateSystemNode(thread.callGraph_, beforeSystem);
						thread.callGraph_.AddLinkFromTo(currentSystemNodeId, beforeSystemNodeId);
						return true;
						});

				}

				///CREATE GRAPHVIZ CALL GRAPH

				// �������� ������ �����
			/*	Agraph_t* g = agopen((char*)"G", Agstrictdirected, nullptr);

				thread.callGraph_.ForEachNode([&](DS::Graph<System>::NodeId nodeId, DS::Graph<System>::Node& node) {

					if (node.HasLinksFrom() || node.HasLinksTo()) {

						Agnode_t* gSystemNode = agnode(g, (char*)node.GetValue().c_str(), 1);
						agsafeset(gSystemNode, (char*)"shape", (char*)"rect", (char*)"");

						node.ForEachLinksFrom([&](DS::Graph<System>::NodeId nodeId) {
							const DS::Graph<System>::Node& fromNode = thread.callGraph_.GetNode(nodeId);

							Agnode_t* gFromNode = agnode(g, (char*)fromNode.GetValue().c_str(), 1);

							Agedge_t* gEdge = agedge(g, gFromNode, gSystemNode, nullptr, 1);

							return true;
							});

						node.ForEachLinksTo([&](DS::Graph<System>::NodeId nodeId) {
							const DS::Graph<System>::Node& toNode = thread.callGraph_.GetNode(nodeId);

							Agnode_t* gToNode = agnode(g, (char*)toNode.GetValue().c_str(), 1);

							Agedge_t* gEdge = agedge(g, gSystemNode, gToNode, nullptr, 1);

							return true;
							});
					}


					return true;
					});*/


				//Parse .dot
				//{
				//	GVC_t* gvc = gvContext();


				//	//Get path
				//	auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

				//	std::filesystem::path includeDirFullPath;

				//	std::filesystem::path::iterator includeDirIt;
				//	for (auto it = randomEcsFilePath.end(); it != randomEcsFilePath.begin(); --it) {
				//		auto folder = *it;
				//		if (folder == projectContext->includeDirectory_) {
				//			includeDirIt = it;
				//			break;
				//		}
				//	}

				//	for (auto it = randomEcsFilePath.begin(); it != includeDirIt; it++) {
				//		includeDirFullPath /= *it;
				//	}

				//	includeDirFullPath /= *includeDirIt;
				//	//Get path


				//	auto dotfile = std::make_shared<OS::TextFile>(includeDirFullPath / "auto_ECSSystemsCallGraph.dot");

				//	char* dotData = nullptr;
				//	unsigned int length = 0;

				//	gvLayout(gvc, g, "dot");
				//	gvRenderData(gvc, g, "dot", &dotData, &length);

				//	agclose(g);
				//	gvFreeContext(gvc);
				//	dotfile->Create();
				//	std::string dotText{ dotData };
				//	(*dotfile) << dotText;
				//	dotfile->Close();
				//}
				//////

				};

			for (auto& childThread : childThreads) {
				createClusterSystemsCallGraph(childThread);
				CalculateSystemsCallOrder(childThread);
			}
			createClusterSystemsCallGraph(mainThread);
			CalculateSystemsCallOrder(mainThread);


			//Group threads systems to get less threads.
			{
				const Common::UInt64 threadsNumber = 10;// projectContext->GetConfig()->GetValueAs<Common::UInt64>("Multithreading.threadsNumber");

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
				cppIncludes.paths_.insert("auto_OksEngine.RunSystems.hpp");

				for (auto ecsFile : parsedECSFiles) {
					ecsFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {
						
						std::filesystem::path systemIncludePath = GetSubPath(
							ecsFile->GetPath().parent_path(),
							projectContext->includeDirectory_,
							ResultRange::FromStartFolderToEnd,
							SearchDirection::FromEndToBegin,
							true);
						cppIncludes.paths_.insert(systemIncludePath / ("auto_OksEngine." + parsedSystem->GetName() + ".hpp"));
						
						});
				}


	
				cppIncludes.paths_.insert("pix3.h");
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
					for (Common::Index i = 0; i < mainThread.systemsOrder_.order_.size(); i++) {
						runMainThreadSystems.Add(std::format(
							"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
							"{}System(world2);"
							"PIXEndEvent();",
							mainThread.systemsOrder_.order_[i],
							mainThread.systemsOrder_.order_[i]
						));
						if (i != mainThread.systemsOrder_.order_.size() - 1) {
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

				////Generate code to run systems that process all entities.
				//projectContext->ForEachSystemEcsFile(
				//	[&](std::shared_ptr<ParsedECSFile> ecsFile) {
				//		auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
				//		if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::AllEntities) {

				//			cppRunSystemsCode.Add(GenerateRunSystemCode(systemEcsFile));
				//		}
				//		return true;
				//	});

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

			std::vector<std::shared_ptr<ParsedSystem>> parsedSystems;
			for (auto parsedECSFile : parsedECSFiles) {
				parsedECSFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {
					parsedSystems.push_back(parsedSystem);
				});
			}

			cppNamespaceObject->Add(GenerateRunInitSystemsFunctionRealization(parsedSystems));
			cppNamespaceObject->Add(GenerateCreateThreadRealization(childThreads));


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

			return {
				GenerateRunSystemsHppFile(projectContext),
				GenerateRunSystemsCppFile(clusters, projectContext)
			};
		}


		//std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext) {

		//	std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> files;


		//	ComponentFileStructureGenerator::CreateInfo ci{
		//		.includeDirectory_ = projectContext->includeDirectory_
		//	};

		//	//Generate auto_OksEngine.{ComponentName}.hpp
		//	projectContext->ForEachComponentEcsFile(
		//		[&](std::shared_ptr<ParsedECSFile> ecsFile, bool isLast) {
		//			auto componentEcsFile = std::dynamic_pointer_cast<ParsedComponent>(ecsFile);
		//			ComponentFileStructureGenerator generator{ ci };
		//			auto file = generator.GenerateECSCXXFilesStructure(projectContext, componentEcsFile);
		//			files.push_back(file);
		//			return true;
		//		});

		//	//Generate auto_OksEngine.{SystemName}.hpp
		//	projectContext->ForEachSystemEcsFile(
		//		[&](std::shared_ptr<ParsedECSFile> ecsFile) {
		//			auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystem>(ecsFile);
		//			SystemFileStructureGenerator::CreateInfo ci{
		//				.includeDirectory_ = projectContext->includeDirectory_
		//			};
		//			SystemFileStructureGenerator generator{ ci };
		//			auto file = generator.GenerateECSCXXFilesStructure(projectContext, systemEcsFile);
		//			files.push_back(file);
		//			return true;
		//		});
		//	//Generate OksEngine.ECS.hpp files For ECS.
		//	ProcessDirectory(projectContext->ci_.workDirs_[0], projectContext);
		//	//
		//	////Generate OksEngine.Components.hpp files For COMPONENTS.
		//	//for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

		//	//	//if (componentsSystems.components_.empty()) {
		//	//	//	continue;
		//	//	//}

		//	//	if (categoryPath == "D:/OksEngine/Tools/ECSGenerator/../../Sources/Engine/Sources\\Debug\\ECS\\ECSEditor\\Resources\\LoadECSFiles") {
		//	//		//__debugbreak();
		//	//	}

		//	//	//Generate components includes for components in the subdirectory 
		//	//	File::Includes includes{};

		//	//	for (auto componentParsedEcsFile : componentsSystems.components_) {
		//	//		auto componentPath = componentParsedEcsFile->GetPath().parent_path();
		//	//		const std::filesystem::path includePath = GetSubPath(
		//	//			componentParsedEcsFile->GetPath().parent_path(),
		//	//			projectContext->includeDirectory_,
		//	//			ResultRange::FromStartFolderToEnd,
		//	//			SearchDirection::FromEndToBegin,
		//	//			true);
		//	//		includes.paths_.insert(includePath / ("auto_OksEngine." + componentParsedEcsFile->GetName() + ".hpp"));
		//	//	}

		//	//	//generate Include OksEngine.Components.hpp files from subdirectories.
		//	//	for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
		//	//		if (path.string().starts_with(categoryPath.string())) {
		//	//			if (path == categoryPath) {
		//	//				continue;
		//	//			}
		//	//			auto pathIt = path.begin();
		//	//			bool subdirFound = true;
		//	//			for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
		//	//				if (*it != *pathIt) {
		//	//					subdirFound = false;
		//	//					break;
		//	//				}
		//	//				++pathIt;
		//	//			}

		//	//			if (subdirFound) {
		//	//				//Subdirectory found!
		//	//				std::filesystem::path subdirPath;
		//	//				++pathIt;
		//	//				for (auto it = path.begin(); it != pathIt; ++it) {
		//	//					subdirPath /= *it;
		//	//				}
		//	//				//std::filesystem::path componentIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
		//	//				const std::filesystem::path componentIncludePath = GetSubPath(
		//	//					subdirPath,
		//	//					projectContext->includeDirectory_,
		//	//					ResultRange::FromStartFolderToEnd,
		//	//					SearchDirection::FromEndToBegin,
		//	//					true);
		//	//				//componentIncludePath ;
		//	//				includes.paths_.insert(componentIncludePath / ("auto_OksEngine.Components.hpp"));
		//	//			}
		//	//		}
		//	//	}

		//	//	File::CreateInfo fci{
		//	//		.isHpp_ = true,
		//	//		.includes_ = includes,
		//	//		.base_ = nullptr
		//	//	};

		//	//	auto categoryComponentsIncludeFile = std::make_shared<File>(fci);
		//	//	std::filesystem::path componentIncludesFullFilePath = categoryPath / "auto_OksEngine.Components.hpp";

		//	//	std::string componentIncludesFullFilePathString = componentIncludesFullFilePath.string();
		//	//	std::replace(componentIncludesFullFilePathString.begin(), componentIncludesFullFilePathString.end(), '\\', '/');
		//	//	files.push_back({ std::filesystem::path{ componentIncludesFullFilePathString }, categoryComponentsIncludeFile });
		//	//}



		//	////Generate OksEngine.Components.hpp files For Systems.
		//	//for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

		//	//	//if (componentsSystems.systems_.empty()) {
		//	//	//	continue;
		//	//	//}
		//	//	//Generate components includes for components in the subdirectory 
		//	//	File::Includes includes{};
		//	//	std::filesystem::path systemsIncludesFilePath;
		//	//	for (auto systemParsedEcsFile : componentsSystems.systems_) {
		//	//		systemsIncludesFilePath = systemParsedEcsFile->GetPath().parent_path();
		//	//		const std::filesystem::path systemIncludePath = GetSubPath(
		//	//			systemParsedEcsFile->GetPath().parent_path(),
		//	//			projectContext->includeDirectory_,
		//	//			ResultRange::FromStartFolderToEnd,
		//	//			SearchDirection::FromEndToBegin,
		//	//			true);

		//	//		includes.paths_.insert(systemIncludePath / ("auto_OksEngine." + systemParsedEcsFile->GetName() + ".hpp"));
		//	//	}

		//	//	//generate Include OksEngine.Systems.hpp files from subdirectories.
		//	//	for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
		//	//		if (componentsSystems.systems_.empty()) {
		//	//			continue;
		//	//		}
		//	//		if (path.string().starts_with(categoryPath.string())) {
		//	//			if (path == categoryPath) {
		//	//				continue;
		//	//			}
		//	//			auto pathIt = path.begin();
		//	//			bool subdirFound = true;
		//	//			for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
		//	//				if (*it != *pathIt) {
		//	//					subdirFound = false;
		//	//					break;
		//	//				}
		//	//				++pathIt;
		//	//			}
		//	//			if (subdirFound) {
		//	//				//Subdirectory found!
		//	//				std::filesystem::path subdirPath;
		//	//				++pathIt;
		//	//				for (auto it = path.begin(); it != pathIt; ++it) {
		//	//					subdirPath /= *it;
		//	//				}
		//	//				//std::filesystem::path systemIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
		//	//				const std::filesystem::path systemIncludePath = GetSubPath(
		//	//					subdirPath,
		//	//					projectContext->includeDirectory_,
		//	//					ResultRange::FromStartFolderToEnd,
		//	//					SearchDirection::FromEndToBegin,
		//	//					true);
		//	//				//systemIncludePath ;
		//	//				includes.paths_.insert(systemIncludePath / ("auto_OksEngine.Systems.hpp"));
		//	//			}
		//	//		}
		//	//	}

		//	//	File::CreateInfo fci{
		//	//		.isHpp_ = true,
		//	//		.includes_ = includes,
		//	//		.base_ = nullptr
		//	//	};

		//	//	auto categorySystemIncludeFile = std::make_shared<File>(fci);
		//	//	std::filesystem::path systemIncludesFullFilePath = systemsIncludesFilePath / "auto_OksEngine.Systems.hpp";

		//	//	std::string systemIncludesFullFilePathString = systemIncludesFullFilePath.string();
		//	//	std::replace(systemIncludesFullFilePathString.begin(), systemIncludesFullFilePathString.end(), '\\', '/');
		//	//	files.push_back({ std::filesystem::path{ systemIncludesFullFilePathString }, categorySystemIncludeFile });
		//	//}

		//	return files;
		//}

	private:
	};




}