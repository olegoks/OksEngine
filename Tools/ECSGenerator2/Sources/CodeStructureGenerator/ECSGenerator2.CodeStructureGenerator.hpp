#pragma once 

#include <System/ECSGenerator2.SystemCodeStructureGenerator.hpp>
#include <Component/ECSGenerator2.ComponentCodeGenerator.hpp>
#include <Struct/ECSGenerator2.StructCodeStructureGenerator.hpp>
#include <Constant/ECSGenerator2.ConstantCodeGenerator.hpp>
#include <Archetype/ECSGenerator2.ArchetypeCodeGenerator.hpp>
#include <ECSGenerator2.CodeGenerator.hpp>

#include <Namespace/ECSGenerator2.ParsedNamespace.hpp>

extern "C" {
#include <graphviz/gvc.h>
}

namespace ECSGenerator2 {

	class CodeStructureGenerator {

	public:

		struct CreateInfo {
			std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles_;
		};


		CodeStructureGenerator(const CreateInfo& ci) : ci_{ ci } {

		}


		//Generate .hpp files for .ecs files that contain systems and components
		std::shared_ptr<CodeStructure::File> GenerateECSFileHppFile(
			const std::filesystem::path& includeDirectory,
			const std::shared_ptr<ParsedECSFile> parsedECSFile) {

			CodeStructure::File::Includes includes;
			includes.paths_.insert("ECS2.hpp");
			includes.paths_.insert("chrono");
			includes.paths_.insert("OksEngine.IComponent.hpp");

			//Add "OksEngine.EditEntity.hpp" header if component contains fields with ECS2::Entity::Id.
			bool needToIncludeEditEntity = false;
			parsedECSFile->ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {

				parsedComponent->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {
					if (fieldInfo.GetTypeName() == "std::vector<ECS2::Entity::Id>" || fieldInfo.GetTypeName() == "ECS2::Entity::Id") {
						needToIncludeEditEntity = true;
						return false;
					}

					return true;
					});
				return true;
				});
			if (needToIncludeEditEntity) {
				includes.paths_.insert("auto_OksEngine.EditEntity.hpp");
			}

			parsedECSFile->ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {

				parsedComponent->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {

					if (fieldInfo.GetTypeName() == "std::string") {
						includes.paths_.insert("string");
					}
					if (fieldInfo.GetTypeName().find("Common") != std::string::npos) {
						includes.paths_.insert("Common.hpp");
					}
					if (fieldInfo.GetTypeName().find("std::chrono") != std::string::npos) {
						includes.paths_.insert("chrono");
					}
					if (fieldInfo.GetTypeName().find("PAL") != std::string::npos) {
						includes.paths_.insert("PAL.hpp");
					}
					if (fieldInfo.GetTypeName().find("glm") != std::string::npos) {
						includes.paths_.insert("glm/glm.hpp");
					}
					if (fieldInfo.GetTypeName().find("std::queue") != std::string::npos) {
						includes.paths_.insert("queue");
					}
					if (fieldInfo.GetTypeName().find("std::vector") != std::string::npos) {
						includes.paths_.insert("vector");
					}
					if (fieldInfo.GetTypeName().find("RAL") != std::string::npos) {
						includes.paths_.insert("RAL.hpp");
					}
					if (fieldInfo.GetTypeName().find("Geom:") != std::string::npos) {
						includes.paths_.insert("Geometry.hpp");
					}
					if (fieldInfo.GetTypeName().find("std::map") != std::string::npos) {
						includes.paths_.insert("map");
					}
					if (fieldInfo.GetTypeName().find("UIAL::Window") != std::string::npos) {
						includes.paths_.insert("UIAL.Window.hpp");
					}
					if (fieldInfo.GetTypeName().find("UI::Window") != std::string::npos) {
						includes.paths_.insert("UI.Window.hpp");
					}
					if (fieldInfo.GetTypeName().find("sf") != std::string::npos) {
						includes.paths_.insert("SFML/Audio.hpp");
					}
					if (fieldInfo.GetTypeName().find("glm::quat") != std::string::npos) {
						includes.paths_.insert("glm/gtc/quaternion.hpp");
					}
					if (fieldInfo.GetTypeName().find("PE") != std::string::npos) {
						includes.paths_.insert("PE.PhysicsEngine.hpp");
					}
					if (fieldInfo.GetTypeName().find("AsyncResourceSubsystem") != std::string::npos) {
						includes.paths_.insert("Resources/OksEngine.Resource.Subsystem.hpp");
					}
					if (fieldInfo.GetTypeName().find("ImGui") != std::string::npos) {
						includes.paths_.insert("imgui/imgui.h");
					}
					if (fieldInfo.GetTypeName().find("ImPlot") != std::string::npos) {
						includes.paths_.insert("implot/implot.h");
					}
					if (fieldInfo.GetTypeName().find("ai") != std::string::npos) {
						includes.paths_.insert("assimp/Importer.hpp");
						includes.paths_.insert("assimp/scene.h");
						includes.paths_.insert("assimp/postprocess.h");
					}

					return true;
					});

				return true;
				});

			if (parsedECSFile->GetName() == "OksEngine.Behaviour") {
				Common::BreakPointLine();
			}

			//New code generation
			std::vector<std::shared_ptr<CodeStructure::Base>> bases;

			parsedECSFile->ForEachRootTable([&](std::shared_ptr<ParsedTable> table) {


				std::function<std::vector<std::shared_ptr<CodeStructure::Base>>(std::shared_ptr<ParsedTable>)> processTable
					= [&](std::shared_ptr<ParsedTable> parsedTable) {

					const ParsedTable::Type tableType = parsedTable->GetType();

					if (tableType == ParsedTable::Type::Namespace) {
						const auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(parsedTable);

						auto namespaceObject = std::make_shared<CodeStructure::Namespace>(parsedNamespace->GetName());

						parsedNamespace->ForEachChildTable([&](std::shared_ptr<ParsedTable> parsedTable) {
							auto base = processTable(parsedTable);
							for (Common::Index i = 0; i < base.size(); i++) {
								namespaceObject->Add(base[i]);
							}
							return true;
							});

						return std::vector<std::shared_ptr<CodeStructure::Base>>{ namespaceObject };
					}
					else if (tableType == ParsedTable::Type::System) {

						const auto parsedSystem = std::dynamic_pointer_cast<ParsedSystem>(parsedTable);

						SystemStructureGenerator::CreateInfo ssgci{
							.includeDirectory_ = includeDirectory
						};

						SystemStructureGenerator systemGenerator{ ssgci };

						if (parsedSystem->GetName() == "LoadConfigFile") {
							Common::BreakPointLine();
						}
						//Add include components.
						auto systemIncludeComponents = systemGenerator.GenerateUpdateMethodRequiredComponentIncludes(parsedSystem->ci_.updateMethod_);
						//std::unordered_set<std::string> includeComponents;

						//includeComponents.insert(systemIncludeComponents.begin(), systemIncludeComponents.end());

						//Generate includes using include directory.
						for (const ParsedComponentPtr component : systemIncludeComponents) {
							for (auto ecsFile : ci_.ecsFiles_) {
								if (ecsFile->GetName() == "OksEngine.Behaviour" && component->GetName() == "LuaScript") {
									Common::BreakPointLine();
								}
								//Skip component include if ecs file contains defintion of the component.
								if (parsedECSFile->IsContainsComponent(component)) {
									continue;
								}
								if (ecsFile->IsContainsComponent(component)) {
									const std::filesystem::path ecsFilePath = ecsFile->GetPath().parent_path();

									auto [it, _] = std::mismatch(
										ecsFilePath.begin(), ecsFilePath.end(),
										includeDirectory.begin()
									);
									std::filesystem::path componentIncludePath;

									while (it != ecsFilePath.end()) {
										componentIncludePath /= *it;
										++it;
									}

									includes.paths_.insert((componentIncludePath / ("auto_" + ecsFile->GetName())).string() + ".hpp");

								}
							}

						}
						//Generate system structure.
						std::vector<std::shared_ptr<CodeStructure::Base>> bases;
						auto systemStruct = systemGenerator.GenerateSystemStructCode(parsedSystem);
						bases.push_back(systemStruct);
						//Generate run system function.
						auto runSystemFunction = systemGenerator.GenerateRunSystemCodeRealization(parsedSystem);
						bases.push_back(runSystemFunction);

						return bases;

					}
					else if (tableType == ParsedTable::Type::Struct) {

						const auto parsedStruct = std::dynamic_pointer_cast<ParsedStruct>(parsedTable);

						StructCodeStructureGenerator::CreateInfo scgci{
							.includeDirectory_ = includeDirectory
						};

						StructCodeStructureGenerator structGenerator{ scgci };

						auto structCodeStructure = structGenerator.GenerateStructCodeStructure(parsedStruct);
						std::vector<std::shared_ptr<CodeStructure::Base>> bases;
						bases.push_back(structCodeStructure);
						auto structsIncludes = structGenerator.GenerateIncludes(parsedStruct);
						includes.paths_.insert(structsIncludes.paths_.begin(), structsIncludes.paths_.end());

						return bases;

					}
					else if (tableType == ParsedTable::Type::Constant) {

						const auto parsedConstant = std::dynamic_pointer_cast<ParsedConstant>(parsedTable);

						ConstantCodeStructureGenerator::CreateInfo scgci{
							.includeDirectory_ = includeDirectory
						};

						ConstantCodeStructureGenerator structGenerator{ scgci };

						auto constantCodeStructure = structGenerator.GenerateConstantVariable(parsedConstant);
						std::vector<std::shared_ptr<CodeStructure::Base>> bases;
						bases.push_back(constantCodeStructure);
						return bases;

					}
					else if (tableType == ParsedTable::Type::Archetype) {

						const auto parsedArchetype = std::dynamic_pointer_cast<ParsedArchetype>(parsedTable);

						ArchetypeCodeStructureGenerator::CreateInfo scgci{
							.includeDirectory_ = includeDirectory
						};

						ArchetypeCodeStructureGenerator arhetypeGenerator{ scgci };

						auto constantCodeStructure = arhetypeGenerator.GenerateArchetypeDefine(parsedArchetype);
						std::vector<std::shared_ptr<CodeStructure::Base>> bases;
						bases.push_back(constantCodeStructure);
						return bases;

					}
					else if (tableType == ParsedTable::Type::Component) {

						const auto parsedComponent = std::dynamic_pointer_cast<ParsedComponent>(parsedTable);

						ComponentCodeStructureGenerator::CreateInfo ccgci{
							.includeDirectory_ = includeDirectory
						};

						ComponentCodeStructureGenerator componentGenerator{ ccgci };

						//Add include components.
						std::vector<std::shared_ptr<CodeStructure::Base>> bases;
						auto componentStruct = componentGenerator.GenerateComponentStruct(parsedComponent);
						bases.push_back(componentStruct);

						//If we want to create realization of help functions manually generate only prototype in .hpp file.
						{
							auto editFunction
								= (parsedComponent->ci_.manualEditFunction_)
								? (componentGenerator.GenerateEditFunctionPrototype(parsedComponent))
								: (componentGenerator.GenerateEditFunctionRealization(parsedComponent));

							bases.push_back(editFunction);
						}
						{
							auto addFunction
								= (parsedComponent->ci_.manualAddFunction_)
								? (componentGenerator.GenerateAddFunctionPrototype(parsedComponent))
								: (componentGenerator.GenerateAddFunctionRealization(parsedComponent));

							bases.push_back(addFunction);
						}
						{
							auto bindFunction
								= (parsedComponent->ci_.manualBindFunction_)
								? (componentGenerator.GenerateBindFunctionPrototype(parsedComponent))
								: (componentGenerator.GenerateBindFunctionRealization(parsedComponent));

							bases.push_back(bindFunction);
						}
						{
							auto parseFunction
								= (parsedComponent->ci_.manualParseFunction_)
								? (componentGenerator.GenerateParseFunctionPrototype(parsedComponent))
								: (componentGenerator.GenerateParseFunctionRealization(parsedComponent));

							bases.push_back(parseFunction);
						}
						{
							auto serializeFunction
								= (parsedComponent->ci_.manualSerializeFunction_)
								? (componentGenerator.GenerateSerializeFunctionPrototype(parsedComponent))
								: (componentGenerator.GenerateSerializeFunctionRealization(parsedComponent));

							bases.push_back(serializeFunction);
						}

						auto componentIncludes = componentGenerator.GenerateIncludes(parsedComponent);
						includes.paths_.insert(componentIncludes.paths_.begin(), componentIncludes.paths_.end());

						return bases;
					}

					return std::vector<std::shared_ptr<ECSGenerator2::CodeStructure::Base>>{};
					};


				auto BASES = processTable(table);
				bases.insert(bases.end(), BASES.begin(), BASES.end());
				return true;
				});
			{
				auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");
				for (auto base : bases) {
					namespaceObject->Add(base);
				}

				//Hack Move template edit functions to end of .hpp file. Because of template specializetion cant be in child namespaces.

				parsedECSFile->ForEachRootTable([&](std::shared_ptr<ParsedTable> table) {

					std::vector<std::string> currentNamespace;
					std::function<void(std::shared_ptr<ParsedTable>)> processTable
						= [&](std::shared_ptr<ParsedTable> parsedTable) {

						const ParsedTable::Type tableType = parsedTable->GetType();
						if (tableType == ParsedTable::Type::Namespace) {

							const auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(parsedTable);

							currentNamespace.push_back(parsedNamespace->GetName());

							parsedNamespace->ForEachChildTable([&](std::shared_ptr<ParsedTable> parsedTable) {
								processTable(parsedTable);
								return true;
								});

						}
						if (tableType == ParsedTable::Type::Component) {

							const auto parsedComponent = std::dynamic_pointer_cast<ParsedComponent>(parsedTable);

							ComponentCodeStructureGenerator::CreateInfo ccgci{
								.includeDirectory_ = includeDirectory
							};

							ComponentCodeStructureGenerator componentGenerator{ ccgci };


							auto templateEditFunction = componentGenerator.GenerateTemplateEditFunctionWithNamespace(currentNamespace, parsedComponent);

							namespaceObject->Add(templateEditFunction);
						}

						};

					processTable(table);

					return true;
					});
				//Hack



				CodeStructure::File::CreateInfo fci{
					.isHpp_ = true,
					.includes_ = includes,
					.base_ = { namespaceObject }
				};
				auto file = std::make_shared<CodeStructure::File>(fci);

				return { file };
			}
			//New code generation

		}


		std::shared_ptr<CodeStructure::File> GenerateECSFileCppFile(
			const std::filesystem::path& includeDirectory,
			const std::shared_ptr<ParsedECSFile> parsedECSFile) {

			if (parsedECSFile->GetName() == "OksEngine.Behaviour") {
				Common::BreakPointLine();
			}
			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");
			{
				using namespace std::string_literals;

				auto generateUpdateMethod = [](std::shared_ptr<ParsedSystem> system) {

					auto generateUpdateMethodParameters =
						[](std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethodInfo) {

						std::vector<CodeStructure::Function::Parameter> updateMethodParameters;

						Common::UInt64 currentEntityIndex = 0;
						updateMethodInfo->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {

							updateMethodParameters.push_back({
								"ECS2::Entity::Id",
								std::format("entity{}id", currentEntityIndex) });

							entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
								CodeStructure::Function::Parameter parameter;
								if (include.IsReadonly()) {
									parameter.inputType_ = std::format("const {}*", include.GetName());
								}
								else {
									parameter.inputType_ = std::format("{}*", include.GetName());
								}

								parameter.valueName_ = std::format("{}{}", include.GetLowerName(), currentEntityIndex);
								updateMethodParameters.push_back(parameter);
								return true;
								});

							++currentEntityIndex;
							return true;
							});

						return updateMethodParameters;

						};

					//Update method.
					const std::vector<CodeStructure::Function::Parameter> updateMethodParameters
						= generateUpdateMethodParameters(system->ci_.updateMethod_);

					CodeStructure::Function::CreateInfo updateMethodCI{
						.name_ = system->GetName() + "::Update",
						.parameters_ = updateMethodParameters,
						.returnType_ = "void",
						.code_ = "",
						.isPrototype_ = false,
						.inlineModifier_ = false
					};

					auto updateMethod = std::make_shared<CodeStructure::Function>(updateMethodCI);

					return updateMethod;
					};
				auto generateAfterUpdateMethod = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

					CodeStructure::Function::CreateInfo afterUpdateMethodCI{
						.name_ = systemEcsFile->GetName() + "::AfterUpdate",
						.parameters_ = { },
						.returnType_ = "void",
						.code_ = "",
						.isPrototype_ = false,
						.inlineModifier_ = false
					};

					auto afterUpdateMethod = std::make_shared<CodeStructure::Function>(afterUpdateMethodCI);

					return afterUpdateMethod;

					};

				//New generation
				parsedECSFile->ForEachRootTable([&](std::shared_ptr<ParsedTable> table) {

					std::function<std::vector<std::shared_ptr<CodeStructure::Base>>(std::shared_ptr<ParsedTable>)> processTable
						= [&](std::shared_ptr<ParsedTable> table) {
						if (table->GetType() == ParsedTable::Type::Namespace) {
							auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(table);
							auto namespaceObject = std::make_shared<CodeStructure::Namespace>(parsedNamespace->GetName());
							parsedNamespace->ForEachChildTable([&](std::shared_ptr<ParsedTable> parsedTable) {
								auto bases = processTable(parsedTable);
								for (auto base : bases) {
									namespaceObject->Add(base);
								}
								return true;
								});

							return std::vector<std::shared_ptr<CodeStructure::Base>>{ namespaceObject };
						}
						if (table->GetType() == ParsedTable::Type::System) {
							auto parsedSystem = std::dynamic_pointer_cast<ParsedSystem>(table);
							std::vector<std::shared_ptr<CodeStructure::Base>> bases;
							bases.push_back(generateUpdateMethod(parsedSystem));
							if (parsedSystem->ci_.afterUpdateMethod_ != nullptr) {
								bases.push_back(generateAfterUpdateMethod(parsedSystem));
							}
							return bases;
						}
						if (table->GetType() == ParsedTable::Type::Component) {
							auto parsedComponent = std::dynamic_pointer_cast<ParsedComponent>(table);
							//If we want to create realization of help functions manually generate only prototype in .hpp file.

							ComponentCodeStructureGenerator::CreateInfo ccsgci{
								.includeDirectory_ = includeDirectory
							};

							ComponentCodeStructureGenerator componentGenerator{ ccsgci };

							std::vector<std::shared_ptr<CodeStructure::Base>> bases;

							if (parsedComponent->ci_.manualEditFunction_) {
								auto editFunction = componentGenerator.GenerateEditFunctionEmptyRealization(parsedComponent);
								bases.push_back(editFunction);
							}
							if (parsedComponent->ci_.manualAddFunction_) {
								auto addFunction = componentGenerator.GenerateAddFunctionEmptyRealization(parsedComponent);

								bases.push_back(addFunction);
							}
							if (parsedComponent->ci_.manualBindFunction_) {
								auto bindFunction = componentGenerator.GenerateBindFunctionEmptyRealization(parsedComponent);
								bases.push_back(bindFunction);
							}
							if (parsedComponent->ci_.manualParseFunction_) {
								auto parseFunction = componentGenerator.GenerateParseFunctionEmptyRealization(parsedComponent);
								bases.push_back(parseFunction);
							}
							if (parsedComponent->ci_.manualSerializeFunction_) {
								auto serializeFunction = componentGenerator.GenerateSerializeFunctionEmptyRealization(parsedComponent);

								bases.push_back(serializeFunction);
							}

							return bases;
						}
						return std::vector<std::shared_ptr<CodeStructure::Base>>{};
						};

					auto bases = processTable(table);
					for (auto base : bases) {
						namespaceObject->Add(base);
					}

					return true;
					});
				//New generation

				//parsedECSFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {


				//	});

			}

			CodeStructure::File::Includes includes;
			auto ecsFilePath = parsedECSFile->GetPath();

			auto [it, _] = std::mismatch(
				ecsFilePath.begin(), ecsFilePath.end(),
				includeDirectory.begin()
			);
			std::filesystem::path filePath;

			while (it != ecsFilePath.end()) {
				filePath /= *it;
				++it;
			}

			includes.paths_.insert(filePath.parent_path() / ("auto_" + parsedECSFile->GetName() + ".hpp"));

			CodeStructure::File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<CodeStructure::File>(fci);

			return { file };
		}

		std::vector<std::shared_ptr<CodeStructure::File>> Generate(
			const std::filesystem::path& includeDirectory,
			const std::shared_ptr<ParsedECSFile> parsedECSFile) {

			auto hppSystemFile = GenerateECSFileHppFile(includeDirectory, parsedECSFile);


			auto cppSystemFile = GenerateECSFileCppFile(includeDirectory, parsedECSFile);


			return { hppSystemFile, cppSystemFile };
		}

		void ProcessDirectory(
			const std::filesystem::path currentDir,
			const std::filesystem::path includeDirectory,
			std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::CodeStructure::File>>& pathToHppECSFile) {

			// Рекурсивно обрабатываем поддиректории
			std::vector<std::filesystem::path> subdirs;
			if (std::filesystem::path{ "D:/OksEngine/Tools/ECSGenerator2/../../Sources/Engine/Sources/Render" } == currentDir) {
				Common::BreakPointLine();
			}
			for (const auto& entry : std::filesystem::directory_iterator(currentDir)) {
				if (entry.is_directory()) {
					ProcessDirectory(entry.path(), includeDirectory, pathToHppECSFile); // Обработка поддиректории
					subdirs.push_back(entry.path().filename());
				}
			}

			// Собираем .ecs файлы в текущей директории
			std::vector<std::filesystem::path> ecs_files;
			for (const auto& entry : std::filesystem::directory_iterator(currentDir)) {
				if (entry.is_regular_file() && entry.path().extension() == ".ecs") {
					ecs_files.push_back(entry.path().filename());
				}
			}

			// Сортируем имена для детерминизма
			std::sort(ecs_files.begin(), ecs_files.end());
			std::sort(subdirs.begin(), subdirs.end());

			auto [it, _] = std::mismatch(
				currentDir.begin(), currentDir.end(),
				includeDirectory.begin()
			);
			std::filesystem::path filePath;

			while (it != currentDir.end()) {
				filePath /= *it;
				++it;
			}



			CodeStructure::File::Includes includes;

			// Включаем .ecs файлы текущей директории
			for (const auto& file : ecs_files) {
				includes.paths_.insert(filePath.string() + "/" + "auto_" + file.stem().string() + ".hpp");
			}
			// Включаем OksEngine.Components.hpp из поддиректорий
			for (const auto& subdir : subdirs) {
				includes.paths_.insert((filePath / subdir).string() + "/auto_OksEngine.ECS.hpp");
			}

			CodeStructure::File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = nullptr
			};
			auto codeStructureFile = std::make_shared<CodeStructure::File>(fci);

			pathToHppECSFile[currentDir / "auto_OksEngine.ECS.hpp"] = codeStructureFile;

		}

		//Generate auto_OksEngine.ECS.hpp files in each dir.
		std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::CodeStructure::File>>
			GenerateDirECSIncludeHppFiles(const std::filesystem::path currentDir, const std::filesystem::path includeDirectory) {

			std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::CodeStructure::File>> hppECSFiles;

			ProcessDirectory(currentDir, includeDirectory, hppECSFiles);

			return hppECSFiles;

		}

		std::shared_ptr<CodeStructure::File>
			GenerateParseEntityHppFile(std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles) {

			auto generateParseComponentCode
				= [](
					std::shared_ptr<ParsedComponent> component) -> CodeStructure::Code {

						CodeStructure::Code code;

						code.Add(std::format(
							"luabridge::LuaRef {}Ref = entity[\"{}\"];",
							component->GetLowerName(),
							component->GetLowerName()));

						code.Add(std::format(
							"if (!{}Ref.isNil()) {{",
							component->GetLowerName()));

						code.Add(std::format("{} {} = Parse{}({}Ref);",
							component->GetName(),
							component->GetLowerName(),
							component->GetName(),
							component->GetLowerName()));

						code.Add(std::format("world->CreateComponent<{}>(newEntityId",
							component->GetName()));

						component->ForEachField([&](const ParsedComponent::FieldInfo& fieldInfo, bool isLast) {

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

			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");

			std::vector<std::shared_ptr<ParsedComponent>> allParsedComponents;


			CodeStructure::Code code;
			code.Add(
				"auto getNewId = [&](ECS2::Entity::Id oldId) {"
				"	#pragma region Assert\n"
				"	ASSERT_FMSG(oldToNewId.contains(oldId),\n"
				"	\"Invalid scene file. Can not define all entities ids: {}.\", static_cast<Common::Index>(oldId));\n"
				"	#pragma endregion\n"
				"	return oldToNewId.at(oldId);\n"
				"};");

			for (auto ecsFile : ecsFiles) {

				//New generation.
				ecsFile->ForEachComponent([&](
					std::vector<std::shared_ptr<ParsedTable>>& childTables,
					std::shared_ptr<ParsedComponent> component) {

						if (!component->ci_.serializable_) {
							return;
						}

						code.Add("{");

						std::vector<std::string> namespaceStrings;
						for (auto childTable : childTables) {
							if (childTable->GetType() == ParsedTable::Type::Namespace) {
								auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(childTable);
								namespaceStrings.push_back(parsedNamespace->GetName());
							}
						}

						if (!namespaceStrings.empty()) {
							std::string fullNamespace;
							for (Common::Index i = 0; i < namespaceStrings.size(); i++) {
								fullNamespace += namespaceStrings[i];
								if (i != namespaceStrings.size() - 1) {
									fullNamespace += "::";
								}
							}

							code.Add("using namespace {};", fullNamespace);
						}

						code.Add(generateParseComponentCode(component));
						code.Add("}");

						code.NewLine();


					});



				//New generation.

				// 
				//ecsFile->ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {
				//	allParsedComponents.push_back(parsedComponent);
				//	});
			}

			CodeStructure::Function::CreateInfo editEntityFunction{
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

			auto editEntityFunctionObject = std::make_shared<CodeStructure::Function>(editEntityFunction);

			//namespaceObject->Add(generateParseEntityFunctionRealization(allParsedComponents));
			namespaceObject->Add(editEntityFunctionObject);

			CodeStructure::File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("OksEngine.ECS.hpp");
			//includes.paths_.insert("magic_enum/magic_enum.hpp");

			//hpp file
			CodeStructure::File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<CodeStructure::File>(fci);

			return file;


			//auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			//std::filesystem::path includeDirFullPath = GetSubPath(
			//	randomEcsFilePath,
			//	projectContext->includeDirectory_,
			//	ECSGenerator2::ResultRange::FromStartToStartFolder,
			//	SearchDirection::FromEndToBegin,
			//	false);

			//std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.ParseEntity.hpp");
			//FormatPath(systemCppFileFullPath);

			//return { systemCppFileFullPath, file };
		}

		std::shared_ptr<CodeStructure::File>
			GenerateSerializeEntityHppFile(std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles) {

			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");


			CodeStructure::Code code;

			code.Add("std::string serializedEntity;");
			code.Add("const ECS2::ComponentsFilter components = world->GetEntityComponentsFilter(entityId);");
			code.Add("serializedEntity += \"\\t\\tID = \" + std::to_string(entityId) + \",\\n\";");

			for (auto ecsFile : ecsFiles) {

				ecsFile->ForEachComponent([&](std::vector<std::shared_ptr<ParsedTable>>& childTables,
					std::shared_ptr<ParsedComponent> component) {

						if (!component->ci_.serializable_) {
							return;
						}

						std::string fullComponentNamespace;

						for (Common::Index i = 0; i < childTables.size(); i++) {
							fullComponentNamespace += childTables[i]->GetName();
							if (i != childTables.size() - 1) {
								fullComponentNamespace += "::";
							}
						}

						code.Add("{");
						if (!childTables.empty()) {
							code.Add("using namespace {};", fullComponentNamespace);
						}
						code.Add(std::format(
							"if (components.IsSet<{}>()) {{"
							"	const auto* component = world->GetComponent<{}>(entityId);"
							"	serializedEntity += \"\\t\\t\" +Serialize{}(component);",
							component->GetName(),
							component->GetName(),
							component->GetName()));

						code.Add("	serializedEntity += \",\\n\";");
						code.Add("}");
						code.Add("}");

					});


			}

			code.Add("return serializedEntity;");


			CodeStructure::Function::CreateInfo serializeEntityFunction{
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

			auto serializedEntityFunctionObject = std::make_shared<CodeStructure::Function>(serializeEntityFunction);

			namespaceObject->Add(serializedEntityFunctionObject);

			CodeStructure::File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("OksEngine.ECS.hpp");
			//includes.paths_.insert("magic_enum/magic_enum.hpp");

			//hpp file
			CodeStructure::File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<CodeStructure::File>(fci);

			return file;
		}

		std::shared_ptr<CodeStructure::File>
			GenerateEditEntityCppFile(std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles) {

			auto generateAddComponentCode = [](std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) -> CodeStructure::Code {

				CodeStructure::Code code;

				// Components to add list.
				code.Add("const char* items[] = {");
				code.NewLine();

				std::vector<std::string> componentNames;

				for (auto parsedComponent : parsedComponents) {
					componentNames.push_back(parsedComponent->GetName());
				}

				std::sort(componentNames.begin(), componentNames.end());

				for (auto parsedComponent : componentNames) {
					code.Add(std::format("{}::GetName(),", parsedComponent));
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

			auto generateEditComponentCode = [](std::shared_ptr<ParsedComponent> component) -> CodeStructure::Code {

				CodeStructure::Code code;
				code.Add(std::format("editComponent.template operator()<{}>(world, entityId);", component->GetFullName()));
				/*code.NewLine();
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
					});*/
				return code;
				};

			auto generateEditEntityFunctionRealization = [&](std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) -> std::shared_ptr<CodeStructure::Function> {

				CodeStructure::Code code;

				code.Add(
					"const std::string idString = std::to_string(entityId);"
					"ImGui::PushID(idString.c_str());"
					"std::string name;"
					"if (world->IsComponentExist<Name>(entityId)) {"
					"auto* nameComponent = world->GetComponent<Name>(entityId);"
					"name = nameComponent->value_;"
					"}"
					"if (ImGui::CollapsingHeader((\"Id: \" + idString + \"  \" + magic_enum::enum_name(world->GetEntityType(entityId)).data() + \" \" + name).c_str())) {"
					"ImGui::Indent(20.f);"

					"auto editComponent = []<class ComponentType>(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) {"

					"bool isExist = world->IsComponentExist<ComponentType>(id);"
					"if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist)) {"
					"ComponentType* component = world->GetComponent<ComponentType>(id);"
					"Edit<ComponentType>(world, component);"
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

				CodeStructure::Function::CreateInfo editEntityFunction{
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

				auto editEntityFunctionObject = std::make_shared<CodeStructure::Function>(editEntityFunction);

				return editEntityFunctionObject;
				};

			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");


			CodeStructure::Code code;

			code.Add(
				"const std::string idString = std::to_string(entityId);"
				"ImGui::PushID(idString.c_str());"
				"std::string name;"
				"if (world->IsComponentExist<Name>(entityId)) {"
				"auto* nameComponent = world->GetComponent<Name>(entityId);"
				"name = nameComponent->value_;"
				"}"
				"if (ImGui::CollapsingHeader((\"Id: \" + idString + \"  \" + magic_enum::enum_name(world->GetEntityType(entityId)).data() + \" \" + name).c_str())) {"
				"ImGui::Indent(20.f);"

				"auto editComponent = []<class ComponentType>(std::shared_ptr<ECS2::World> world, ECS2::Entity::Id id) {"

				"bool isExist = world->IsComponentExist<ComponentType>(id);"
				"if (ImGui::CollapsingHeader(ComponentType::GetName(), &isExist)) {"
				"ComponentType* component = world->GetComponent<ComponentType>(id);"
				"Edit<ComponentType>(world, component);"
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

			//Generate 
			// {
			//	using namespace XXX;
			//	editComponent.template operator()<YYY>(world, entityId);
			// }
			// code for all components.
			for (auto ecsFile : ecsFiles) {

				ecsFile->ForEachComponent([&](
					std::vector<std::shared_ptr<ParsedTable>> childTables,
					std::shared_ptr<ParsedComponent> parsedComponent) {

						code.Add("{");
						//std::vector<std::string> namespaceStrings;
						//for (auto childTable : childTables) {
						//	if (childTable->GetType() == ParsedTable::Type::Namespace) {
						//		auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(childTable);
						//		namespaceStrings.push_back(parsedNamespace->GetName());
						//	}
						//}

						//if (!namespaceStrings.empty()) {
						//	std::string fullNamespace;
						//	for (Common::Index i = 0; i < namespaceStrings.size(); i++) {
						//		fullNamespace += namespaceStrings[i];
						//		if (i != namespaceStrings.size() - 1) {
						//			fullNamespace += "::";
						//		}
						//	}

						//	code.Add("using namespace {};", fullNamespace);
						//}

						code.Add(generateEditComponentCode(parsedComponent));
						code.Add("}");
					});

			}


			code.Add("ImGui::PopID();");
			code.Add("ImGui::SeparatorText(\"Add component\");");
			code.Add("ImGui::Indent(20.0f);");

			// Components to add list.
			// Generate
			//const char* items[] = {
			//	API::GetName(),
			//	Active::GetName(),
			//	AfterPin::GetName(),
			//	Animation::GetName(),
			//	AnimationEnded::GetName(),
			// };
			code.Add("const char* items[] = {");

			std::vector<std::string> componentNames;

			for (auto ecsFile : ecsFiles) {

				ecsFile->ForEachComponent([&](
					std::vector<std::shared_ptr<ParsedTable>> childTables,
					std::shared_ptr<ParsedComponent> parsedComponent) {

						std::vector<std::string> namespaceStrings;
						for (auto childTable : childTables) {
							if (childTable->GetType() == ParsedTable::Type::Namespace) {
								auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(childTable);
								namespaceStrings.push_back(parsedNamespace->GetName());
							}
						}

						std::string componentFullName;
						if (!namespaceStrings.empty()) {
							std::string fullNamespace;
							for (Common::Index i = 0; i < namespaceStrings.size(); i++) {
								fullNamespace += namespaceStrings[i];
								fullNamespace += "::";
							}

							componentFullName += fullNamespace;
						}
						componentFullName += parsedComponent->GetName();
						componentNames.push_back(componentFullName);
					});

			}

			std::sort(componentNames.begin(), componentNames.end());

			for (auto parsedComponent : componentNames) {
				code.Add(std::format("{}::GetName(),", parsedComponent));
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

			for (auto ecsFile : ecsFiles) {

				ecsFile->ForEachComponent([&](
					std::vector<std::shared_ptr<ParsedTable>> childTables,
					std::shared_ptr<ParsedComponent> parsedComponent) {

						std::vector<std::string> namespaceStrings;
						for (auto childTable : childTables) {
							if (childTable->GetType() == ParsedTable::Type::Namespace) {
								auto parsedNamespace = std::dynamic_pointer_cast<ParsedNamespace>(childTable);
								namespaceStrings.push_back(parsedNamespace->GetName());
							}
						}

						std::string fullNamespace;
						std::string componentFullName;
						if (!namespaceStrings.empty()) {

							for (Common::Index i = 0; i < namespaceStrings.size(); i++) {
								fullNamespace += namespaceStrings[i];
								fullNamespace += "::";
							}

							componentFullName += fullNamespace;
						}
						componentFullName += parsedComponent->GetName();

						code.Add(std::format("if (currentComponent == {}::GetName()) {{", componentFullName));
						code.Add(std::format("{}Add{}(world.get(), entityId);", fullNamespace, parsedComponent->GetName()));
						code.Add("}");
					});

			}


			code.Add("ImGui::PopID();");

			code.Add("ImGui::Unindent(20.0f);");

			code.Add("}");
			code.Add("ImGui::Separator();");
			code.Add("ImGui::Unindent(20.f);");
			code.Add("}");
			code.Add("ImGui::PopID();");

			CodeStructure::Function::CreateInfo editEntityFunction{
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

			auto editEntityFunctionObject = std::make_shared<CodeStructure::Function>(editEntityFunction);

			namespaceObject->Add(editEntityFunctionObject);

			CodeStructure::File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("imgui.h");
			includes.paths_.insert("auto_OksEngine.ECS.hpp");
			includes.paths_.insert("magic_enum/magic_enum.hpp");
			includes.paths_.insert("auto_OksEngine.EditEntity.hpp");

			//hpp file
			CodeStructure::File::CreateInfo fci{
				.isHpp_ = false,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<CodeStructure::File>(fci);

			return file;
		}

		std::shared_ptr<CodeStructure::File>
			GenerateEditEntityHppFile(std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles) {

			auto generateEditEntityFunctionPrototype = [&]() -> std::shared_ptr<CodeStructure::Function> {

				CodeStructure::Function::CreateInfo editEntityFunction{
					.name_ = "EditEntity",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world" },
						{ "ECS2::Entity::Id", "entityId" }
					},
					.returnType_ = "void",
					.code_ = {},
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto editEntityFunctionObject = std::make_shared<CodeStructure::Function>(editEntityFunction);

				return editEntityFunctionObject;
				};

			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");

			namespaceObject->Add(generateEditEntityFunctionPrototype());

			CodeStructure::File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");

			//hpp file
			CodeStructure::File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<CodeStructure::File>(fci);

			return file;
		}

		std::shared_ptr<CodeStructure::File> GenerateEditEntityHppFile(std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) {

			auto generateAddComponentCode = [](std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) -> CodeStructure::Code {

				CodeStructure::Code code;

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

			auto generateEditComponentCode = [](std::shared_ptr<ParsedComponent> component) -> CodeStructure::Code {

				CodeStructure::Code code;
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

			auto generateEditEntityFunctionRealization = [&](std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) -> std::shared_ptr<CodeStructure::Function> {

				CodeStructure::Code code;

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

				CodeStructure::Function::CreateInfo editEntityFunction{
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

				auto editEntityFunctionObject = std::make_shared<CodeStructure::Function>(editEntityFunction);

				return editEntityFunctionObject;
				};

			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");

			namespaceObject->Add(generateEditEntityFunctionRealization(parsedComponents));

			CodeStructure::File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("imgui.h");
			includes.paths_.insert("OksEngine.ECS.hpp");
			includes.paths_.insert("magic_enum/magic_enum.hpp");

			//hpp file
			CodeStructure::File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};

			auto file = std::make_shared<CodeStructure::File>(fci);

			return file;

			//auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			//std::filesystem::path includeDirFullPath = GetSubPath(randomEcsFilePath, projectContext->includeDirectory_, ECSGenerator::ResultRange::FromStartToStartFolder, SearchDirection::FromEndToBegin, false);

			//std::filesystem::path systemCppFileFullPath = includeDirFullPath / ("auto_OksEngine.EditEntity.hpp");
			//FormatPath(systemCppFileFullPath);

			//return { systemCppFileFullPath, file };
		}

		using System = std::string;
		using Component = std::string;

		class SystemsOrder {
		public:

			void AddSystemAfter(ParsedSystemPtr system, ParsedSystemPtr afterSystem) {

#pragma region Assert
				ASSERT_FMSG(afterSystem != nullptr, "");
#pragma endregion

				for (Common::Index i = 0; i < order_.size(); i++) {
					const ParsedSystemPtr& currentSystem = order_[i];
					if (currentSystem == afterSystem) {
#pragma region Assert
						ASSERT_FMSG(system != nullptr, "");
#pragma endregion
						if (i != order_.size() - 1) {
							order_.insert(order_.begin() + i + 1, system);
						}
						else {
							order_.push_back(system);
						}
					}

				}

			}

			void AddSystemAfter(ParsedSystemPtr system, std::unordered_set<ParsedSystemPtr> afterSystems) {


				std::unordered_set<ParsedSystemPtr> checkedSystems;
				for (Common::Index i = 0; i < order_.size(); i++) {
					const ParsedSystemPtr& currentSystem = order_[i];
					if (afterSystems.contains(currentSystem)) {
						checkedSystems.insert(currentSystem);
						if (checkedSystems.size() == afterSystems.size()) {
#pragma region Assert
							ASSERT_FMSG(system != nullptr, "");
#pragma endregion
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

			void AddSystem(ParsedSystemPtr system, std::unordered_set<ParsedSystemPtr> afterSystems, std::unordered_set<ParsedSystemPtr> beforeSystems) {

				auto findLastAfterSystem = [](
					const std::vector<System>& systems,
					const std::unordered_set<System>& afterSystems) {

						Common::UInt64 bestCandidate = 0;




					};

				auto findFirstBeforeSystem = [](
					const std::vector<ParsedSystemPtr>& systems,
					const std::unordered_set<ParsedSystemPtr>& afterSystems) {

					};

				std::unordered_set<ParsedSystemPtr> checkedSystems;



				for (Common::Index i = 0; i < order_.size(); i++) {
					const ParsedSystemPtr& currentSystem = order_[i];
					if (afterSystems.contains(currentSystem)) {
						checkedSystems.insert(currentSystem);
						if (checkedSystems.size() == afterSystems.size()) {
#pragma region Assert
							ASSERT_FMSG(system != nullptr, "");
#pragma endregion
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

			bool IsSystemAdded(ParsedSystemPtr system) {
				for (Common::Index i = 0; i < order_.size(); i++) {
					const ParsedSystemPtr& currentSystem = order_[i];
					if (currentSystem == system) {
						return true;
					}
				}
				return false;
			}

			std::vector<ParsedSystemPtr> order_;
		};

		struct Thread {

			//std::map<System, std::set<Component>> systems_;
			std::vector<ParsedSystemPtr> systems_;

			DS::Graph<ParsedSystemPtr> callGraph_;
			SystemsOrder systemsOrder_;
		};

		CodeStructure::Code GenerateRunSystemCode(std::shared_ptr<ParsedSystem> systemEcsFile) {

			const std::string fullSystemName = GetFullTableNameWithNamespace(systemEcsFile);
			CodeStructure::Code runSystemCode;
			runSystemCode.Add("PIXBeginEvent(PIX_COLOR(255, 0, 0), \"" + fullSystemName + "\");");
			runSystemCode.Add(fullSystemName + "System(world2);");
			runSystemCode.NewLine();
			runSystemCode.Add("PIXEndEvent();");
			return runSystemCode;
		}

		std::shared_ptr<CodeStructure::Function> GenerateRunInitSystemsFunctionRealization(std::vector<ParsedSystemPtr> parsedSystems) {

			DS::Graph<ParsedSystemPtr> initCallGraph;
			for (auto parsedSystem : parsedSystems) {
				if (parsedSystem->ci_.type_ == ParsedSystem::Type::Initialize) {
					DS::Graph<ParsedSystemPtr>::Node::Id currentSystemNodeId = DS::Graph<ParsedSystemPtr>::Node::invalidId_;
					if (!initCallGraph.IsNodeExist(parsedSystem)) {
						currentSystemNodeId = initCallGraph.AddNode(parsedSystem);
					}
					else {
						currentSystemNodeId = initCallGraph.FindNode(parsedSystem);
					}
					if (parsedSystem->ci_.callOrderInfo_ != nullptr) {
						parsedSystem->ci_.callOrderInfo_->ForEachRunAfterSystem([&](const ParsedSystem::CallOrderInfo::System& afterSystem) {
							DS::Graph<ParsedSystemPtr>::Node::Id afterSystemNodeId = DS::Graph<ParsedSystemPtr>::Node::invalidId_;

							ParsedSystemPtr afterSystemPtr = nullptr;
							for (auto parsedSystem : parsedSystems) {
								auto fullSystemName = GetFullTableNameWithNamespace(parsedSystem);
								if (afterSystem.name_ == fullSystemName) {
									afterSystemPtr = parsedSystem;
								}
							}


							if (!initCallGraph.IsNodeExist(afterSystemPtr)) {
								afterSystemNodeId = initCallGraph.AddNode(afterSystemPtr);
							}
							else {
								afterSystemNodeId = initCallGraph.FindNode(afterSystemPtr);
							}
							initCallGraph.AddLinkFromTo(afterSystemNodeId, currentSystemNodeId);
							return true;
							});
					}
					if (parsedSystem->ci_.callOrderInfo_ != nullptr) {
						parsedSystem->ci_.callOrderInfo_->ForEachRunBeforeSystem([&](const ParsedSystem::CallOrderInfo::System& beforeSystem) {

							DS::Graph<ParsedSystemPtr>::Node::Id beforeSystemNodeId = DS::Graph<ParsedSystemPtr>::Node::invalidId_;

							ParsedSystemPtr beforeSystemPtr = nullptr;
							for (auto parsedSystem : parsedSystems) {
								auto fullSystemName = GetFullTableNameWithNamespace(parsedSystem);
								if (beforeSystem.name_ == fullSystemName) {
									beforeSystemPtr = parsedSystem;
								}
							}

							if (!initCallGraph.IsNodeExist(beforeSystemPtr)) {
								beforeSystemNodeId = initCallGraph.AddNode(beforeSystemPtr);
							}
							else {
								beforeSystemNodeId = initCallGraph.FindNode(beforeSystemPtr);
							}
							initCallGraph.AddLinkFromTo(currentSystemNodeId, beforeSystemNodeId);
							return true;
							});
					}
				}
			}

			//Find systems that root of dependence and generate code.
			auto findRoots = [&](DS::Graph<ParsedSystemPtr>& graph) {
				std::unordered_set<DS::Graph<ParsedSystemPtr>::NodeId> roots;
				graph.ForEachNode([&](
					DS::Graph<ParsedSystemPtr>::NodeId systemNodeId,
					DS::Graph<ParsedSystemPtr>::Node& systemNode
					) {
						if (!systemNode.HasLinksFrom()) {
							roots.insert(systemNodeId);
						}
						return true;
					});

				return roots;
				};

			std::unordered_set<DS::Graph<ParsedSystemPtr>::Node::Id> roots = findRoots(initCallGraph);

			SystemsOrder systemsOrder;
			for (auto rootNodeId : roots) {
				auto rootNode = initCallGraph.GetNode(rootNodeId);
#pragma region Assert
				ASSERT_FMSG(rootNode.GetValue() != nullptr, "");
#pragma endregion
				systemsOrder.order_.push_back(rootNode.GetValue());
			}

			for (auto rootNodeId : roots) {
				auto rootNode = initCallGraph.GetNode(rootNodeId);
				ProcessNode(systemsOrder, initCallGraph, rootNodeId);

			}

			CodeStructure::Code runInitSystemsCode;
			runInitSystemsCode.Add(
				"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"Start initialize frame\");"
				"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"StartFrame\");"
				"world2->StartFrame();"
				"PIXEndEvent();");

			auto getECSSystemByName = [](std::vector<ParsedSystemPtr> parsedSystems, const std::string& systemName) {

				for (auto parsedSystem : parsedSystems) {
					if (parsedSystem->GetName() == systemName) {
						return parsedSystem;
					}
				}

				OS::AssertFail();

				return std::shared_ptr<ParsedSystem>{};
				};

			//Generate code to run systems that process all entities.
			for (auto& systemName : systemsOrder.order_) {
				auto parsedSystem = systemName;

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
			CodeStructure::Function::CreateInfo cppRunSystemsFunction{
				.name_ = "RunInitializeSystems",
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "world2" }
				},
				.returnType_ = "void",
				.code_ = runInitSystemsCode,
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto runInitializeSystemsFunctionRealization = std::make_shared<CodeStructure::Function>(cppRunSystemsFunction);

			return runInitializeSystemsFunctionRealization;

		}

		std::shared_ptr<CodeStructure::Function> GenerateCreateThreadRealization(const std::vector<Thread>& threads) {


			//Create threads
			CodeStructure::Code cppCreateThreadsCode;
			{

				for (Common::Index i = 0; i < threads.size(); ++i) {
					auto& thread = threads[i];
					const std::string threadIdStr = std::to_string(i);
					cppCreateThreadsCode.AddComment("Thread " + threadIdStr);
					////Add comment: what are components in use.
					//std::set<std::string> threadComponents;
					//for (auto& system : thread.systems_) {
					//	for (auto& component : system.second) {
					//		threadComponents.insert(component);
					//	}
					//}
					//for (auto& threadComponent : threadComponents) {
					//	cppCreateThreadsCode.AddComment(threadComponent);
					//}

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


					CodeStructure::Code runThreadSystems;
					for (Common::Index i = 0; i < thread.systemsOrder_.order_.size(); i++) {
						runThreadSystems.Add(std::format(
							"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
							"{}System(world2);"
							"PIXEndEvent();",
							GetFullTableNameWithNamespace(thread.systemsOrder_.order_[i]),
							GetFullTableNameWithNamespace(thread.systemsOrder_.order_[i])
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
			CodeStructure::Function::CreateInfo cppCreateThreadsFunction{
				.name_ = "CreateThreads",
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "world2" }
				},
				.returnType_ = "void",
				.code_ = cppCreateThreadsCode,
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto cppCreateThreadsFunctionObject = std::make_shared<CodeStructure::Function>(cppCreateThreadsFunction);

			return cppCreateThreadsFunctionObject;
		}

		std::shared_ptr<CodeStructure::File> GenerateRunSystemsHppFile() {

			auto generateRunSystemsFunctionPrototype = []() -> std::shared_ptr<CodeStructure::Function> {
				CodeStructure::Function::CreateInfo hppRunSystemsFunction{
					.name_ = "RunSystems",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world2"}},
					.returnType_ = "void",
					.code_ = {},
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				return std::make_shared<CodeStructure::Function>(hppRunSystemsFunction);
				};

			auto generateCreateThreadsFunctionPrototype = []() -> std::shared_ptr<CodeStructure::Function> {
				//Create threads method prototype.
				CodeStructure::Function::CreateInfo hppCreateThreadsFunction{
					.name_ = "CreateThreads",
					.parameters_ = { { "std::shared_ptr<ECS2::World>", "world2"} },
					.returnType_ = "void",
					.code_ = {},
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				return std::make_shared<CodeStructure::Function>(hppCreateThreadsFunction);
				};

			auto generateRunInitSystemsFunctionPrototype = []() -> std::shared_ptr<CodeStructure::Function> {
				//CreateThreads method prototype.
				CodeStructure::Function::CreateInfo cppRunSystemsFunction{
					.name_ = "RunInitializeSystems",
					.parameters_ = {
						{ "std::shared_ptr<ECS2::World>", "world2" }
					},
					.returnType_ = "void",
					.code_ = {},
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				return std::make_shared<CodeStructure::Function>(cppRunSystemsFunction);
				};

			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");

			namespaceObject->Add(generateRunSystemsFunctionPrototype());
			namespaceObject->Add(generateCreateThreadsFunctionPrototype());
			namespaceObject->Add(generateRunInitSystemsFunctionPrototype());

			CodeStructure::File::Includes includes{ };
			includes.paths_.insert("boost/asio/thread_pool.hpp");
			includes.paths_.insert("boost/asio/post.hpp");
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("auto_OksEngine.ECS.hpp");

			//hpp file
			CodeStructure::File::CreateInfo fci{
			.isHpp_ = true,
			.includes_ = includes,
			.base_ = namespaceObject
			};

			auto file = std::make_shared<CodeStructure::File>(fci);

			return file;

		}

		std::shared_ptr<CodeStructure::File> GenerateUtilsHppFile(std::vector<std::shared_ptr<ParsedECSFile>> parsedECSFiles) {

			auto namespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");


			{
				CodeStructure::Code code;
				code.Add("\n#define FOR_EACH_COMPONENT(prefix, postfix) \\\n");

				for (auto parsedECSFile : parsedECSFiles) {
					parsedECSFile->ForEachComponent([&](ParsedComponentPtr parsedComponent) {

						using namespace std::string_literals;
						std::string format = "\tprefix {} postfix \\\n";
						code.Add(format, parsedComponent->GetFullName());

						return true;
						});
				}

				auto defineForEachComponent = std::make_shared<CodeStructure::CodeBlock>(code);

				namespaceObject->Add(defineForEachComponent);

			}
			{
				CodeStructure::Code code;
				code.Add("\n#define COMPONENTS_LIST_NAMES() \\\n");

				for (auto parsedECSFile : parsedECSFiles) {
					parsedECSFile->ForEachComponent([&](ParsedComponentPtr parsedComponent) {

						using namespace std::string_literals;
						std::string format = "\t{}::GetName(),\\\n";
						code.Add(format, parsedComponent->GetFullName());

						return true;
						});
				}

				auto defineForEachComponent = std::make_shared<CodeStructure::CodeBlock>(code);

				namespaceObject->Add(defineForEachComponent);

			}

			{
				CodeStructure::Code code;

				for (auto parsedECSFile : parsedECSFiles) {
					parsedECSFile->ForEachComponent([&](ParsedComponentPtr parsedComponent) {

						code.Add("\nif({} == {}::GetTypeId()) {{\n", "componentTypeId", parsedComponent->GetFullName());
						code.Add("	return {}::GetName();\n", parsedComponent->GetFullName());
						code.Add("}\n");

						return true;
						});
				}


				code.Add("ASSERT_FAIL_MSG(\"Attempt to use type that is not ECS component type.\");");
				code.Add("return nullptr;");

				CodeStructure::Function::CreateInfo cppRunSystemsFunction{
					.name_ = "GetComponentNameByTypeId",
					.parameters_ = {
						{ "ECS2::ComponentTypeId", "componentTypeId" }
					},
					.returnType_ = "const char*",
					.code_ = {code},
					.isPrototype_ = false,
					.inlineModifier_ = true
				};

				namespaceObject->Add(std::make_shared<CodeStructure::Function>(cppRunSystemsFunction));
			}

			{
				CodeStructure::Code code;

				for (auto parsedECSFile : parsedECSFiles) {
					parsedECSFile->ForEachComponent([&](ParsedComponentPtr parsedComponent) {

						code.Add("\nif({} == {}::GetName()) {{\n", "componentName", parsedComponent->GetFullName());
						const std::string concatedNamespace = parsedComponent->GetConcatedNamespace();
						if (concatedNamespace != "") {
							code.Add("	{}::Add{}(world, entityId);\n", concatedNamespace, parsedComponent->GetName());
						}
						else {
							code.Add("	Add{}(world, entityId);\n", parsedComponent->GetName());
						}
						code.Add("}\n");

						return true;
						});
				}

				CodeStructure::Function::CreateInfo cppRunSystemsFunction{
					.name_ = "AddComponentWithName",
					.parameters_ = {
						{ "const char*", "componentName" },
						{ "ECS2::World*", "world" },
						{ "ECS2::Entity::Id", "entityId" }
					},
					.returnType_ = "void",
					.code_ = {code},
					.isPrototype_ = false,
					.inlineModifier_ = true
				};

				namespaceObject->Add(std::make_shared<CodeStructure::Function>(cppRunSystemsFunction));
			}

			{
				CodeStructure::Code code;

				for (auto parsedECSFile : parsedECSFiles) {
					parsedECSFile->ForEachComponent([&](ParsedComponentPtr parsedComponent) {

						code.Add("\nif({} == {}::GetName()) {{\n", "name", parsedComponent->GetFullName());
						code.Add("	return {}::GetTypeId();\n", parsedComponent->GetFullName());
						code.Add("}\n");

						return true;
						});
				}
				code.Add("ASSERT_FAIL_MSG(\"Attempt to use component name that is not component.\");");
				code.Add("return 0;");

				CodeStructure::Function::CreateInfo cppRunSystemsFunction{
					.name_ = "GetComponentTypeIdByName",
					.parameters_ = {
						{ "const char*", "name" }
					},
					.returnType_ = "ECS2::ComponentTypeId",
					.code_ = {code},
					.isPrototype_ = false,
					.inlineModifier_ = true,
					.descriptionComment_ = "Input component name must be pointer taked from call Component::GetName()!!!"
				};

				namespaceObject->Add(std::make_shared<CodeStructure::Function>(cppRunSystemsFunction));
			}

			CodeStructure::File::Includes includes{ };
			includes.paths_.insert("ECS2.World.hpp");
			includes.paths_.insert("auto_OksEngine.ECS.hpp");

			//hpp file
			CodeStructure::File::CreateInfo fci{
			.isHpp_ = true,
			.includes_ = includes,
			.base_ = namespaceObject,
			.isFormat_ = false // Do not format this file because formater work bad with multiline #define
			};

			auto file = std::make_shared<CodeStructure::File>(fci);

			return file;

		}

		void ProcessNode(
			SystemsOrder& systemsOrder,
			const DS::Graph<ParsedSystemPtr>& graph,
			const DS::Graph<ParsedSystemPtr>::Node::Id& nodeId,
			bool isFrom = false) {

			auto& node = graph.GetNode(nodeId);
			if (node.HasLinks()) {
				isFrom = isFrom;
			}
			std::unordered_set<ParsedSystemPtr> systemsFrom;
			node.ForEachLinksFrom([&](DS::Graph<ParsedSystemPtr>::Node::Id fromNodeToId) {
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
				std::unordered_set<DS::Graph<ParsedSystemPtr>::Node::Id> roots;
				thread.callGraph_.ForEachNode([&](
					const DS::Graph<ParsedSystemPtr>::Node::Id systemNodeId,
					const DS::Graph<ParsedSystemPtr>::Node& systemNode
					) {
						if (!systemNode.HasLinksFrom()) {
							roots.insert(systemNodeId);
						}
						return true;
					});
				return roots;
				};

			std::unordered_set<DS::Graph<ParsedSystemPtr>::Node::Id> roots = findRoots(thread);

			SystemsOrder systemsOrder;
			for (auto rootNodeId : roots) {
				auto rootNode = thread.callGraph_.GetNode(rootNodeId);

#pragma region Assert
				ASSERT_FMSG(rootNode.GetValue() != nullptr, "");
#pragma endregion

				thread.systemsOrder_.order_.push_back(rootNode.GetValue());
			}

			for (auto rootNodeId : roots) {
				auto rootNode = thread.callGraph_.GetNode(rootNodeId);
				ProcessNode(thread.systemsOrder_, thread.callGraph_, rootNodeId);

			}

		}

		std::shared_ptr<CodeStructure::File>
			GenerateRunSystemsCppFile(std::vector<std::shared_ptr<ParsedECSFile>> parsedECSFiles) {

			////Systems and which components they use.
			//std::map<ParsedSystemPtr, std::set<ParsedComponentPtr>> componentSystem;
			//{
			//	for (auto parsedECSFile : parsedECSFiles) {
			//		parsedECSFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {

			//			if (parsedSystem->IsInitializeSystem()) {
			//				return true;
			//			}

			//			const std::string systemName = parsedSystem->GetName();
			//			for (auto& entity : parsedSystem->ci_.updateMethod_->processesEntities_) {
			//				entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {

			//					const std::string includeName = include.GetName();

			//					//"Behaviour::ScriptName::X" -> "Behaviour", "ScriptName", "X"
			//					auto parseIncludeName = [](const std::string& name) {
			//						std::vector<std::string> result;
			//						size_t start = 0;
			//						size_t end = name.find("::");

			//						while (end != std::string::npos) {
			//							result.push_back(name.substr(start, end - start));
			//							start = end + 2; // Пропускаем "::"
			//							end = name.find("::", start);
			//						}

			//						// Добавляем последнюю часть
			//						result.push_back(name.substr(start));

			//						return result;
			//						};

			//					
			//					//if (componentSystem.find(include.name_) != componentSystem.end()) {
			//					//	componentSystem.insert({});
			//					//}
			//					//componentSystem[include.name_].insert(systemName);
			//					return true;
			//					});
			//			}
			//			return true;
			//			});
			//	}
			//}

			////Systems and which components they use
			//std::map<std::string, std::set<std::string>> systemComponents;
			//{
			//	for (auto& [component, systems] : componentSystem) {
			//		for (const auto& system : systems) {
			//			//systemComponents[system].insert(component);
			//		}
			//	}
			//}

			//Separate systems on threads.
			std::vector<Thread> childThreads;

			Thread childThread;
			Thread mainThread;
			for (auto parsedECSFile : parsedECSFiles) {
				parsedECSFile->ForEachSystem(
					[&](std::shared_ptr<ParsedSystem> parsedSystem) {
						if (parsedSystem->ci_.type_ == ParsedSystem::Type::FrameToFrame) {

							if (parsedSystem->GetThread() == ParsedSystem::Thread::Child) {
								childThread.systems_.push_back(parsedSystem);
							}
							if (parsedSystem->GetThread() == ParsedSystem::Thread::Main) {
								mainThread.systems_.push_back(parsedSystem);
							}
						}
						return true;
					});
			}
			childThreads.push_back(childThread);

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

					auto getCreateSystemNode = [&](DS::Graph<ParsedSystemPtr>& graph, ParsedSystemPtr parsedSystem) {
						DS::Graph<System>::Node::Id nodeId = DS::Graph<System>::Node::invalidId_;


						if (!graph.IsNodeExist(parsedSystem)) {
							nodeId = graph.AddNode(parsedSystem);
						}
						else {
							nodeId = graph.FindNode(parsedSystem);
						}
						return nodeId;
						};

					DS::Graph<System>::Node::Id currentSystemNodeId = getCreateSystemNode(thread.callGraph_, system);

					if (system->ci_.callOrderInfo_ != nullptr) {
						system->ci_.callOrderInfo_->ForEachRunAfterSystem([&](const ParsedSystem::CallOrderInfo::System& afterSystem) {
							auto parsedSystem = afterSystem.ptr_;

#pragma region Assert
							ASSERT_FMSG(parsedSystem != nullptr, "");
#pragma endregion

							DS::Graph<System>::Node::Id afterSystemNodeId = getCreateSystemNode(thread.callGraph_, parsedSystem);
							thread.callGraph_.AddLinkFromTo(afterSystemNodeId, currentSystemNodeId);
							return true;
							});
					}
					if (system->ci_.callOrderInfo_ != nullptr) {
						system->ci_.callOrderInfo_->ForEachRunBeforeSystem([&](const ParsedSystem::CallOrderInfo::System& beforeSystem) {

							auto parsedSystem = beforeSystem.ptr_;
#pragma region Assert
							ASSERT_FMSG(parsedSystem != nullptr, "");
#pragma endregion
							DS::Graph<System>::Node::Id beforeSystemNodeId = getCreateSystemNode(thread.callGraph_, parsedSystem);


							thread.callGraph_.AddLinkFromTo(currentSystemNodeId, beforeSystemNodeId);
							return true;
							});
					}
				}

				///CREATE GRAPHVIZ CALL GRAPH

				// �������� ������ �����
				Agraph_t* g = agopen((char*)"G", Agstrictdirected, nullptr);

				thread.callGraph_.ForEachNode([&](DS::Graph<ParsedSystemPtr>::NodeId nodeId, DS::Graph<ParsedSystemPtr>::Node& node) {

					if (node.HasLinksFrom() || node.HasLinksTo()) {

						Agnode_t* gSystemNode = agnode(g, (char*)node.GetValue()->GetFullName().c_str(), 1);
						agsafeset(gSystemNode, (char*)"shape", (char*)"rect", (char*)"");

						node.ForEachLinksFrom([&](DS::Graph<System>::NodeId nodeId) {
							const DS::Graph<ParsedSystemPtr>::Node& fromNode = thread.callGraph_.GetNode(nodeId);

							Agnode_t* gFromNode = agnode(g, (char*)fromNode.GetValue()->GetFullName().c_str(), 1);

							Agedge_t* gEdge = agedge(g, gFromNode, gSystemNode, nullptr, 1);

							return true;
							});

						node.ForEachLinksTo([&](DS::Graph<System>::NodeId nodeId) {
							const DS::Graph<ParsedSystemPtr>::Node& toNode = thread.callGraph_.GetNode(nodeId);

							Agnode_t* gToNode = agnode(g, (char*)toNode.GetValue()->GetFullName().c_str(), 1);

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
						//auto randomEcsFilePath = parsedECSFiles[0]->GetPath();

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
						//Get path

						static int clusterIndex = 0;
						auto dotfile = std::make_shared<OS::TextFile>("D:/OksEngine/auto_ECSSystemsCallGraph" + std::to_string(clusterIndex) + ".dot");
						clusterIndex++;
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
					////

				};

			for (auto& childThread : childThreads) {
				createClusterSystemsCallGraph(childThread);
				CalculateSystemsCallOrder(childThread);
			}
			createClusterSystemsCallGraph(mainThread);
			CalculateSystemsCallOrder(mainThread);


			//Group threads systems to get less threads.
			//{
			//	const Common::UInt64 threadsNumber = 10;// projectContext->GetConfig()->GetValueAs<Common::UInt64>("Multithreading.threadsNumber");

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


			using namespace std::string_literals;

			//Generate .CPP.

			//Generate Includes.
			CodeStructure::File::Includes cppIncludes{ };
			{
				//Must be first to escape 
				//error C1189: #error:  WinSock.h has already been included
				cppIncludes.paths_.insert("auto_OksEngine.RunSystems.hpp");

				/*for (auto ecsFile : parsedECSFiles) {
					ecsFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {

						std::filesystem::path systemIncludePath = GetSubPath(
							ecsFile->GetPath().parent_path(),
							projectContext->includeDirectory_,
							ResultRange::FromStartFolderToEnd,
							SearchDirection::FromEndToBegin,
							true);
						cppIncludes.paths_.insert(systemIncludePath / ("auto_OksEngine." + parsedSystem->GetName() + ".hpp"));

						});
				}*/



				cppIncludes.paths_.insert("pix3.h");
			}

			auto cppNamespaceObject = std::make_shared<CodeStructure::Namespace>("OksEngine");

			{

				for (Common::Index i = 0; i < childThreads.size(); ++i) {
					auto& thread = childThreads[i];

					CodeStructure::Variable::CreateInfo threadVariable{
						.type_ = "std::thread",
						.name_ = "thread" + std::to_string(i)
					};
					cppNamespaceObject->Add(std::make_shared<CodeStructure::Variable>(threadVariable));

					std::mutex thread16Mutex;

					CodeStructure::Variable::CreateInfo threadMutexVariable{
						.type_ = "std::mutex",
						.name_ = "thread" + std::to_string(i) + "Mutex"
					};
					cppNamespaceObject->Add(std::make_shared<CodeStructure::Variable>(threadMutexVariable));

					CodeStructure::Variable::CreateInfo runSystemThreadVariable{
						.type_ = "std::atomic_bool",
						.name_ = "runSystemThread" + std::to_string(i),
						.initValue_ = "false"
					};
					cppNamespaceObject->Add(std::make_shared<CodeStructure::Variable>(runSystemThreadVariable));

					CodeStructure::Variable::CreateInfo threadCVVariable{
						.type_ = "std::condition_variable",
						.name_ = "thread" + std::to_string(i) + "CV"
					};
					cppNamespaceObject->Add(std::make_shared<CodeStructure::Variable>(threadCVVariable));
				}

				CodeStructure::Variable::CreateInfo threadCVVariable{
						.type_ = "Common::UInt64",
						.name_ = "frameNumber",
						.initValue_ = "0"
				};
				cppNamespaceObject->Add(std::make_shared<CodeStructure::Variable>(threadCVVariable));
			}

			CodeStructure::Code cppRunSystemsCode;
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
					CodeStructure::Code runMainThreadSystems;
					for (Common::Index i = 0; i < mainThread.systemsOrder_.order_.size(); i++) {
						runMainThreadSystems.Add(std::format(
							"PIXBeginEvent(PIX_COLOR(255, 0, 0), \"{}\");"
							"{}System(world2);"
							"PIXEndEvent();",
							GetFullTableNameWithNamespace(mainThread.systemsOrder_.order_[i]),
							GetFullTableNameWithNamespace(mainThread.systemsOrder_.order_[i])
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
			CodeStructure::Function::CreateInfo cppRunSystemsFunction{
				.name_ = "RunSystems",
				.parameters_ = {
					{ "std::shared_ptr<ECS2::World>", "world2" }
				},
				.returnType_ = "void",
				.code_ = cppRunSystemsCode,
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto cppRunSystemsFunctionObject = std::make_shared<CodeStructure::Function>(cppRunSystemsFunction);
			cppNamespaceObject->Add(cppRunSystemsFunctionObject);

			std::vector<std::shared_ptr<ParsedSystem>> parsedSystems;
			for (auto parsedECSFile : parsedECSFiles) {
				parsedECSFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {
					parsedSystems.push_back(parsedSystem);
					return true;
					});
			}

			cppNamespaceObject->Add(GenerateRunInitSystemsFunctionRealization(parsedSystems));
			cppNamespaceObject->Add(GenerateCreateThreadRealization(childThreads));


			//cpp file
			CodeStructure::File::CreateInfo cppfci{
			.isHpp_ = false,
			.includes_ = cppIncludes,
			.base_ = cppNamespaceObject
			};

			auto cppFile = std::make_shared<CodeStructure::File>(cppfci);

			return cppFile;

		}

		CreateInfo ci_;
	};


}