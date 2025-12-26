#pragma once

#include <System/ECSGenerator2.ParsedSystem.hpp>

#include <ECSGenerator2.Common.hpp>

namespace ECSGenerator2 {


	class SystemStructureGenerator {
	public:

		struct CreateInfo {
			std::filesystem::path includeDirectory_;
		};

		SystemStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		std::shared_ptr<CodeStructure::Function> GenerateUpdateMethodRealization(
			const std::string& systemName,
			std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethodInfo) {

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
				= generateUpdateMethodParameters(updateMethodInfo);

			CodeStructure::Function::CreateInfo updateMethodCI{
				.name_ = systemName + "::Update",
				.parameters_ = updateMethodParameters,
				.returnType_ = "void",
				.code_ = "",
				.isPrototype_ = false,
				.inlineModifier_ = false
			};

			auto updateMethod = std::make_shared<CodeStructure::Function>(updateMethodCI);

			return updateMethod;

		}

		//returns array of names of components that needed for Update method.
		std::unordered_set<ParsedComponentPtr> GenerateUpdateMethodRequiredComponentIncludes(
			std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethodInfo) {

			std::unordered_set<ParsedComponentPtr> requiredComponentNames{ };

			for (auto& createsEntities : updateMethodInfo->createsEntities_) {
				for (auto& createsEntityComponent : createsEntities.creates_) {
					requiredComponentNames.insert(createsEntityComponent.ptr_);
				}
			}
			updateMethodInfo->ForEachRandomAccessEntity([&](
				ParsedSystem::RandomAccessEntity& entity,
				bool isLast) {

					ParsedArchetype::ProcessComponentRecursive processComponentName = [&](ParsedArchetype::Component& component) {
						
						ASSERT(component.ptr_->GetType() == ParsedTable::Type::Component);

						requiredComponentNames.insert(Common::pointer_cast<ParsedComponent>(component.ptr_));
						
						};

					if (entity.archetype_ != nullptr) {
						entity.archetype_->ptr_->ForEachComponentRecursive(processComponentName);
					}
					entity.ForEachInclude([&](ParsedSystem::Include& include, bool isLast) {
						requiredComponentNames.insert(include.ptr_);
						return true;
						});

					return true;
				});

			updateMethodInfo->ForEachCreateEntity([&](
				ParsedSystem::CreatesEntity& entity,
				bool isLast) {

					for (auto& create : entity.creates_) {
						requiredComponentNames.insert(create.ptr_);
					}

					return true;
				});

			for (auto& entity : updateMethodInfo->processesEntities_) {
				for (auto componentInclude : entity.includes_) {
					requiredComponentNames.insert(componentInclude.ptr_);
				}
				for (auto componentCreates : entity.creates_) {
					requiredComponentNames.insert(componentCreates.ptr_);
				}
				for (auto componentRemoves : entity.removes_) {
					requiredComponentNames.insert(componentRemoves.ptr_);
				}
				for (auto componentExclude : entity.excludes_) {
					requiredComponentNames.insert(componentExclude.ptr_);
				}
			}


#pragma region Assert

			ASSERT_FMSG(!requiredComponentNames.contains(nullptr), "");
#pragma endregion


			return requiredComponentNames;
		}

		CodeStructure::File::Includes GenerateUpdateMethodIncludes(
			const std::string& includeDirectory,
			const std::string& systemName,
			const std::filesystem::path& systemEcsFilePath) {

			CodeStructure::File::Includes includes{ };


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

		std::vector<CodeStructure::Function::Parameter> GenerateUpdateMethodParameters(std::shared_ptr<ParsedSystem::UpdateMethodInfo> updateMethod) {



			std::vector<CodeStructure::Function::Parameter> updateMethodParameters;

			Common::UInt64 currentEntityIndex = 0;
			updateMethod->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {

				updateMethodParameters.push_back({
					"ECS2::Entity::Id",
					std::format("entity{}id", currentEntityIndex) });

				entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {


					CodeStructure::Function::Parameter parameter;
					if (include.IsReadonly()) {
						parameter.inputType_ = std::format("const {}*", include.GetFullName());
					}
					else {
						parameter.inputType_ = std::format("{}*", include.GetFullName());
					}

					parameter.valueName_ = std::format("{}{}", include.GetLowerName(), currentEntityIndex);
					updateMethodParameters.push_back(parameter);
					return true;
					});

				++currentEntityIndex;
				return true;
				});

			return updateMethodParameters;
		}

		std::shared_ptr<CodeStructure::Struct> GenerateSystemStructCode(std::shared_ptr<ParsedSystem> system) {

			if (system->GetName() == "DrawNodes") {
				Common::BreakPointLine();
			}

			auto generateGetComponentsFilter = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateEntity method.
				CodeStructure::Function::CreateInfo createDynamicEntityCI{
					.name_ = "GetComponentsFilter",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "ECS2::ComponentsFilter",
					.code_ = "return world_->GetEntityComponentsFilter(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto getEntityComponentsFilterMethod = std::make_shared<CodeStructure::Function>(createDynamicEntityCI);

				return getEntityComponentsFilterMethod;
				};
			auto generateAfterUpdateMethodPrototype = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				CodeStructure::Function::CreateInfo updateMethodCI{
					.name_ = "AfterUpdate",
					.parameters_ = { },
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<CodeStructure::Function>(updateMethodCI);

				return updateMethod;

				};
			auto generateUpdateMethodPrototype = [&](std::shared_ptr<ParsedSystem> systemEcsFile) {

				using namespace std::string_literals;

				if (system->GetName() == "CreateLuaScriptEntity") {
					Common::BreakPointLine();
				}

				std::vector<CodeStructure::Function::Parameter> updateMethodParameters = GenerateUpdateMethodParameters(system->ci_.updateMethod_);

				CodeStructure::Function::CreateInfo updateMethodCI{
					.name_ = "Update",
					.parameters_ = updateMethodParameters,
					.returnType_ = "void",
					.code_ = "",
					.isPrototype_ = true,
					.inlineModifier_ = false
				};

				auto updateMethod = std::make_shared<CodeStructure::Function>(updateMethodCI);

				return updateMethod;

				};
			auto generateCreateDynamicEntityRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateEntity method.
				CodeStructure::Function::CreateInfo createDynamicEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "return world_->CreateEntity();",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto createDynamicEntityMethod = std::make_shared<CodeStructure::Function>(createDynamicEntityCI);

				return createDynamicEntityMethod;
				};
			auto generateDestroyEntityRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//CreateEntity method.
				CodeStructure::Function::CreateInfo createDynamicEntityCI{
					.name_ = "DestroyEntity",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }},
					.returnType_ = "void",
					.code_ = "world_->DestroyEntity(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false
				};

				auto createDynamicEntityMethod = std::make_shared<CodeStructure::Function>(createDynamicEntityCI);

				return createDynamicEntityMethod;
				};
			auto generateCreateArchetypeEntityRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {


				//CreateEntity<> method.
				CodeStructure::Function::CreateInfo createArchetypeEntityCI{
					.name_ = "CreateEntity",
					.parameters_ = { },
					.returnType_ = "ECS2::Entity::Id",
					.code_ = "return world_->CreateEntity<Components...>();",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto createArchetypeEntityMethod = std::make_shared<CodeStructure::Function>(createArchetypeEntityCI);

				return createArchetypeEntityMethod;
				};
			
			
			auto generateGetComponentsMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				CodeStructure::Code getComponentsCode;
				{

					if (!systemEcsFile->ci_.updateMethod_->randomAccessesEntities_.empty()) {
						//Add Assert.
						{
							getComponentsCode.Add("STATIC_ASSERT_MSG(");
							getComponentsCode.Add("(");

							getComponentsCode.Add("Common::IsAnyOf<Components, ");

							systemEcsFile->ci_.updateMethod_->ForEachRandomAccessComponentName(
								[&](const std::string& componentName, bool isLast) {
									getComponentsCode.Add(componentName);
									if (!isLast) {
										getComponentsCode.Add(",");
									}
									return true;
								});
							getComponentsCode.Add(">() && ...");
							getComponentsCode.Add(")");
							getComponentsCode.Add(
								", \"Attempt to access component that system("
								+ systemEcsFile->GetName() +
								") can't access. Added access entities description to .ecs file that corresponds to system\");"
							);
						}
					}
					getComponentsCode.Add("return world_->GetComponents<Components...>();");
				}	
				CodeStructure::Function::CreateInfo getComponentsCI{
					.name_ = "GetComponents",
					.parameters_ = {},
					.returnType_ = "std::tuple<ECS2::Entity::Id*, Components*...>",
					.code_ = getComponentsCode,
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto getComponentsMethod = std::make_shared<CodeStructure::Function>(getComponentsCI);

				return getComponentsMethod;
				};

			auto generateGetEntityComponentsMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				CodeStructure::Code getComponentsCode;
				{
					if (!systemEcsFile->ci_.updateMethod_->randomAccessesEntities_.empty()) {
						//Add Assert.
						{
							getComponentsCode.Add("STATIC_ASSERT_MSG(");
							getComponentsCode.Add("(");

							getComponentsCode.Add("Common::IsAnyOf<Components, ");

							systemEcsFile->ci_.updateMethod_->ForEachRandomAccessComponentName(
								[&](const std::string& componentName, bool isLast) {
									getComponentsCode.Add(componentName);
									if (!isLast) {
										getComponentsCode.Add(",");
									}
									return true;
								});
							getComponentsCode.Add(">() && ...");
							getComponentsCode.Add(")");
							getComponentsCode.Add(
								", \"Attempt to access component that system("
								+ systemEcsFile->GetName() +
								") can't access. Added access entities description to .ecs file that corresponds to system\");"
							);
						}
					}
					getComponentsCode.Add("return world_->GetComponents<Components...>(entityId);");
				}
				CodeStructure::Function::CreateInfo getComponentsCI{
					.name_ = "GetComponents",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "std::tuple<Components*...>",
					.code_ = getComponentsCode,
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto getComponentsMethod = std::make_shared<CodeStructure::Function>(getComponentsCI);

				return getComponentsMethod;
				};



			auto generateGetComponentMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				CodeStructure::Code getComponentCode;
				{
					if (!systemEcsFile->ci_.updateMethod_->randomAccessesEntities_.empty()) {
						//Add Assert.
						{
							getComponentCode.Add("STATIC_ASSERT_MSG(");
							getComponentCode.Add("(");
							systemEcsFile->ci_.updateMethod_->ForEachRandomAccessComponentName(
								[&](const std::string& componentName, bool isLast) {
									getComponentCode.Add(std::format("std::is_same_v<Component, {}>", componentName));
									if (!isLast) {
										getComponentCode.Add(" || ");
									}
									return true;
								});
							getComponentCode.Add(")");
							getComponentCode.Add(
								", \"Attempt to access component{} that system("
								+ systemEcsFile->GetName() +
								") can't access. Added access entities description to .ecs file that corresponds to system\");"
							);
						}
					}
					getComponentCode.Add("return world_->GetComponent<Component>(entityId);");
				}
				CodeStructure::Function::CreateInfo getComponentCI{
					.name_ = "GetComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "Component*",
					.code_ = getComponentCode,
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component" }
				};

				auto getComponentMethod = std::make_shared<CodeStructure::Function>(getComponentCI);

				return getComponentMethod;
				};
			auto generateCreateComponentMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {


				CodeStructure::Code createComponentCode;
				//If entity is archetype check if entity can contain this component.
				{
					createComponentCode.Add("ASSERT_FMSG("
						"(world_->IsArchetypeEntity(entityId)) ?"
						"(world_->GetArchetypeComponents(entityId).IsSet<Component>()) :"
						"(true), \"Attempt to create component {} for archetype entity that can't containt this component.\", Component::GetName());");
				}

				createComponentCode.Add("world_->CreateComponent<Component>(entityId,\"" + systemEcsFile->GetFullName() + "\", std::forward<Args>(args)...);");

				//CreateComponent method.
				CodeStructure::Function::CreateInfo createComponentCI{
					.name_ = "CreateComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
					.returnType_ = "void",
					.code_ = createComponentCode,
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component", "...Args" }
				};

				auto createComponentMethod = std::make_shared<CodeStructure::Function>(createComponentCI);

				return createComponentMethod;
				};
			auto generateRemoveComponentMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				CodeStructure::Code expression;
				{
					std::vector<std::string> componentNamesToRemove;

					systemEcsFile->ci_.updateMethod_->ForEachProcessEntity([&](ParsedSystem::ProcessedEntity& processedEntity, bool isLast) {
						processedEntity.ForEachRemove([&](ParsedSystem::Remove& remove, bool isLast) {
							componentNamesToRemove.push_back(remove.ptr_->GetFullName());
							return true;
							});
						return true;
						});

					systemEcsFile->ci_.updateMethod_->ForEachRandomAccessEntity([&](ParsedSystem::RandomAccessEntity& randomAccessEntity, bool isLast) {
						randomAccessEntity.ForEachRemove([&](ParsedSystem::Remove& remove, bool isLast) {
							componentNamesToRemove.push_back(remove.ptr_->GetFullName());
							return true;
							});
						return true;
						});

					for (Common::Index i = 0; i < componentNamesToRemove.size(); i++) {
						std::string componentNameToRemove = componentNamesToRemove[i];
						expression.Add(std::format("std::is_same_v<Component, {}>", componentNameToRemove));
						if (!(i == componentNamesToRemove.size() - 1)) {
							expression.Add(" || ");
						}
					}

				}

				CodeStructure::Code removeComponentRealization;
				{
					if (!expression.code_.empty()) {
						//Add Assert.
						{
							removeComponentRealization.Add("STATIC_ASSERT_MSG(");
							removeComponentRealization.Add("(" + expression.code_ + ")");
							removeComponentRealization.Add(
								",\"Attempt to remove component that system("
								+ systemEcsFile->GetName() +
								") can't remove. Added access entities description to .ecs file that corresponds to system\");"
							);
						}
					}
					removeComponentRealization.Add("world_->RemoveComponent<Component>(entityId, \"" + systemEcsFile->GetFullName() + "\",std::forward<Args>(args)...);");
				}

				//CreateComponent method.
				CodeStructure::Function::CreateInfo removeComponentCI{
					.name_ = "RemoveComponent",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" }, { "Args&&", "...args"}},
					.returnType_ = "void",
					.code_ = removeComponentRealization,//"world_->RemoveComponent<Component>(entityId, \"" + systemEcsFile->GetFullName() + "\",std::forward<Args>(args)...);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component", "...Args" }
				};

				auto removeComponentMethod = std::make_shared<CodeStructure::Function>(removeComponentCI);

				return removeComponentMethod;
				};
			auto generateIsComponentExistMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//IsComponentExist method.
				CodeStructure::Function::CreateInfo isComponentExistCI{
					.name_ = "IsComponentExist",
					.parameters_ = { { "ECS2::Entity::Id", "entityId" } },
					.returnType_ = "bool",
					.code_ = "return world_->IsComponentExist<Component>(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "Component" }
				};

				auto isComponentExistMethod = std::make_shared<CodeStructure::Function>(isComponentExistCI);

				return isComponentExistMethod;
				};
			auto generateIsEntityExistMethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//IsEntityExist method.
				CodeStructure::Function::CreateInfo isEntityExistCI{
					.name_ = "IsEntityExist",
					.parameters_ = { },
					.returnType_ = "bool",
					.code_ = "return world_->IsEntityExist<Components...>();",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { "...Components" }
				};

				auto isEntityExistMethod = std::make_shared<CodeStructure::Function>(isEntityExistCI);

				return isEntityExistMethod;
				};
			auto generateIsEntityExist2MethodRealization = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				//IsEntityExist method.
				CodeStructure::Function::CreateInfo isEntityExistCI{
					.name_ = "IsEntityExist",
					.parameters_ = { { "ECS2::Entity::Id", "entityId"} },
					.returnType_ = "bool",
					.code_ = "return world_->IsEntityExist(entityId);",
					.isPrototype_ = false,
					.inlineModifier_ = false,
					.templateParameters_ = { }
				};

				auto isEntityExistMethod = std::make_shared<CodeStructure::Function>(isEntityExistCI);

				return isEntityExistMethod;
				};

			std::vector<std::shared_ptr<CodeStructure::Function>> methods{
				generateUpdateMethodPrototype(system),
				generateGetComponentsFilter(system),

				generateIsComponentExistMethodRealization(system),

				generateDestroyEntityRealization(system),

				generateIsEntityExistMethodRealization(system),
				generateIsEntityExist2MethodRealization(system)
			};


			if (system->ci_.afterUpdateMethod_ != nullptr) {
				methods.push_back(generateAfterUpdateMethodPrototype(system));
			}

			if (!system->ci_.updateMethod_->randomAccessesEntities_.empty()) {
				methods.push_back(generateGetComponentsMethodRealization(system));
				methods.push_back(generateGetComponentMethodRealization(system));
				methods.push_back(generateGetEntityComponentsMethodRealization(system));
			}

			//Generate CreateComponent method if needed.
			{
				//bool isSystemCreatesComponents = false;

				////If update method creates compponents for processed entities -> generate CreateComponent method.
				//system->ci_.updateMethod_->ForEachProcessEntity(
				//	[&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {
				//	
				//		if (!entity.creates_.empty()) {
				//			isSystemCreatesComponents = true;
				//		}
				//		if (isSystemCreatesComponents) {
				//			return false;
				//		}
				//		return true;
				//	
				//	});

				////If update method creates entities -> generate CreateComponent method. 
				//if (!isSystemCreatesComponents && !system->ci_.updateMethod_->createsEntities_.empty()) {
				//	isSystemCreatesComponents = true;
				//}

				if (system->ci_.updateMethod_->IsCreatesComponents()) {
					methods.push_back(generateCreateComponentMethodRealization(system));
				}

				if (system->ci_.updateMethod_->IsRemovesComponents()) {
					methods.push_back(generateRemoveComponentMethodRealization(system));
				}



			}

			//Generate create entity method if needed.
			{
				//If update method creates entities -> generate CreateComponent method. 
				if (!system->ci_.updateMethod_->createsEntities_.empty()) {
					methods.push_back(generateCreateDynamicEntityRealization(system));
					methods.push_back(generateCreateArchetypeEntityRealization(system));
				}
			}

			CodeStructure::Struct::CreateInfo sci{
				.name_ = system->GetName(),
				.parent_ = "",
				.fields_ = { { "std::shared_ptr<ECS2::World>", "world" } },
				.methods_ = methods
			};
			auto structObject = std::make_shared<CodeStructure::Struct>(sci);

			return structObject;
		}

		std::shared_ptr<CodeStructure::Function> GenerateUpdateMethodRealization(std::shared_ptr<ParsedSystem> system) {

			auto generateUpdateMethodParameters =
				[](std::shared_ptr<ParsedSystem> system) {

				std::vector<CodeStructure::Function::Parameter> updateMethodParameters;

				Common::UInt64 currentEntityIndex = 0;
				system->ci_.updateMethod_->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {

					updateMethodParameters.push_back({
						"ECS2::Entity::Id",
						std::format("entity{}id", currentEntityIndex) });

					entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
						CodeStructure::Function::Parameter parameter;
						if (include.IsReadonly()) {
							parameter.inputType_ = std::format("const {}*", include.GetFullName());
						}
						else {
							parameter.inputType_ = std::format("{}*", include.GetFullName());
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
				= generateUpdateMethodParameters(system);

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
		}

		std::shared_ptr<CodeStructure::Function> GenerateRunSystemCodeRealization(std::shared_ptr<ParsedSystem> system) {

			auto generateUpdateMethodCallCode = [](std::shared_ptr<ParsedSystem> systemEcsFile) {

				CodeStructure::Code realization;

				Common::UInt64 currentEntityIndex = 0;
				realization.Add("BEGIN_PROFILE( \"{} Call\");", systemEcsFile->GetName());
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


				realization.Add("END_PROFILE();");

				return realization;
				};


			CodeStructure::Code realization;
			//"BEGIN_PROFILE( \"{}\");"
			//	"{}System(world2);"
			//	"END_PROFILE();",
			//	GetFullTableNameWithNamespace(mainThread.systemsOrder_.order_[i]),
			//	GetFullTableNameWithNamespace(mainThread.systemsOrder_.order_[i])
			
			//Render render{ world };
			realization.Add("MT_ASSERT();");
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
						realization.Add(entity.excludes_[i].GetFullName());
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

						realization.Add(include.GetFullName());
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

							include.GetFullName(), include.GetLowerName(), currentEntityIndex));
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


			std::vector<CodeStructure::Function::Parameter> parameters;
			parameters.push_back({ "std::shared_ptr<ECS2::World>", "world" });

			CodeStructure::Function::CreateInfo funcci{
				.name_ = system->GetName() + "System",
				.parameters_ = parameters,
				.returnType_ = "void",
				//Save run system function code realization to validate system .ecs description.
				.code_ = (system->ci_.isEnabled_) ? (realization) : (CodeStructure::Code{""}),
				.inlineModifier_ = true
			};
			auto runSystem = std::make_shared<CodeStructure::Function>(funcci);

			return runSystem;

		}

		CreateInfo ci_;
	};


}