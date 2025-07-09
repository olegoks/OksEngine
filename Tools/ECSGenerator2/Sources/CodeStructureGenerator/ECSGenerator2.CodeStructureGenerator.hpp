#pragma once 

#include <System/ECSGenerator2.SystemCodeStructureGenerator.hpp>
#include <Component/ECSGenerator2.ComponentCodeGenerator.hpp>
#include <ECSGenerator2.CodeGenerator.hpp>

namespace ECSGenerator2 {

	class CodeStructureGenerator {

	public:

		struct CreateInfo {
			std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles_;
		};


		CodeStructureGenerator(const CreateInfo& ci) : ci_{ ci } {

		}

		std::shared_ptr<File> GenerateECSFileHppFile(
			const std::filesystem::path& includeDirectory,
			const std::shared_ptr<ParsedECSFile> parsedECSFile) {

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");

			File::Includes includes;
			includes.paths_.insert("ECS2.hpp");
			includes.paths_.insert("chrono");
			includes.paths_.insert("OksEngine.IComponent.hpp");

			std::unordered_set<std::string> includeComponents;

			parsedECSFile->ForEachComponent(
				[&](std::shared_ptr<ParsedComponent> parsedComponent) {

					ComponentCodeStructureGenerator::CreateInfo ccgci{
						.includeDirectory_ = includeDirectory
					};

					ComponentCodeStructureGenerator componentGenerator{ ccgci };

					//Add include components.
					auto componentStruct = componentGenerator.GenerateComponentStruct(parsedComponent);
					namespaceObject->Add(componentStruct);
					auto addFunction = componentGenerator.GenerateAddFunction(parsedComponent);
					namespaceObject->Add(addFunction);
					auto editFunction = componentGenerator.GenerateEditFunction(parsedComponent);
					namespaceObject->Add(editFunction);
					auto templateEditFunction = componentGenerator.GenerateTemplateEditFunction(parsedComponent);
					namespaceObject->Add(templateEditFunction);
					auto bindFunction = componentGenerator.GenerateBindFunction(parsedComponent);
					namespaceObject->Add(bindFunction);
					auto parseFunction = componentGenerator.GenerateParseFunction(parsedComponent);
					namespaceObject->Add(parseFunction);
					auto serializeFunction = componentGenerator.GenerateSerializeFunction(parsedComponent);
					namespaceObject->Add(serializeFunction);

					auto componentIncludes = componentGenerator.GenerateIncludes(parsedComponent);
					includes.paths_.insert(componentIncludes.paths_.begin(), componentIncludes.paths_.end());
				});

			parsedECSFile->ForEachSystem(
				[&](std::shared_ptr<ParsedSystem> parsedSystem) {

					SystemStructureGenerator::CreateInfo ssgci{
						.includeDirectory_ = includeDirectory
					};

					SystemStructureGenerator systemGenerator{ ssgci };

					if (parsedSystem->GetName() == "CallUpdateMethod") {
						Common::BreakPointLine();
					}
					//Add include components.
					auto systemIncludeComponents = systemGenerator.GenerateUpdateMethodRequiredComponentIncludes(parsedSystem->ci_.updateMethod_);

					includeComponents.insert(systemIncludeComponents.begin(), systemIncludeComponents.end());

					//Generate system structure.
					auto systemStruct = systemGenerator.GenerateSystemStructCode(parsedSystem);
					namespaceObject->Add(systemStruct);

					//Generate run system function.
					auto runSystemFunction = systemGenerator.GenerateRunSystemCodeRealization(parsedSystem);
					namespaceObject->Add(runSystemFunction);

				});

			//Generate includes using include directory.
			for (const std::string componentName : includeComponents) {
				for (auto ecsFile : ci_.ecsFiles_) {
					//Skip component include if ecs file contains defintion of the component.
					if (parsedECSFile->IsContainsComponent(componentName)) {
						continue;
					}
					if (ecsFile->IsContainsComponent(componentName)) {
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

						includes.paths_.insert((componentIncludePath / ("auto_OksEngine." + componentName)).string() + ".hpp");

					}
				}

			}

			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			return { file };
		}


		std::shared_ptr<File> GenerateECSFileCppFile(
			const std::filesystem::path& includeDirectory,
			const std::shared_ptr<ParsedECSFile> parsedECSFile) {

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");
			{
				using namespace std::string_literals;

				auto generateUpdateMethod = [](std::shared_ptr<ParsedSystem> system) {

					auto generateUpdateMethodParameters =
						[](std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethodInfo) {

								std::vector<Function::Parameter> updateMethodParameters;

								Common::UInt64 currentEntityIndex = 0;
								updateMethodInfo->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {

									updateMethodParameters.push_back({
										"ECS2::Entity::Id",
										std::format("entity{}id", currentEntityIndex) });

									entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
										Function::Parameter parameter;
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
					const std::vector<Function::Parameter> updateMethodParameters
						= generateUpdateMethodParameters(system->ci_.updateMethod_);

					Function::CreateInfo updateMethodCI{
						.name_ = system->GetName() + "::Update",
						.parameters_ = updateMethodParameters,
						.returnType_ = "void",
						.code_ = "",
						.isPrototype_ = false,
						.inlineModifier_ = false
					};

					auto updateMethod = std::make_shared<Function>(updateMethodCI);

					return updateMethod;
					};
				auto generateAfterUpdateMethod = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

						Function::CreateInfo afterUpdateMethodCI{
							.name_ = systemEcsFile->GetName() + "::AfterUpdate",
							.parameters_ = { },
							.returnType_ = "void",
							.code_ = "",
							.isPrototype_ = false,
							.inlineModifier_ = false
						};

						auto afterUpdateMethod = std::make_shared<Function>(afterUpdateMethodCI);

						return afterUpdateMethod;

					};

				parsedECSFile->ForEachSystem([&](std::shared_ptr<ParsedSystem> parsedSystem) {

					namespaceObject->Add(generateUpdateMethod(parsedSystem));
					if (parsedSystem->ci_.afterUpdateMethod_ != nullptr) {
						namespaceObject->Add(generateAfterUpdateMethod(parsedSystem));
					}

					});

			}

			File::Includes includes;
			auto ecsFilePath = parsedECSFile->GetPath();

			//auto [it, _] = std::mismatch(
			//	ecsFilePath.begin(), ecsFilePath.end(),
			//	includeDirectory.begin()
			//);
			//std::filesystem::path filePath;

			//while (it != ecsFilePath.end()) {
			//	filePath /= *it;
			//	++it;
			//}

			includes.paths_.insert(ecsFilePath.parent_path() / ("auto_OksEngine." + parsedECSFile->GetName() + ".cpp"));

			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			return { file };
		}

		std::vector<std::shared_ptr<File>> Generate(
			const std::filesystem::path& includeDirectory,
			const std::shared_ptr<ParsedECSFile> parsedECSFile) {

			auto hppSystemFile = GenerateECSFileHppFile(includeDirectory, parsedECSFile);
			auto cppSystemFile = GenerateECSFileCppFile(includeDirectory, parsedECSFile);
			return { hppSystemFile, cppSystemFile };
		}

		void ProcessDirectory(
			const std::filesystem::path currentDir,
			const std::filesystem::path includeDirectory,
			std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::File>>& pathToHppECSFile) {

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



			File::Includes includes;

			// Включаем .ecs файлы текущей директории
			for (const auto& file : ecs_files) {
				includes.paths_.insert(filePath.string() + "/" + "auto_" + file.stem().string() + ".hpp");
			}
			// Включаем OksEngine.Components.hpp из поддиректорий
			for (const auto& subdir : subdirs) {
				includes.paths_.insert((filePath / subdir).string() + "/auto_OksEngine.ECS.hpp");
			}

			File::CreateInfo fci{
				.isHpp_ = true,
				.includes_ = includes,
				.base_ = nullptr
			};
			auto codeStructureFile = std::make_shared<File>(fci);

			pathToHppECSFile[currentDir / "auto_OksEngine.ECS.hpp"] = codeStructureFile;

		}

		//Generate auto_OksEngine.ECS.hpp files in each dir.
		std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::File>>
			GenerateDirECSIncludeHppFiles(const std::filesystem::path currentDir, const std::filesystem::path includeDirectory) {

			std::map<std::filesystem::path, std::shared_ptr<ECSGenerator2::File>> hppECSFiles;

			ProcessDirectory(currentDir, includeDirectory, hppECSFiles);

			return hppECSFiles;

		}

		std::shared_ptr<File> 
			GenerateParseEntityHppFile(std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles) {

			auto generateParseComponentCode = [](std::shared_ptr<ParsedComponent> component) -> Code {

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

			auto generateParseEntityFunctionRealization = [&](std::vector<std::shared_ptr<ParsedComponent>> parsedComponents) -> std::shared_ptr<Function> {

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
				for (auto parsedComponent : parsedComponents) {

					if (!parsedComponent->ci_.serializable_) {
						continue;
					}

					code.Add("{");
					code.Add(generateParseComponentCode(parsedComponent));
					code.Add("}");

					code.NewLine();

				}

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

			std::vector<std::shared_ptr<ParsedComponent>> allParsedComponents;
			for (auto ecsFile : ecsFiles) {
				ecsFile->ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {
					allParsedComponents.push_back(parsedComponent);
					});
			}

			namespaceObject->Add(generateParseEntityFunctionRealization(allParsedComponents));

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

		std::shared_ptr<File>
			GenerateSerializeEntityHppFile(std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles) {

			auto generateSerializeComponentCode = [](std::shared_ptr<ParsedComponent> component, bool isLast) -> Code {

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

			auto generateSerializeEntityFunctionRealization = [&](std::vector<std::shared_ptr<ParsedComponent>> components) -> std::shared_ptr<Function> {

				Code code;

				code.Add("std::string serializedEntity;");
				code.Add("const ECS2::ComponentsFilter components = world->GetEntityComponentsFilter(entityId);");
				code.Add("serializedEntity += \"\\t\\tID = \" + std::to_string(entityId) + \",\\n\";");
				//Generate serialize components.
				std::vector<std::shared_ptr<ParsedComponent>> allParsedComponents;
				for (auto ecsFile : ecsFiles) {
					ecsFile->ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {
						allParsedComponents.push_back(parsedComponent);
						});
				}
				for (Common::Index i = 0; i < components.size(); i++) {

					auto component = components[i];

					if (!component->ci_.serializable_) {
						continue;
					}
					code.Add("{");
					code.Add(generateSerializeComponentCode(
						component,
						(i == components.size() - 1)));
					code.Add("}");

					code.NewLine();

				}

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

			std::vector<std::shared_ptr<ParsedComponent>> allParsedComponents;
			for (auto ecsFile : ecsFiles) {
				ecsFile->ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {
					allParsedComponents.push_back(parsedComponent);
					});
			}
			namespaceObject->Add(generateSerializeEntityFunctionRealization(allParsedComponents));

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

			return file;
		}

		std::shared_ptr<File> 
			GenerateEditEntityHppFile(std::vector<std::shared_ptr<ParsedECSFile>> ecsFiles) {

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
					"if (ImGui::CollapsingHeader((\"Id: \" + idString + \"  \" + name + \" \" + magic_enum::enum_name(world->GetEntityType(entityId)).data()).c_str())) {"
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

			std::vector<std::shared_ptr<ParsedComponent>> allParsedComponents;
			for (auto ecsFile : ecsFiles) {
				ecsFile->ForEachComponent([&](std::shared_ptr<ParsedComponent> parsedComponent) {
					allParsedComponents.push_back(parsedComponent);
					});
			}

			namespaceObject->Add(generateEditEntityFunctionRealization(allParsedComponents));

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
					if (parsedSystem->ci_.callOrderInfo_ != nullptr) {
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
					}
					if (parsedSystem->ci_.callOrderInfo_ != nullptr) {
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

				return std::shared_ptr<ParsedSystem>{};
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
			includes.paths_.insert("auto_OksEngine.ECS.hpp");

			//hpp file
			File::CreateInfo fci{
			.isHpp_ = true,
			.includes_ = includes,
			.base_ = namespaceObject
			};

			auto file = std::make_shared<File>(fci);

			return file;

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

		std::shared_ptr<File>
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
						if (parsedSystem->ci_.type_ == ParsedSystem::Type::FrameToFrame) {


							if (parsedSystem->GetThread() == ParsedSystem::Thread::Child) {
								childThread.systems_.push_back(parsedSystem);
							}
							if (parsedSystem->GetThread() == ParsedSystem::Thread::Main) {
								mainThread.systems_.push_back(parsedSystem);
							}
						}
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

					DS::Graph<System>::Node::Id currentSystemNodeId = getCreateSystemNode(thread.callGraph_, system->GetName());

					if (system->ci_.callOrderInfo_ != nullptr) {
						system->ci_.callOrderInfo_->ForEachRunAfterSystem([&](const std::string& afterSystem) {
#pragma region Assert 
							//OS::AssertMessage(thread.systems_.contains(afterSystem), "Current thread doesn't contain After System:" + afterSystem);
#pragma endregion		
							DS::Graph<System>::Node::Id afterSystemNodeId = getCreateSystemNode(thread.callGraph_, afterSystem);
							thread.callGraph_.AddLinkFromTo(afterSystemNodeId, currentSystemNodeId);
							return true;
							});
					}
					if (system->ci_.callOrderInfo_ != nullptr) {
						system->ci_.callOrderInfo_->ForEachRunBeforeSystem([&](const std::string& beforeSystem) {
#pragma region Assert 
							//OS::AssertMessage(thread.systems_.contains(beforeSystem), "Current thread doesn't contain Before System:" + beforeSystem);
#pragma endregion		
							DS::Graph<System>::Node::Id beforeSystemNodeId = getCreateSystemNode(thread.callGraph_, beforeSystem);
							thread.callGraph_.AddLinkFromTo(currentSystemNodeId, beforeSystemNodeId);
							return true;
							});
					}
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
			File::Includes cppIncludes{ };
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

			return cppFile;

		}

		CreateInfo ci_;
	};


}