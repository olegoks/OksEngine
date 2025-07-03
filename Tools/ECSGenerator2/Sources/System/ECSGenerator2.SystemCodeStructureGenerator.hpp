#pragma once

#include <System/ECSGenerator2.ParsedSystem.hpp>

#include <ECSGenerator2.Common.hpp>

namespace ECSGenerator2 {


	class SystemStructureGenerator {
	public:

		struct CreateInfo {
			std::string includeDirectory_;
		};

		SystemStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		std::shared_ptr<Function> GenerateUpdateMethodRealization(
			const std::string& systemName,
			std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethodInfo) {

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
				= generateUpdateMethodParameters(updateMethodInfo);

			Function::CreateInfo updateMethodCI{
				.name_ = systemName + "::Update",
				.parameters_ = updateMethodParameters,
				.returnType_ = "void",
				.code_ = "",
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto updateMethod = std::make_shared<Function>(updateMethodCI);

			return updateMethod;

		}

		//returns array of names of components that needed for Update method.
		std::vector<std::string> GenerateUpdateMethodRequiredComponentIncludes(std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethodInfo) {

			std::vector<std::string> requiredComponentNames{ };

			for (auto& createsEntities : updateMethodInfo->createsEntities_) {
				for (auto& createsEntityComponent : createsEntities) {
					requiredComponentNames.push_back(createsEntityComponent);
				}
			}

			updateMethodInfo->ForEachRandomAccessEntity([&](
				const ParsedSystem::RandomAccessEntity& entity,
				bool isLast) {

					entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
						requiredComponentNames.push_back(include.GetName());
						return true;
						});

					return true;
				});



			for (auto& entity : updateMethodInfo->processesEntities_) {
				for (auto componentInclude : entity.includes_) {
					requiredComponentNames.push_back(componentInclude.GetName());
				}
				for (auto componentCreates : entity.creates_) {
					requiredComponentNames.push_back(componentCreates);
				}

				for (auto componentRemoves : entity.removes_) {
					requiredComponentNames.push_back(componentRemoves);
				}
				for (auto componentExclude : entity.excludes_) {
					requiredComponentNames.push_back(componentExclude);
				}
			}

			return requiredComponentNames;
		}


		File::Includes GenerateUpdateMethodIncludes(
			const std::string& includeDirectory,
			const std::string& systemName,
			const std::filesystem::path& systemEcsFilePath) {

			File::Includes includes{ };


			const std::string systemName = systemName;
			std::filesystem::path systemsIncludesFilePath;
			std::filesystem::path systemFullPath = systemEcsFilePath;
			systemsIncludesFilePath = systemFullPath.parent_path();
			std::filesystem::path systemIncludePath = GetSubPath(
				systemFullPath.parent_path(),
				includeDirectory,
				ResultRange::FromStartFolderToEnd,
				SearchDirection::FromEndToBegin,
				true);

			systemIncludePath /= ("auto_OksEngine." + systemName + ".hpp");
			includes.paths_.insert(systemIncludePath);

			return includes;
		}



		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> GenerateUpdateMethodRealization(std::shared_ptr<ProjectContext> projectContext, std::shared_ptr<ParsedSystem> systemEcsFile) {


			//const File::Includes includes = GenerateUpdateMethodIncludes(
			//	projectContext->includeDirectory_,
			//	systemEcsFile->GetName(),
			//	systemEcsFile->GetEcsFilePath());

			auto namespaceObject = std::make_shared<Namespace>("OksEngine");
			{
				using namespace std::string_literals;



				namespaceObject->Add(GenerateUpdateMethodRealization(projectContext, systemEcsFile));

				//auto generateAfterUpdateMethod = [](
				//	std::shared_ptr<ProjectContext> projectContext,
				//	std::shared_ptr<ParsedSystem> systemEcsFile) {

				//		Function::CreateInfo afterUpdateMethodCI{
				//			.name_ = systemEcsFile->GetName() + "::AfterUpdate",
				//			.parameters_ = { },
				//			.returnType_ = "void",
				//			.code_ = "",
				//			.isPrototype_ = false,
				//			.inlineModifier_ = false
				//		};

				//		auto afterUpdateMethod = std::make_shared<Function>(afterUpdateMethodCI);

				//		return afterUpdateMethod;

				//	};

				if (systemEcsFile->ci_.afterUpdateMethod_) {
					namespaceObject->Add(generateAfterUpdateMethod(projectContext, systemEcsFile));
				}
			}

			File::CreateInfo fci{
				.isHpp_ = false,
				.includes_ = includes,
				.base_ = namespaceObject
			};
			auto file = std::make_shared<File>(fci);

			std::filesystem::path componentHppFileFullPath
				= systemEcsFile->GetEcsFilePath().parent_path() / ("OksEngine." + systemEcsFile->GetName() + ".cpp");
			std::string componentHppFileFullPathString = componentHppFileFullPath.string();
			std::replace(componentHppFileFullPathString.begin(), componentHppFileFullPathString.end(), '\\', '/');
			return { std::filesystem::path{ componentHppFileFullPathString }, file };

		}

		std::vector<Function::Parameter> GenerateUpdateMethodParameters(std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethod) {

			std::vector<Function::Parameter> updateMethodParameters;

			Common::UInt64 currentEntityIndex = 0;
			updateMethod->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {

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

		}

		std::shared_ptr<Struct> GenerateSystemStructCode(std::shared_ptr<ParsedSystem> system) {

			auto generateGetComponentsFilter = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateEntity method.
				Function::CreateInfo createDynamicEntityCI{
					.name_ = "GetComponentsFilter",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "ECS2::ComponentsFilter",
					.code_ = "return world_->GetEntityComponentsFilter(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto getEntityComponentsFilterMethod = std::make_shared<Function>(createDynamicEntityCI);

				return getEntityComponentsFilterMethod;
				};
			auto generateAfterUpdateMethodPrototype = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				Function::CreateInfo updateMethodCI{
					.name_ = "AfterUpdate",
					.parameters_ = { },
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				return updateMethod;

				};
			auto generateUpdateMethodPrototype = [&](std::shared_ptr<ParsedSystem> systemEcsFile) {

				using namespace std::string_literals;

				std::vector<Function::Parameter> updateMethodParameters = GenerateUpdateMethodParameters(system->ci_.updateMethod_);

				Function::CreateInfo updateMethodCI{
					.name_ = "Update",
					.parameters_ = updateMethodParameters,
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<Function>(updateMethodCI);

				return updateMethod;

				};
			auto generateCreateDynamicEntityRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateEntity method.
				Function::CreateInfo createDynamicEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "return world_->CreateEntity();",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto createDynamicEntityMethod = std::make_shared<Function>(createDynamicEntityCI);

				return createDynamicEntityMethod;
				};
			auto generateDestroyEntityRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateEntity method.
				Function::CreateInfo createDynamicEntityCI{
					.name_ = "DestroyEntity",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }},
					.returnType_ = "void",
					.code_ = "world_->DestroyEntity(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto createDynamicEntityMethod = std::make_shared<Function>(createDynamicEntityCI);

				return createDynamicEntityMethod;
				};
			auto generateCreateArchetypeEntityRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {


				//CreateEntity<> method.
				Function::CreateInfo createArchetypeEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "return world_->CreateEntity<Components...>();",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto createArchetypeEntityMethod = std::make_shared<Function>(createArchetypeEntityCI);

				return createArchetypeEntityMethod;
				};
			auto generateGetComponentMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				Code getComponentCode;
				{
					if (!systemEcsFile->ci_.updateMethod_->randomAccessesEntities_.empty()) {
						//Add Assert.
						getComponentCode.Add("OS::AssertMessage(");
						systemEcsFile->ci_.updateMethod_->ForEachRandomAccessComponent(
							[&](const std::string& componentName, bool isLast) {
								getComponentCode.Add(std::format("std::is_same_v<Component, {}>", componentName));
								if (!isLast) {
									getComponentCode.Add(" || ");
								}
								return true;
							});
						getComponentCode.Add(
							",std::format(\"Attempt to access component{} that system("
							+ systemEcsFile->GetName() +
							") can't access. Added access entities description to .ecs file that corresponds to system\", Component::GetName()));"
						);
					}
					getComponentCode.Add("return world_->GetComponent<Component>(entityId);");
				}
				Function::CreateInfo getComponentCI{
					.name_ = "GetComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "Component*",
					.code_ = getComponentCode,
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component" }
				};

				auto getComponentMethod = std::make_shared<Function>(getComponentCI);

				return getComponentMethod;
				};
			auto generateCreateComponentMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateComponent method.
				Function::CreateInfo createComponentCI{
					.name_ = "CreateComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
					.returnType_ = "void",
					.code_ = "world_->CreateComponent<Component>(entityId, std::forward<Args>(args)...);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component", "...Args" }
				};

				auto createComponentMethod = std::make_shared<Function>(createComponentCI);

				return createComponentMethod;
				};
			auto generateRemoveComponentMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateComponent method.
				Function::CreateInfo removeComponentCI{
					.name_ = "RemoveComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
					.returnType_ = "void",
					.code_ = "world_->RemoveComponent<Component>(entityId, std::forward<Args>(args)...);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component", "...Args" }
				};

				auto removeComponentMethod = std::make_shared<Function>(removeComponentCI);

				return removeComponentMethod;
				};
			auto generateIsComponentExistMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//IsComponentExist method.
				Function::CreateInfo isComponentExistCI{
					.name_ = "IsComponentExist",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "bool",
					.code_ = "return world_->IsComponentExist<Component>(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component" }
				};

				auto isComponentExistMethod = std::make_shared<Function>(isComponentExistCI);

				return isComponentExistMethod;
				};
			auto generateIsEntityExistMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//IsEntityExist method.
				Function::CreateInfo isEntityExistCI{
					.name_ = "IsEntityExist",
					.parameters_ = { },
					.returnType_ = "bool",
					.code_ = "return world_->IsEntityExist<Components...>();",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto isEntityExistMethod = std::make_shared<Function>(isEntityExistCI);

				return isEntityExistMethod;
				};
			auto generateIsEntityExist2MethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//IsEntityExist method.
				Function::CreateInfo isEntityExistCI{
					.name_ = "IsEntityExist",
					.parameters_ = { { "ECS2::Entity::Id", "entityId"} },
					.returnType_ = "bool",
					.code_ = "return world_->IsEntityExist(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { }
				};

				auto isEntityExistMethod = std::make_shared<Function>(isEntityExistCI);

				return isEntityExistMethod;
				};

			std::vector<std::shared_ptr<Function>> methods{
				generateUpdateMethodPrototype(system),
				generateCreateComponentMethodRealization(system),
				generateGetComponentsFilter(system),
				generateRemoveComponentMethodRealization(system),
				generateGetComponentMethodRealization(system),
				generateIsComponentExistMethodRealization(system),
				generateCreateDynamicEntityRealization(system),
				generateDestroyEntityRealization(system),
				generateCreateArchetypeEntityRealization(system),
				generateIsEntityExistMethodRealization(system),
				generateIsEntityExist2MethodRealization(system)
			};

			Struct::CreateInfo sci{
				.name_ = system->GetName(),
				.parent_ = "",
				.fields_ = { { "std::shared_ptr<ECS2::World>", "world" } },
				.methods_ = methods
			};
			auto structObject = std::make_shared<Struct>(sci);
		}

		std::shared_ptr<Function> GenerateRunSystemCodeRealization(std::shared_ptr<ParsedSystem> system) {

			auto generateUpdateMethodCallCode = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				Code realization;

				Common::UInt64 currentEntityIndex = 0;
				realization.Add(systemEcsFile->GetLowerName() + ".Update(");
				systemEcsFile->ci_.updateMethod_->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {

					realization.Add(std::format("entity{}Id", currentEntityIndex));

					if (!entity.includes_.empty()) {
						realization.Add(", ");
					}
					realization.NewLine();
					entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
						realization.Add(std::format("{}{}",
							include.GetLowerName(), currentEntityIndex));
						if (!isLast) {
							realization.Add(", ");
						}
						return true;
						});
					if (!isLast) {
						realization.Add(", ");
					}
					++currentEntityIndex;
					return true;
					});

				realization.Add(");");

				return realization;
				};


			Code realization;
			//Render render{ world };
			realization.Add(std::format(
				"{} {}{{ world }};",
				system->GetName(),
				system->GetLowerName()
			));

			Common::UInt64 currentEntityIndex = 0;
			system->ci_.updateMethod_->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {

				//Generate exclude.
				{
					realization.Add(std::format("ECS2::ComponentsFilter excludeEntity{};", currentEntityIndex));
					realization.NewLine();
					if (!entity.excludes_.empty()) {
						realization.Add(std::format("excludeEntity{}.SetBits<", currentEntityIndex));
					}


					for (Common::Index i = 0; i < entity.excludes_.size(); ++i) {
						realization.Add(entity.excludes_[i]);
						if (i != entity.excludes_.size() - 1) {
							realization.Add(", ");
						}

					}
					if (!entity.excludes_.empty()) {
						realization.Add(">();");
						realization.NewLine();
					}
				}


				//Generate ForEachEntity
				if (entity.processesAllCombinations_) {
					realization.Add(std::format("world->ForEachEntity([&](ECS2::Entity::Id entity{}Id) {{", currentEntityIndex));

				}
				else {
					realization.Add("world->ForEachEntity<");
					realization.NewLine();
					entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
						realization.Add(include.name_);
						if (!isLast) {
							realization.Add(", ");
						}
						realization.NewLine();
						return true;
						});
					realization.Add(std::format(">(excludeEntity{}, [&](", currentEntityIndex));
					realization.Add(std::format("ECS2::Entity::Id entity{}Id, ", currentEntityIndex));
					entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
						realization.Add(std::format("{}*{}{}",

							include.name_, include.GetLowerName(), currentEntityIndex));
						if (!isLast) {
							realization.Add(", ");
						}
						realization.NewLine();
						return true;
						});
					realization.Add("){");
				}

				//Generate Update method if its last entity;
				if (currentEntityIndex == system->ci_.updateMethod_->processesEntities_.size() - 1) {
					realization.Add(generateUpdateMethodCallCode(system));
				}

				++currentEntityIndex;
				return true;
				});

			system->ci_.updateMethod_->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {
				realization.Add("});");
				return true;
				});

			if (currentEntityIndex == 0) {
				//If there are no entities just call update method.
				realization.Add(system->GetLowerName() + ".Update();");
			}

			if (system->ci_.afterUpdateMethod_) {
				realization.Add(std::format("{}.AfterUpdate();", system->GetLowerName()));
			}

			std::vector<Function::Parameter> parameters;
			parameters.push_back({ "std::shared_ptr<ECS2::World>", "world" });

			Function::CreateInfo funcci{
				.name_ = system->GetName() + "System",
				.parameters_ = parameters,
				.returnType_ = "void",
				.code_ = realization,
				.inlineModifier_ = true
			};
			auto runSystem = std::make_shared<Function>(funcci);

			return runSystem;

		}

		std::pair<
			std::filesystem::path,
			std::shared_ptr<File>> GenerateECSCXXFilesStructure(
				std::shared_ptr<ProjectContext> projectContext,
				std::shared_ptr<ParsedSystem> systemEcsFile) {
					{


						namespaceObject->Add(structObject);





						Code realization;

						realization.Add(generateRunSystemCode(systemEcsFile));




						namespaceObject->Add(runSystem);


						File::CreateInfo fci{
							.isHpp_ = true,
							.includes_ = includes,
							.base_ = namespaceObject
						};
						auto file = std::make_shared<File>(fci);

						std::filesystem::path systemHppFileFullPath
							= systemEcsFile->GetEcsFilePath().parent_path() / ("auto_OksEngine." + systemEcsFile->GetName() + ".hpp");
						std::string systemHppFileFullPathString = systemHppFileFullPath.string();
						std::replace(systemHppFileFullPathString.begin(), systemHppFileFullPathString.end(), '\\', '/');
						return { std::filesystem::path{ systemHppFileFullPathString }, file };


					}



					CreateInfo ci_;
		};


	}