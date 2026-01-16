#pragma once 

#include <filesystem>
#include <string>
#include <functional>

#include <Common.hpp>
#include <OS.Logger.hpp>

#include <ECSGenerator2.ParsedTable.hpp>
#include <Component/ECSGenerator2.ParsedComponent.hpp>
#include <Archetype/ECSGenerator2.ParsedArchetype.hpp>
#include <ECSGenerator2.Code.hpp>

namespace ECSGenerator2 {


	class ParsedSystem : public ParsedTable {
	public:

		struct Dependence {
			std::shared_ptr<ParsedComponent> ptr_ = nullptr;

			std::string GetFullName() const {
				auto mergeArraysPreserveOrder = [](const std::vector<std::string>& arr1,
					const std::vector<std::string>& arr2) {
						std::vector<std::string> result;
						std::unordered_set<std::string> seen;

						// Сначала добавляем все уникальные элементы из первого массива
						for (const auto& str : arr1) {
							if (seen.find(str) == seen.end()) {
								result.push_back(str);
								seen.insert(str);
							}
						}

						// Затем добавляем уникальные элементы из второго массива
						for (const auto& str : arr2) {
							if (seen.find(str) == seen.end()) {
								result.push_back(str);
								seen.insert(str);
							}
						}

						return result;
					};


				const auto namespaceStrings = GetComponentNamespace(ptr_);


				const auto parsedComponentName = ECSGenerator2::ParseFullName(ptr_->GetName());
				const auto componentFullName = mergeArraysPreserveOrder(namespaceStrings, parsedComponentName);

				std::string fullNamespace;
				for (Common::Index i = 0; i < componentFullName.size(); i++) {
					fullNamespace += componentFullName[i];
					if (i != componentFullName.size() - 1) {
						fullNamespace += "::";
					}
				}

				return "OksEngine::" + fullNamespace;

			}

		};

		struct Archetype {
			std::string name_ = "";
			ParsedArchetypePtr ptr_ = nullptr;
		};

		struct Exclude : public Dependence {
			std::string name_;

			[[nodiscard]]
			const std::string& GetName() const noexcept {
				return name_;
			}
		};
		
		struct Create : public Dependence {
			std::string name_;

			[[nodiscard]]
			const std::string& GetName() const noexcept {
				return name_;
			}
		};

		struct Access : public Dependence {
			std::string name_;

			[[nodiscard]]
			const std::string& GetName() const noexcept {
				return name_;
			}
		};

		struct Remove : public Dependence {
			std::string name_;

			[[nodiscard]]
			const std::string& GetName() const noexcept {
				return name_;
			}
		};

		struct Include : public Dependence {
			//std::shared_ptr<ParsedComponent> ptr_ = nullptr;
			std::string name_;
			bool readonly_ = true;

			[[nodiscard]]
			bool IsReadonly() const {
				return readonly_;
			}

			[[nodiscard]]
			const std::string& GetName() const noexcept {
				return name_;
			}

			[[nodiscard]]
			const std::string GetLowerName() const noexcept {
				//If include contains type with namespace (exmpl: "Behaviour::LuaContext")need to exchange :: to _
				std::string name = GetName();
				std::replace(name.begin(), name.end(), ':', '_');
				//
				return std::string{ (char)std::tolower(name[0]) } + name.substr(1);
			}
		};

		struct RandomAccessEntity {
			std::shared_ptr<Archetype> archetype_ = nullptr;
			std::vector<Include> includes_;
			std::vector<Create> creates_;
			std::vector<Remove> removes_;
			bool randomAccessComponents_ = false;

			using ProcessInclude = std::function<bool(Include& include, bool isLast)>;

			void ForEachInclude(ProcessInclude&& processInclude){
				for (Common::Index i = 0; i < includes_.size(); i++) {
					Include& include = includes_[i];
					if (!processInclude(include, (i == includes_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessCreate = std::function<bool(Create& create, bool isLast)>;
			void ForEachCreate(ProcessCreate&& processInclude) {
				for (Common::Index i = 0; i < creates_.size(); i++) {
					Create& include = creates_[i];
					if (!processInclude(include, (i == creates_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessRemove = std::function<bool(Remove& include, bool isLast)>;

			void ForEachRemove(ProcessRemove&& processExclude) {
				for (Common::Index i = 0; i < removes_.size(); i++) {
					Remove& include = removes_[i];
					if (!processExclude(include, (i == removes_.size() - 1))) {
						break;
					}
				}
			}

			[[nodiscard]]
			bool IsProcessesComponent(std::string& component){
				bool isProcessesComponent = false;
				ForEachInclude([&](const ParsedSystem::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component) {
						isProcessesComponent = true;
						return false;
					}
					return true;
					});
				return isProcessesComponent;
			}

			[[nodiscard]]
			bool IsReadsComponent(const std::string& component) {
				bool isReadsComponent = false;
				ForEachInclude([&](ParsedSystem::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && dependenceSystemInclude.readonly_) {
						isReadsComponent = true;
						return false;
					}
					return true;
					});
				return isReadsComponent;
			}

			[[nodiscard]]
			bool IsChangesComponent(const std::string& component) {
				bool isChangesComponent = false;
				ForEachInclude([&](ParsedSystem::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && !dependenceSystemInclude.readonly_) {
						isChangesComponent = true;
						return false;
					}
					return true;
					});
				return isChangesComponent;
			}

		};

		struct ProcessedEntity {
			std::vector<Include> includes_;
			bool processesAllCombinations_ = false;
			std::vector<Exclude> excludes_;
			std::vector<Create> creates_;
			std::vector<Remove> removes_;
			std::vector<Access> accesses_;
			bool randomAccessComponents_ = false; // Process components that are not required using IsComponentExist()/GetComponentsFilter() and GetComponent()

			using ProcessInclude = std::function<bool(Include& include, bool isLast)>;

			void ForEachInclude(ProcessInclude&& processInclude) {
				for (Common::Index i = 0; i < includes_.size(); i++) {
					Include& include = includes_[i];
					if (!processInclude(include, (i == includes_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessConstInclude = std::function<bool(const Include& include, bool isLast)>;
			void ForEachInclude(ProcessConstInclude&& processInclude) const {
				for (Common::Index i = 0; i < includes_.size(); i++) {
					const Include& include = includes_[i];
					if (!processInclude(include, (i == includes_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessExclude = std::function<bool(Exclude& include, bool isLast)>;

			void ForEachExclude(ProcessExclude&& processExclude) {
				for (Common::Index i = 0; i < excludes_.size(); i++) {
					Exclude& include = excludes_[i];
					if (!processExclude(include, (i == excludes_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessAccess = std::function<bool(Access& include, bool isLast)>;

			void ForEachAccess(ProcessAccess&& processExclude) {
				for (Common::Index i = 0; i < accesses_.size(); i++) {
					Access& include = accesses_[i];
					if (!processExclude(include, (i == accesses_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessConstAccess = std::function<bool(const Access& include, bool isLast)>;

			void ForEachAccess(ProcessConstAccess&& processExclude) const {
				for (Common::Index i = 0; i < accesses_.size(); i++) {
					const Access& include = accesses_[i];
					if (!processExclude(include, (i == accesses_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessCreate = std::function<bool(Create& include, bool isLast)>;

			void ForEachCreate(ProcessCreate&& processExclude) {
				for (Common::Index i = 0; i < creates_.size(); i++) {
					Create& include = creates_[i];
					if (!processExclude(include, (i == creates_.size() - 1))) {
						break;
					}
				}
			}

			using ProcessRemove = std::function<bool(Remove& include, bool isLast)>;

			void ForEachRemove(ProcessRemove&& processExclude) {
				for (Common::Index i = 0; i < removes_.size(); i++) {
					Remove& include = removes_[i];
					if (!processExclude(include, (i == removes_.size() - 1))) {
						break;
					}
				}
			}

			[[nodiscard]]
			bool IsProcessesComponent(const std::string& component) const {
				bool isProcessesComponent = false;
				ForEachInclude([&](const ParsedSystem::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component) {
						isProcessesComponent = true;
						return false;
					}
					return true;
					});
				return isProcessesComponent;
			}

			[[nodiscard]]
			bool IsReadsComponent(const std::string& component) const {
				bool isReadsComponent = false;
				ForEachInclude([&](const ParsedSystem::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && dependenceSystemInclude.readonly_) {
						isReadsComponent = true;
						return false;
					}
					return true;
					});
				return isReadsComponent;
			}

			[[nodiscard]]
			bool IsChangesComponent(const std::string& component) const {
				bool isChangesComponent = false;
				ForEachInclude([&](const ParsedSystem::Include& dependenceSystemInclude, bool isLast) {
					if (dependenceSystemInclude.name_ == component && !dependenceSystemInclude.readonly_) {
						isChangesComponent = true;
						return false;
					}
					return true;
					});
				return isChangesComponent;
			}

			[[nodiscard]]
			bool IsRemovesComponents() {
				return !removes_.empty();
			}

			[[nodiscard]]
			bool IsCreatesComponents() {
				return !creates_.empty();
			}

		};

		struct CreatesEntity {
			std::vector<Create> creates_;

			using ProcessCreate = std::function<bool(Create& create, bool isLast)>;
			void ForEachCreate(const ProcessCreate& processCreate) {
				for (Common::Index i = 0; i < creates_.size(); i++) {
					Create& create = creates_[i];
					const bool isContinue = processCreate(create, (i == creates_.size() - 1));
					if (!isContinue) {
						break;
					}
				}
			}

		};

		enum class Thread {
			Main,
			Child,
			Undefined
		};

		virtual ParsedTable::Type GetType() const noexcept override {
			return ParsedTable::Type::System;
		}

		virtual std::shared_ptr<ParsedTable> Clone() const noexcept override {
			return std::make_shared<ParsedSystem>(*this);
		}

		static Thread StringToThread(const std::string& threadString) noexcept {

			if (threadString == "Main") {
				return Thread::Main;
			}
			else if (threadString == "Child") {
				return Thread::Child;
			}

#pragma region Assert
			ASSERT_FAIL_MSG("Invalid thread name.");
#pragma endregion

			return Thread::Undefined;
		}

		Thread GetThread() const noexcept {
			return ci_.thread_;
		}

		struct UpdateMethodInfo {

			std::vector<ProcessedEntity> processesEntities_;
			std::vector<RandomAccessEntity> randomAccessesEntities_;
			std::vector<CreatesEntity> createsEntities_; // we dont need info: dynamic or archetype entity to create because we need generate only to add components includes.

			[[deprecated]]
			bool IsCreatesComponent(const std::string& componentName);

			[[deprecated]]
			[[nodiscard]]
			bool IsChangesComponent(const std::string& component);

			[[deprecated]]
			[[nodiscard]]
			bool IsReadsComponent(const std::string& component);

			[[nodiscard]]
			Common::Size GetProcessesEntitiesNumber() const noexcept;

			[[nodiscard]]
			bool IsRemovesComponents() noexcept {
				bool isRemovesComponents = false;
				ForEachProcessEntity([&](ProcessedEntity& entity, bool isLast) {
					if (entity.IsRemovesComponents()) {
						isRemovesComponents = true;
						return false;
					}

					return true;
					});

				//TODO: in random access we can remove component too.
				//ForEachRandomAccessEntity([]() {
				//	
				//	
				//	});
				return isRemovesComponents;
			}

			[[nodiscard]]
			bool IsCreatesComponents() noexcept {
				bool isCreatesComponents = false;
				ForEachProcessEntity([&](ProcessedEntity& entity, bool isLast) {
					if (entity.IsCreatesComponents()) {
						isCreatesComponents = true;
						return false;
					}

					return true;
					});

				ForEachCreateEntity([&](CreatesEntity& entity, bool isLast) {
					
					if (!entity.creates_.empty()) {
						isCreatesComponents = true;
						return false;
					}

					return true;
					});
				return isCreatesComponents;
			}

			using ProcessComponentName = std::function<bool(std::string& systemName, bool isLast)>;

			using ProcessRandomAccessEntity = std::function<bool(RandomAccessEntity& entity, bool isLast)>;
			void ForEachRandomAccessEntity(ProcessRandomAccessEntity&& processEntity);

			using ProcessCreateEntity = std::function<bool(CreatesEntity& entity, bool isLast)>;
			void ForEachCreateEntity(ProcessCreateEntity&& processEntity);

			void ForEachRandomAccessComponentName(ProcessComponentName&& processComponent);

			using ProcessConstRequestEntity = std::function<bool(const ProcessedEntity& entity, bool isLast)>;
			void ForEachProcessEntity(ProcessConstRequestEntity&& processEntity) const;

			using ProcessRequestEntity = std::function<bool(ProcessedEntity& entity, bool isLast)>;
			void ForEachProcessEntity(ProcessRequestEntity&& processEntity);

			[[deprecated]]
			[[nodiscard]]
			bool IsProcessesComponent(std::string& component);
		
			[[deprecated]]
			bool IsAccessesComponentByRandomAccess(const std::string& componentName);

		};

		struct AfterUpdateMethodInfo {

		};

		struct CallOrderInfo {

			struct System {
				std::string name_;
				std::shared_ptr<ParsedSystem> ptr_ = nullptr;
			};

			std::vector<System> runAfter_;
			std::vector<System> runBefore_;

			using ProcessSystemName = std::function<bool(System)>;

			void ForEachRunAfterSystem(ProcessSystemName&& processSystemName);
			void ForEachRunBeforeSystem(ProcessSystemName&& processSystemName);

		};

		enum class Type {
			Initialize,
			FrameToFrame,
			Undefined
		};

		static Type StringToType(const std::string& typeString) noexcept {

			if (typeString == "FrameToFrame") {
				return Type::FrameToFrame;
			}
			else if (typeString == "Initialize") {
				return Type::Initialize;
			}

#pragma region Assert
			ASSERT_FAIL_MSG("Invalid thread name.");
#pragma endregion

			return Type::Undefined;
		}

		struct CreateInfo {
			bool isEnabled_ = true;
			std::vector<std::string> namespace_;
			std::string name_;
			Thread thread_ = Thread::Undefined;
			Type type_ = Type::Undefined;
			std::shared_ptr<CallOrderInfo> callOrderInfo_ = nullptr;
			std::shared_ptr<UpdateMethodInfo> updateMethod_;
			std::shared_ptr<AfterUpdateMethodInfo> afterUpdateMethod_ = nullptr;
		};


		ParsedSystem(const CreateInfo& createInfo) :
			ci_{ createInfo } { }

		Thread GetThread() {
			return ci_.thread_;
		}

		const std::string& GetName() const noexcept override {
			return ci_.name_;
		}

		std::string GetLowerName() const {
			return std::string{ (char)std::tolower(GetName()[0]) } + GetName().substr(1);
		}

		[[nodiscard]]
		bool IsInitializeSystem() const {
			return ci_.type_ == Type::Initialize;
		}

		CreateInfo ci_;
	};

	using ParsedSystemPtr = std::shared_ptr<ParsedSystem>;

	inline std::vector<std::string> GetSystemNamespace(ParsedSystemPtr parsedSystem) {

		std::vector<std::string> systemNamespace;

		parsedSystem->ForEachParentTable([&](ParsedTablePtr parsedTable) {
			
			systemNamespace.insert(systemNamespace.begin(), parsedTable->GetName());

			return true;
			
			});

		return systemNamespace;
	}

	[[nodiscard]]
	inline bool AreSystemsCodependent(
		std::shared_ptr<ParsedSystem> firstSystem,
		std::shared_ptr<ParsedSystem> secondSystem) {

#pragma region Assert
		ASSERT_FMSG(firstSystem->GetName() != secondSystem->GetName(), "");
#pragma endregion

		bool isDepends = false;

		firstSystem->ci_.updateMethod_->ForEachProcessEntity([&](ParsedSystem::ProcessedEntity& entity, bool isLast) {
			if (entity.randomAccessComponents_ || entity.processesAllCombinations_) {
				isDepends = true;
				return false;
			}
			entity.ForEachInclude([&](ParsedSystem::Include& include, bool isLast) {
				if (secondSystem->ci_.updateMethod_->IsCreatesComponent(include.name_)) {
					isDepends = true;
					OS::LogInfo("Dependence",
						{ "System {} depends from {}, because system {} processes component {} that system {} creates.",
						firstSystem->GetName(),
						secondSystem->GetName(),
						firstSystem->GetName(),
						include.name_,
						secondSystem->GetName() });
					return false;
				}
				if ((include.IsReadonly() && secondSystem->ci_.updateMethod_->IsChangesComponent(include.name_)) ||
					(!include.IsReadonly() && secondSystem->ci_.updateMethod_->IsChangesComponent(include.name_)) ||
					(!include.IsReadonly() && secondSystem->ci_.updateMethod_->IsReadsComponent(include.name_))) {
					isDepends = true;
					return false;
				}
				if (secondSystem->ci_.updateMethod_->IsProcessesComponent(include.name_)) {
					isDepends = true;
					OS::LogInfo("Dependence",
						{ "System {} depends from {},  because they both processes component {}.",
						firstSystem->GetName(),
						secondSystem->GetName(),
						include.name_ });
					return false;
				}

				if (secondSystem->ci_.updateMethod_->IsAccessesComponentByRandomAccess(include.name_)) {
					isDepends = true;
					OS::LogInfo("Dependence",
						{ "System {} depends from {}, because system {} processes component {} that system {} accesses.",
						firstSystem->GetName(),
						secondSystem->GetName(),
						firstSystem->GetName(),
						include.name_,
						secondSystem->GetName() });
					return false;
				}
				return true;
				});
			return true;
			});
		return isDepends;

	}

}