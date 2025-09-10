#pragma once 

#include <filesystem>
#include <string>
#include <functional>

#include <Common.hpp>
#include <OS.Logger.hpp>

#include <ECSGenerator2.ParsedTable.hpp>
#include <Component/ECSGenerator2.ParsedComponent.hpp>
#include <ECSGenerator2.Code.hpp>

namespace ECSGenerator2 {


	class ParsedSystem : public ParsedTable {
	public:

		using Exclude = std::string;

		struct Include {
			std::shared_ptr<ParsedComponent> ptr_ = nullptr;
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
			std::vector<Include> includes_;
			std::vector<std::string> creates_;
			bool randomAccessComponents_ = false;

			using ProcessInclude = std::function<bool(const Include& include, bool isLast)>;

			void ForEachInclude(ProcessInclude&& processInclude) const {
				for (Common::Index i = 0; i < includes_.size(); i++) {
					const Include& include = includes_[i];
					if (!processInclude(include, (i == includes_.size() - 1))) {
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

		};

		struct ProcessedEntity {
			std::vector<Include> includes_;
			bool processesAllCombinations_ = false;
			std::vector<Exclude> excludes_;
			std::vector<std::string> creates_;
			std::vector<std::string> removes_;
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

			using ProcessExclude = std::function<bool(const Exclude& include, bool isLast)>;

			void ForEachExclude(ProcessExclude&& processExclude) const {
				for (Common::Index i = 0; i < excludes_.size(); i++) {
					const Exclude& include = excludes_[i];
					if (!processExclude(include, (i == excludes_.size() - 1))) {
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
			OS::AssertFailMessage("Invalid thread name.");
#pragma endregion

			return Thread::Undefined;
		}

		Thread GetThread() const noexcept {
			return ci_.thread_;
		}

		struct UpdateMethodInfo {

			std::vector<ProcessedEntity> processesEntities_;
			std::vector<RandomAccessEntity> randomAccessesEntities_;
			std::vector<std::vector<std::string>> createsEntities_; // we dont need info: dynamic or archetype entity to create because we need generate only to add components includes.

			bool IsCreatesComponent(const std::string& componentName);

			[[nodiscard]]
			bool IsChangesComponent(const std::string& component) const;

			[[nodiscard]]
			bool IsReadsComponent(const std::string& component) const;

			[[nodiscard]]
			Common::Size GetProcessesEntitiesNumber() const noexcept;

			using ProcessComponentName = std::function<bool(const std::string& systemName, bool isLast)>;

			using ProcessRandomAccessEntity = std::function<bool(const RandomAccessEntity& entity, bool isLast)>;
			void ForEachRandomAccessEntity(ProcessRandomAccessEntity&& processEntity) const;

			void ForEachRandomAccessComponent(ProcessComponentName&& processComponent) const;

			using ProcessConstRequestEntity = std::function<bool(const ProcessedEntity& entity, bool isLast)>;
			void ForEachProcessEntity(ProcessConstRequestEntity&& processEntity) const;

			using ProcessRequestEntity = std::function<bool(ProcessedEntity& entity, bool isLast)>;
			void ForEachProcessEntity(ProcessRequestEntity&& processEntity);

			[[nodiscard]]
			bool IsProcessesComponent(const std::string& component);
		

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
			OS::AssertFailMessage("Invalid thread name.");
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
		OS::AssertMessage(firstSystem->GetName() != secondSystem->GetName(), "");
#pragma endregion

		bool isDepends = false;

		firstSystem->ci_.updateMethod_->ForEachProcessEntity([&](const ParsedSystem::ProcessedEntity& entity, bool isLast) {
			if (entity.randomAccessComponents_ || entity.processesAllCombinations_) {
				isDepends = true;
				return false;
			}
			entity.ForEachInclude([&](const ParsedSystem::Include& include, bool isLast) {
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