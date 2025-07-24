#pragma once

#include <System/ECSGenerator2.ParsedSystem.hpp>
#include <Component/ECSGenerator2.ParsedComponent.hpp>
#include <Struct/ECSGenerator2.ParsedStruct.hpp>

namespace ECSGenerator2 {


	class ParsedECSFile {
	public:

		struct CreateInfo {
			std::filesystem::path path_;
			std::vector<std::shared_ptr<ParsedSystem>> systems_;
			std::vector<std::shared_ptr<ParsedComponent>> components_;
			std::vector<std::shared_ptr<ParsedStruct>> structs_;
		};

		ParsedECSFile(const CreateInfo& ci) :
			ci_{ ci } {  }

		//Access systems.
		using ProcessSystem = std::function<void(std::shared_ptr<ParsedSystem>)>;

		void ForEachSystem(ProcessSystem&& processSystem) {
			for (Common::Index i = 0; i < ci_.systems_.size(); i++) {
				std::shared_ptr<ParsedSystem> system = ci_.systems_[i];
				processSystem(system);
			}
		}

		//Access components.
		using ProcessComponent = std::function<void(std::shared_ptr<ParsedComponent>)>;

		void ForEachComponent(ProcessComponent&& processComponent) {
			for (Common::Index i = 0; i < ci_.components_.size(); i++) {
				std::shared_ptr<ParsedComponent> component = ci_.components_[i];
				processComponent(component);
			}
		}

		using ProcessStruct = std::function<void(std::shared_ptr<ParsedStruct>)>;

		void ForEachStruct(ProcessStruct&& processStruct) {
			for (Common::Index i = 0; i < ci_.structs_.size(); i++) {
				std::shared_ptr<ParsedStruct> parsedStruct = ci_.structs_[i];
				processStruct(parsedStruct);
			}
		}

		[[nodiscard]]
		bool IsContainsSystems() const noexcept {
			return !ci_.systems_.empty();
		}

		[[nodiscard]]
		bool IsContainsComponent(const std::string& componentName) {
			for (auto parsedComponent : ci_.components_) {
				if (componentName == parsedComponent->GetName()) {
					return true;
				}
			}
			return false;
		}

		[[nodiscard]]
		const std::filesystem::path GetPath() const noexcept {
			return ci_.path_;
		}

		[[nodiscard]]
		const std::string GetName() const noexcept {
			return GetPath().filename().stem().string();
		}

	private:

		CreateInfo ci_;
	};


}