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
			std::map<System, std::set<Component>> systems_;

			DS::Graph<System> callGraph_;
			SystemsOrder systemsOrder_;
		};


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

			DS::Graph<System> initCallGraph;
			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedECSFile> ecsFile) {
					auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
					if (systemEcsFile->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize) {
						DS::Graph<System>::Node::Id currentSystemNodeId = DS::Graph<System>::Node::invalidId_;
						if (!initCallGraph.IsNodeExist(systemEcsFile->GetName())) {
							currentSystemNodeId = initCallGraph.AddNode(systemEcsFile->GetName());
						}
						else {
							currentSystemNodeId = initCallGraph.FindNode(systemEcsFile->GetName());
						}
						systemEcsFile->ForEachRunAfterSystem([&](const System& afterSystem) {
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

						systemEcsFile->ForEachRunBeforeSystem([&](const System& beforeSystem) {

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
					return true;
				});





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

			//Generate code to run systems that process all entities.
			for (auto& systemName : systemsOrder.order_) {
				auto ecsFile = projectContext->GetEcsFileByName(systemName);
				auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
				runInitSystemsCode.Add(GenerateRunSystemCode(systemEcsFile));

				//Check if system creates new components, if yes we need to add them immideatly
				{
					bool isCreatesEntityComponent = false;
					systemEcsFile->ForEachRequestEntity([&](const ParsedSystemECSFile::RequestEntity& entity, bool isLast) {
						if (!entity.creates_.empty()) {
							isCreatesEntityComponent = true;
							return false;
						}
						return true;
						});
					if (!isCreatesEntityComponent) {
						for (const auto& randomAccessEntity : systemEcsFile->ci_.accessesEntities_) {
							if (!randomAccessEntity.creates_.empty()) {
								isCreatesEntityComponent = true;
								break;
							}
						}
					}
					isCreatesEntityComponent = isCreatesEntityComponent || !systemEcsFile->ci_.createsEntities_.empty();
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



			//Generate files Paths

			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.RunSystems.hpp");
			FormatPath(systemCppFileFullPath);

			return { systemCppFileFullPath, file };
		}

		std::pair<std::filesystem::path, std::shared_ptr<File>>
			GenerateEditEntityHppFile(std::shared_ptr<ProjectContext> projectContext) {

			auto generateAddComponentCode = [](std::shared_ptr<ProjectContext> projectContext) -> Code {

				Code code;

				// Components to add list.
				code.Add("const char* items[] = {");
				code.NewLine();

				projectContext->ForEachComponentEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile, bool isLast) {

					auto component = std::dynamic_pointer_cast<ParsedComponentECSFile>(ecsFile);
					code.Add(std::format("{}::GetName(),", component->GetName()));
					code.NewLine();
					return true;
					});

				code.Add("};");

				//ImGui combo box with components list.
				code.Add(
					"static int addComponentIndex = 0;"
					"ImGui::Combo(\"\", &addComponentIndex, items, std::size(items));"
					"ImGui::PushID(\"Add\");"
					"const std::string currentComponent = items[addComponentIndex];"
				);

				projectContext->ForEachComponentEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile, bool isLast) {

					auto component = std::dynamic_pointer_cast<ParsedComponentECSFile>(ecsFile);
					code.Add(std::format("if (currentComponent == {}::GetName()) {{", component->GetName()));
					code.Add(std::format("Add{}(world.get(), entityId);", component->GetName()));
					code.Add("}");
					code.NewLine();
					return true;
					});
				code.Add("ImGui::PopID();");
				return code;
				};

			auto generateEditComponentCode = [](std::shared_ptr<ParsedComponentECSFile> component) -> Code {

				Code code;
				code.Add(std::format("editComponent.template operator()<{}>(world, entityId);", component->GetName()));
				code.NewLine();
				component->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {

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

			auto generateEditEntityFunctionRealization = [&](std::shared_ptr<ProjectContext> projectContext) -> std::shared_ptr<Function> {

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
				projectContext->ForEachComponentEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile, bool isLast) {

					auto component = std::dynamic_pointer_cast<ParsedComponentECSFile>(ecsFile);

					code.Add(generateEditComponentCode(component));
					code.NewLine();
					return true;
					});
				code.Add("ImGui::PopID();");
				code.Add("ImGui::SeparatorText(\"Add component\");");
				code.Add("ImGui::Indent(20.0f);");

				code.Add(generateAddComponentCode(projectContext));
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

			namespaceObject->Add(generateEditEntityFunctionRealization(projectContext));

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

			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.EditEntity.hpp");
			FormatPath(systemCppFileFullPath);

			return { systemCppFileFullPath, file };
		}


		std::pair<std::filesystem::path, std::shared_ptr<File>>
			GenerateParseEntityHppFile(std::shared_ptr<ProjectContext> projectContext) {

			auto generateParseComponentCode = [](std::shared_ptr<ParsedComponentECSFile> component) -> Code {

				Code code;

				//luabridge::LuaRef position2DRef = entity["position2D"];
				code.Add(std::format(
					"luabridge::LuaRef {}Ref = entity[\"{}\"];",
					component->GetLowerName(),
					component->GetLowerName()));

				//if (!position2DRef.isNil()) {
				code.Add(std::format(
					"if (!{}Ref.isNil()) {{",
					component->GetLowerName()));

				//Position2D position2D = ParsePosition2D(position2DRef);
				code.Add(std::format("{} {} = Parse{}({}Ref);",
					component->GetName(),
					component->GetLowerName(),
					component->GetName(),
					component->GetLowerName()));

				//CreateComponent<Position2D>(newEntityId
				code.Add(std::format("world->CreateComponent<{}>(newEntityId",
					component->GetName()));

				//, position2D.x_, position2D.y_
				component->ForEachField([&](const ParsedComponentECSFile::FieldInfo& fieldInfo, bool isLast) {

					code.Comma();

					if (fieldInfo.typeName_ == "ECS2::Entity::Id") {
						code.Add(std::format("getNewId({}.{}_)",
							component->GetLowerName(),
							fieldInfo.GetName()));
					}
					else {
						code.Add(std::format("{}.{}_",
							component->GetLowerName(),
							fieldInfo.GetName()));
					}
					return true;
					});

				code.Add(");");
				code.Add("}");

				return code;
				};

			auto generateParseEntityFunctionRealization = [&](std::shared_ptr<ProjectContext> projectContext) -> std::shared_ptr<Function> {

				Code code;
				code.Add(
					"auto getNewId = [&](ECS2::Entity::Id oldId) {"
					"	#pragma region Assert\n"
					"	OS::AssertMessage(oldToNewId.contains(oldId),\n"
					"	std::format(\"Invalid scene file. Can not define all entities ids: {}.\", static_cast<Common::Index>(oldId)));\n"
					"	#pragma endregion\n"
					"	return oldToNewId.at(oldId);\n"
					"};");

				//Generate edit components.
				projectContext->ForEachComponentEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile, bool isLast) {

					auto component = std::dynamic_pointer_cast<ParsedComponentECSFile>(ecsFile);

					if (!component->serializable_) {
						return true;
					}

					code.Add("{");
					code.Add(generateParseComponentCode(component));
					code.Add("}");

					code.NewLine();

					return true;
					});

				Function::CreateInfo editEntityFunction{
					.name_ = "ParseEntity",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world" },
						{ "luabridge::LuaRef", "entity" },
						{ "ECS2::Entity::Id", "newEntityId" },
						{ "const std::map<ECS2::Entity::Id, ECS2::Entity::Id>&", "oldToNewId" }
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

			namespaceObject->Add(generateParseEntityFunctionRealization(projectContext));

			File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("OksEngine.ECS.hpp");
			//includes.paths_.insert("magic_enum/magic_enum.hpp");

			//hpp file
			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<File>(fci);

			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.ParseEntity.hpp");
			FormatPath(systemCppFileFullPath);

			return { systemCppFileFullPath, file };
		}

		std::pair<std::filesystem::path, std::shared_ptr<File>>
			GenerateSerializeEntityHppFile(std::shared_ptr<ProjectContext> projectContext) {

			auto generateSerializeComponentCode = [](std::shared_ptr<ParsedComponentECSFile> component, bool isLast) -> Code {

				Code code;

				code.Add(std::format(
					"if (components.IsSet<{}>()) {{"
					"	const auto* component = world->GetComponent<{}>(entityId);"
					"	serializedEntity += \"\\t\\t\" +Serialize{}(component);",
					component->GetName(),
					component->GetName(),
					component->GetName()));

				//if (!isLast) {
					code.Add("	serializedEntity += \",\\n\";");
				//}

				code.Add("}");

				return code;
				};

			auto generateSerializeEntityFunctionRealization = [&](std::shared_ptr<ProjectContext> projectContext) -> std::shared_ptr<Function> {

				Code code;

				code.Add("std::string serializedEntity;");
				code.Add("const ECS2::ComponentsFilter components = world->GetEntityComponentsFilter(entityId);");
				code.Add("serializedEntity += \"\\t\\tID = \" + std::to_string(entityId) + \",\\n\";");
				//Generate serialize components.
				projectContext->ForEachComponentEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile, bool isLast) {

					auto component = std::dynamic_pointer_cast<ParsedComponentECSFile>(ecsFile);

					if (!component->serializable_) {
						return true;
					}
					code.Add("{");
					code.Add(generateSerializeComponentCode(component, isLast));
					code.Add("}");

					code.NewLine();

					return true;
					});

				code.Add("return serializedEntity;");

				Function::CreateInfo serializeEntityFunction{
					.name_ = "SerializeEntity",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world" },
						{ "ECS2::Entity::Id", "entityId" }
					},
					.returnType_ = "std::string",
					.code_ = code,
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto serializedEntityFunctionObject = std::make_shared<Function>(serializeEntityFunction);

				return serializedEntityFunctionObject;
				};

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");

			namespaceObject->Add(generateSerializeEntityFunctionRealization(projectContext));

			File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("OksEngine.ECS.hpp");
			//includes.paths_.insert("magic_enum/magic_enum.hpp");

			//hpp file
			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<File>(fci);

			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.SerializeEntity.hpp");
			FormatPath(systemCppFileFullPath);

			return { systemCppFileFullPath, file };
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
				if (rootNode.GetValue() == "System1") {
					//__debugbreak();
				}
				ProcessNode(thread.systemsOrder_, thread.callGraph_, rootNodeId);

			}

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
						return true;
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
						const auto ecsFile = projectContext->GetEcsFileByName(systemName);

						auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
						if (systemEcsFile->IsInitializeSystem()) {
							continue;
						}
						if (systemEcsFile->ci_.thread_ == ParsedSystemECSFile::Thread::Main) {
							isMainThread = true;
						}
						//projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedECSFile> ecsFile) {
						//	//if ("OksEngine.ProcessInput.ecs" == ecsFile->GetPath().filename()) {
						//	//	//__debugbreak();
						//	//}
						//	
						//	if (systemName == systemEcsFile->GetName() && systemEcsFile->ci_.thread_ == ParsedSystemECSFile::Thread::Main) {
						//		
						//		return false;
						//	}
						//	return true;
						//	});

					}
					Thread clusterThread;
					std::map<std::string, DS::Graph<std::string>::Node::Id> systemNameToNode;
					for (Agnode_t* systemNode : clusterSystems) {
						std::string systemName = agnameof(systemNode);

						clusterThread.systems_[systemName] = componentSystem[systemName];




					}
					if (isMainThread) {
						/*				mainThread.systems_.insert(
											clusterThread.systems_.begin(),
											clusterThread.systems_.end());*/
						mainThread = clusterThread;

					}
					else {
						childThreads.push_back(clusterThread);
					}
				}
			}


			//Create call graph for each thread.
			auto createClusterSystemsCallGraph =
				[&](std::shared_ptr<ProjectContext> projectContext, Thread& thread) {

				for (auto systemName : thread.systems_) {

					auto getCreateSystemNode = [](DS::Graph<System>& graph, const System& system) {
						DS::Graph<System>::Node::Id nodeId = DS::Graph<System>::Node::invalidId_;
						if (!graph.IsNodeExist(system)) {
							nodeId = graph.AddNode(system);
						}
						else {
							nodeId = graph.FindNode(system);
						}
						return nodeId;
						};

					DS::Graph<System>::Node::Id currentSystemNodeId = getCreateSystemNode(thread.callGraph_, systemName.first);


					auto ecsFile = projectContext->GetEcsFileByName(systemName.first);
					auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
					if (systemName.first == "EndDrawingMainMenuBarECSItem") {
						currentSystemNodeId = currentSystemNodeId;
					}
					systemEcsFile->ForEachRunAfterSystem([&](const std::string& afterSystem) {
#pragma region Assert 
						OS::AssertMessage(thread.systems_.contains(afterSystem), "");
#pragma endregion		
						DS::Graph<System>::Node::Id afterSystemNodeId = getCreateSystemNode(thread.callGraph_, afterSystem);
						thread.callGraph_.AddLinkFromTo(afterSystemNodeId, currentSystemNodeId);
						return true;
						});

					systemEcsFile->ForEachRunBeforeSystem([&](const std::string& beforeSystem) {
#pragma region Assert 
						OS::AssertMessage(thread.systems_.contains(beforeSystem), "");
#pragma endregion		
						DS::Graph<System>::Node::Id beforeSystemNodeId = getCreateSystemNode(thread.callGraph_, beforeSystem);
						thread.callGraph_.AddLinkFromTo(currentSystemNodeId, beforeSystemNodeId);
						return true;
						});

				}

				///CREATE GRAPHVIZ CALL GRAPH

				// �������� ������ �����
				Agraph_t* g = agopen((char*)"G", Agstrictdirected, nullptr);

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
					});


				//Parse .dot
				{
					GVC_t* gvc = gvContext();


					//Get path
					auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

					std::filesystem::path includeDirFullPath;

					std::filesystem::path::iterator includeDirIt;
					for (auto it = randomEcsFilePath.end(); it != randomEcsFilePath.begin(); --it) {
						auto folder = *it;
						if (folder == projectContext->includeDirectory_) {
							includeDirIt = it;
							break;
						}
					}

					for (auto it = randomEcsFilePath.begin(); it != includeDirIt; it++) {
						includeDirFullPath /= *it;
					}

					includeDirFullPath /= *includeDirIt;
					//Get path


					auto dotfile = std::make_shared<OS::TextFile>(includeDirFullPath / "auto_ECSSystemsCallGraph.dot");

					char* dotData = nullptr;
					unsigned int length = 0;

					gvLayout(gvc, g, "dot");
					gvRenderData(gvc, g, "dot", &dotData, &length);

					agclose(g);
					gvFreeContext(gvc);
					dotfile->Create();
					std::string dotText{ dotData };
					(*dotfile) << dotText;
					dotfile->Close();
				}
				//////

				};

			for (auto& childThread : childThreads) {
				createClusterSystemsCallGraph(projectContext, childThread);
				CalculateSystemsCallOrder(childThread);
			}
			createClusterSystemsCallGraph(projectContext, mainThread);
			CalculateSystemsCallOrder(mainThread);


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
				cppIncludes.paths_.insert("auto_OksEngine.RunSystems.hpp");

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
						cppIncludes.paths_.insert(systemIncludePath / ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp"));
						return true;
					});
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

			return {
				GenerateRunSystemsHppFile(projectContext),
				GenerateRunSystemsCppFile(clusters, projectContext)
			};
		}

		void ProcessDirectory(const std::filesystem::path& dir, std::shared_ptr<ProjectContext> projectContext) {

			// Рекурсивно обрабатываем поддиректории
			std::vector<std::filesystem::path> subdirs;
			for (const auto& entry : std::filesystem::directory_iterator(dir)) {
				if (entry.is_directory()) {
					ProcessDirectory(entry.path(), projectContext); // Обработка поддиректории
					subdirs.push_back(entry.path().filename());
				}
			}

			// Собираем .ecs файлы в текущей директории
			std::vector<std::filesystem::path> ecs_files;
			for (const auto& entry : std::filesystem::directory_iterator(dir)) {
				if (entry.is_regular_file() && entry.path().extension() == ".ecs") {
					ecs_files.push_back(entry.path().filename());
				}
			}

			// Сортируем имена для детерминизма
			std::sort(ecs_files.begin(), ecs_files.end());
			std::sort(subdirs.begin(), subdirs.end());

			const std::filesystem::path componentIncludePath = GetSubPath(
				dir,
				projectContext->includeDirectory_,
				ResultRange::FromStartFolderToEnd,
				SearchDirection::FromEndToBegin,
				true);
			//componentIncludePath ;
			//includes.paths_.insert(componentIncludePath / ("auto_OksEngine.Components.hpp"));

			// Создаем OksEngine.Components.hpp
			std::ofstream out(dir / "auto_OksEngine.ECS.hpp");
			if (!out) return;

			//if (!ecs_files.empty()) {
				// Включаем .ecs файлы текущей директории
			for (const auto& file : ecs_files) {
				out << "#include \"" << componentIncludePath.string() + "/" + "auto_" + file.stem().string() + ".hpp" << "\"\n";
			}
			// Включаем OksEngine.Components.hpp из поддиректорий
			for (const auto& subdir : subdirs) {
				out << "#include \"" << subdir.string() << "/auto_OksEngine.ECS.hpp\"\n";
			}
			//}
		}

		std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> GenerateECSCXXFilesStructure(std::shared_ptr<ProjectContext> projectContext) {

			std::vector<std::pair<std::filesystem::path, std::shared_ptr<File>>> files;


			ComponentFileStructureGenerator::CreateInfo ci{
				.includeDirectory_ = projectContext->includeDirectory_
			};

			//Generate auto_OksEngine.{ComponentName}.hpp
			projectContext->ForEachComponentEcsFile(
				[&](std::shared_ptr<ParsedECSFile> ecsFile, bool isLast) {
					auto componentEcsFile = std::dynamic_pointer_cast<ParsedComponentECSFile>(ecsFile);
					ComponentFileStructureGenerator generator{ ci };
					auto file = generator.GenerateECSCXXFilesStructure(projectContext, componentEcsFile);
					files.push_back(file);
					return true;
				});

			//Generate auto_OksEngine.{SystemName}.hpp
			projectContext->ForEachSystemEcsFile(
				[&](std::shared_ptr<ParsedECSFile> ecsFile) {
					auto systemEcsFile = std::dynamic_pointer_cast<ParsedSystemECSFile>(ecsFile);
					SystemFileStructureGenerator::CreateInfo ci{
						.includeDirectory_ = projectContext->includeDirectory_
					};
					SystemFileStructureGenerator generator{ ci };
					auto file = generator.GenerateECSCXXFilesStructure(projectContext, systemEcsFile);
					files.push_back(file);
					return true;
				});
			//Generate OksEngine.ECS.hpp files For ECS.
			ProcessDirectory(projectContext->ci_.workDirs_[0], projectContext);
			//
			////Generate OksEngine.Components.hpp files For COMPONENTS.
			//for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

			//	//if (componentsSystems.components_.empty()) {
			//	//	continue;
			//	//}

			//	if (categoryPath == "D:/OksEngine/Tools/ECSGenerator/../../Sources/Engine/Sources\\Debug\\ECS\\ECSEditor\\Resources\\LoadECSFiles") {
			//		//__debugbreak();
			//	}

			//	//Generate components includes for components in the subdirectory 
			//	File::Includes includes{};

			//	for (auto componentParsedEcsFile : componentsSystems.components_) {
			//		auto componentPath = componentParsedEcsFile->GetPath().parent_path();
			//		const std::filesystem::path includePath = GetSubPath(
			//			componentParsedEcsFile->GetPath().parent_path(),
			//			projectContext->includeDirectory_,
			//			ResultRange::FromStartFolderToEnd,
			//			SearchDirection::FromEndToBegin,
			//			true);
			//		includes.paths_.insert(includePath / ("auto_OksEngine." + componentParsedEcsFile->GetName() + ".hpp"));
			//	}

			//	//generate Include OksEngine.Components.hpp files from subdirectories.
			//	for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
			//		if (path.string().starts_with(categoryPath.string())) {
			//			if (path == categoryPath) {
			//				continue;
			//			}
			//			auto pathIt = path.begin();
			//			bool subdirFound = true;
			//			for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
			//				if (*it != *pathIt) {
			//					subdirFound = false;
			//					break;
			//				}
			//				++pathIt;
			//			}

			//			if (subdirFound) {
			//				//Subdirectory found!
			//				std::filesystem::path subdirPath;
			//				++pathIt;
			//				for (auto it = path.begin(); it != pathIt; ++it) {
			//					subdirPath /= *it;
			//				}
			//				//std::filesystem::path componentIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
			//				const std::filesystem::path componentIncludePath = GetSubPath(
			//					subdirPath,
			//					projectContext->includeDirectory_,
			//					ResultRange::FromStartFolderToEnd,
			//					SearchDirection::FromEndToBegin,
			//					true);
			//				//componentIncludePath ;
			//				includes.paths_.insert(componentIncludePath / ("auto_OksEngine.Components.hpp"));
			//			}
			//		}
			//	}

			//	File::CreateInfo fci{
			//		.isHpp_ = true,
			//		.includes_ = includes,
			//		.base_ = nullptr
			//	};

			//	auto categoryComponentsIncludeFile = std::make_shared<File>(fci);
			//	std::filesystem::path componentIncludesFullFilePath = categoryPath / "auto_OksEngine.Components.hpp";

			//	std::string componentIncludesFullFilePathString = componentIncludesFullFilePath.string();
			//	std::replace(componentIncludesFullFilePathString.begin(), componentIncludesFullFilePathString.end(), '\\', '/');
			//	files.push_back({ std::filesystem::path{ componentIncludesFullFilePathString }, categoryComponentsIncludeFile });
			//}



			////Generate OksEngine.Components.hpp files For Systems.
			//for (auto [categoryPath, componentsSystems] : projectContext->oneCategoryEcsFiles_) {

			//	//if (componentsSystems.systems_.empty()) {
			//	//	continue;
			//	//}
			//	//Generate components includes for components in the subdirectory 
			//	File::Includes includes{};
			//	std::filesystem::path systemsIncludesFilePath;
			//	for (auto systemParsedEcsFile : componentsSystems.systems_) {
			//		systemsIncludesFilePath = systemParsedEcsFile->GetPath().parent_path();
			//		const std::filesystem::path systemIncludePath = GetSubPath(
			//			systemParsedEcsFile->GetPath().parent_path(),
			//			projectContext->includeDirectory_,
			//			ResultRange::FromStartFolderToEnd,
			//			SearchDirection::FromEndToBegin,
			//			true);

			//		includes.paths_.insert(systemIncludePath / ("auto_OksEngine." + systemParsedEcsFile->GetName() + ".hpp"));
			//	}

			//	//generate Include OksEngine.Systems.hpp files from subdirectories.
			//	for (auto [path, componentsSystems] : projectContext->oneCategoryEcsFiles_) {
			//		if (componentsSystems.systems_.empty()) {
			//			continue;
			//		}
			//		if (path.string().starts_with(categoryPath.string())) {
			//			if (path == categoryPath) {
			//				continue;
			//			}
			//			auto pathIt = path.begin();
			//			bool subdirFound = true;
			//			for (auto it = categoryPath.begin(); it != categoryPath.end(); ++it) {
			//				if (*it != *pathIt) {
			//					subdirFound = false;
			//					break;
			//				}
			//				++pathIt;
			//			}
			//			if (subdirFound) {
			//				//Subdirectory found!
			//				std::filesystem::path subdirPath;
			//				++pathIt;
			//				for (auto it = path.begin(); it != pathIt; ++it) {
			//					subdirPath /= *it;
			//				}
			//				//std::filesystem::path systemIncludePath = GetSubPathFromFolderToEnd(subdirPath, projectContext->includeDirectory_);
			//				const std::filesystem::path systemIncludePath = GetSubPath(
			//					subdirPath,
			//					projectContext->includeDirectory_,
			//					ResultRange::FromStartFolderToEnd,
			//					SearchDirection::FromEndToBegin,
			//					true);
			//				//systemIncludePath ;
			//				includes.paths_.insert(systemIncludePath / ("auto_OksEngine.Systems.hpp"));
			//			}
			//		}
			//	}

			//	File::CreateInfo fci{
			//		.isHpp_ = true,
			//		.includes_ = includes,
			//		.base_ = nullptr
			//	};

			//	auto categorySystemIncludeFile = std::make_shared<File>(fci);
			//	std::filesystem::path systemIncludesFullFilePath = systemsIncludesFilePath / "auto_OksEngine.Systems.hpp";

			//	std::string systemIncludesFullFilePathString = systemIncludesFullFilePath.string();
			//	std::replace(systemIncludesFullFilePathString.begin(), systemIncludesFullFilePathString.end(), '\\', '/');
			//	files.push_back({ std::filesystem::path{ systemIncludesFullFilePathString }, categorySystemIncludeFile });
			//}

			return files;
		}

	private:
	};




}