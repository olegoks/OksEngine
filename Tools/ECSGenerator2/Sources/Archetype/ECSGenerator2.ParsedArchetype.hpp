#pragma once 

#include <ECSGenerator2.Common.hpp>
#include <ECSGenerator2.CodeFile.hpp>

#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <random>  
#include <format>

#include <ECSGenerator2.ParsedTable.hpp>

namespace ECSGenerator2 {

	class ParsedArchetype : public ParsedTable {
	public:

		struct Component {
			ParsedTablePtr ptr_ = nullptr;
			std::string name_ = "";
		};

		struct ChildArchetype {
			ParsedTablePtr ptr_ = nullptr;
			std::string name_ = "";
		};

		struct CreateInfo {
			std::string name_;

			std::vector<Component> components_;
			std::vector<ChildArchetype> archetypes_; // References to other arhetypes.
		};

		ParsedArchetype(const CreateInfo& createInfo) :
			ci_{ createInfo } { }

		virtual ParsedTable::Type GetType() const noexcept override {
			return ParsedTable::Type::Archetype;
		}

		virtual std::shared_ptr<ParsedTable> Clone() const noexcept override {
			return std::make_shared<ParsedArchetype>(*this);
		}

		using ProcessComponent = std::function<void(Component&, bool isLast)>;

		void ForEachComponent(ProcessComponent&& processComponent) {
			for (Common::Index i = 0; i < ci_.components_.size(); i++) {
				Component& component = ci_.components_[i];
				const bool isLast = (i == (ci_.components_.size() - 1));
				processComponent(component, isLast);
			}
		}

		using ProcessRefArchetype = std::function<void(ChildArchetype&, bool isLast)>;

		void ForEachRefArchetype(ProcessRefArchetype&& processArchetype) {
			for (Common::Index i = 0; i < ci_.archetypes_.size(); i++) {
				ChildArchetype& archetype = ci_.archetypes_[i];
				const bool isLast = (i == (ci_.archetypes_.size() - 1));
				processArchetype(archetype, isLast);
			}
		}

		using ProcessComponentRecursive = std::function<void(Component&)>;
		void ForEachComponentRecursive(ProcessComponentRecursive& processComponent) {

			for (auto& component : ci_.components_) {
				processComponent(component);
			}

			for (auto archetype : ci_.archetypes_) {
				auto childArchetype = Common::pointer_cast<ParsedArchetype>(archetype.ptr_);
				childArchetype->ForEachComponentRecursive(processComponent);
			}


		}

		[[nodiscard]]
		virtual const std::string& GetName() const noexcept override {
			return ci_.name_;
		}

		[[nodiscard]]
		std::string GetLowerName() {
			return std::string{ static_cast<char>(std::tolower(GetName()[0])) } + GetName().substr(1);
		}

		CreateInfo ci_;

	};

	using ParsedArchetypePtr = std::shared_ptr<ParsedArchetype>;

	inline std::vector<std::string> GetArchetypeNamespace(ParsedArchetypePtr parsedArchetype) {

		std::vector<std::string> archetypeNamespace;

		parsedArchetype->ForEachParentTable([&](ParsedTablePtr parsedTable) {

			archetypeNamespace.insert(archetypeNamespace.begin(), parsedTable->GetName());

			return true;

			});

		return archetypeNamespace;
	}

}