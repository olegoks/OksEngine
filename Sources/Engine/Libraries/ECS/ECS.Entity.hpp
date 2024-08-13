#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <unordered_set>
#include <optional>
#include <algorithm>
#include <functional>
#include <utility>
#include <set>
#include <Common.hpp>

#include <ECS.Component.hpp>

namespace ECS {

	class Entity {
	public:

		class Filter {
		public:

			template<class ComponentType>
			Filter& Include() {
				OS::AssertMessage(
					!excludes_.contains(ComponentType::GetId()), 
					"Attempt to use the component in includes and excludes.");
				includes_.insert(ComponentType::GetId());
				return *this;
			}

			template<class ComponentType>
			Filter& DeleteInclude() {
				OS::AssertMessage(
					includes_.contains(ComponentType::GetId()),
					"Attempt to delete include that doesnt exist");
				includes_.erase(ComponentType::GetId());
				return *this;
			}

			template<class ComponentType>
			Filter& Exclude() {
				OS::AssertMessage(
					!includes_.contains(ComponentType::GetId()),
					"Attempt to use the component in includes and excludes.");
				excludes_.insert(ComponentType::GetId());
				return *this;
			}

			template<class ComponentType>
			Filter& DeleteExclude() {
				OS::AssertMessage(
					excludes_.contains(ComponentType::GetId()),
					"Attempt to delete exclude that doesnt exist");
				excludes_.erase(ComponentType::GetId());
				return *this;
			}

			[[nodiscard]]
			bool operator==(const Filter& filter) const noexcept {
				if (this == &filter) { return true; }
				return (includes_ == filter.includes_) && (excludes_ == filter.excludes_);
			}

			[[nodiscard]]
			bool Matches(const Filter& filter) const noexcept {
				const bool thereAreAllNeedIncludes = std::ranges::includes(filter.includes_, includes_);
				if(thereAreAllNeedIncludes) {
					std::set<ComponentTypeId> intersection{};
					std::ranges::set_intersection(
						excludes_, filter.includes_,
						std::inserter(intersection, intersection.begin()));
					if(intersection.empty()){
						//Useless but for clean logic
						std::set<ComponentTypeId> intersection2{};
						std::ranges::set_intersection(
							filter.excludes_, includes_,
							std::inserter(intersection2, intersection2.begin()));
						if (intersection2.empty()) {
							return true;
						}
					}
					return false;
				}
				return false;
			}

		private:
			std::set<ComponentTypeId> includes_;
			std::set<ComponentTypeId> excludes_;
		};

		class Id {
		public:

			using ValueType = Common::Size;

			static const Id invalid_;

			Id() noexcept : value_{ invalid_ } { }
			Id(ValueType value) noexcept : value_{ value } {}

			operator ValueType() const noexcept {
				return value_;
			}

			Id operator++(int value) noexcept {
				return ++value_;
			}

			struct Hash {
				std::size_t operator()(const Id& id) const noexcept {
					const Id::ValueType value = id.operator size_t();
					return std::hash<Id::ValueType>{}(value);
				}
			};

			~Id() noexcept = default;

		private:
			ValueType value_;
		};


		[[nodiscard]]
		Id GetId() const noexcept { return id_; }

		[[nodiscard]]
		Filter& GetFilter() noexcept
		{
			return filter_;
		}

	private:
		Filter filter_;
		Id id_;

	};


	inline const Entity::Id Entity::Id::invalid_ = Common::Limits<Entity::Id::ValueType>::Max();


}
