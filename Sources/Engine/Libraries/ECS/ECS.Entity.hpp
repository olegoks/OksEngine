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
#include <bitset>
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
					!excludes_.test(ComponentType::GetTypeId()), 
					"Attempt to use the component in includes and excludes.");
				includes_.set(ComponentType::GetTypeId());
				return *this;
			}

			template<class ComponentType>
			Filter& DeleteInclude() {
				OS::AssertMessage(
					includes_.test(ComponentType::GetTypeId()),
					"Attempt to delete include that doesnt exist");
				includes_.reset(ComponentType::GetTypeId());
				return *this;
			}

			template<class ComponentType>
			Filter& Exclude() {
				OS::AssertMessage(
					!includes_.test(ComponentType::GetTypeId()),
					"Attempt to use the component in includes and excludes.");
				excludes_.set(ComponentType::GetTypeId());
				return *this;
			}

			Filter& ExcludeAll() {
				excludes_.set();
				return *this;
			}

			template<class ComponentType>
			Filter& DeleteExclude() {
				OS::AssertMessage(
					excludes_.test(ComponentType::GetTypeId()),
					"Attempt to delete exclude that doesnt exist");
				excludes_.reset(ComponentType::GetTypeId());
				return *this;
			}

			[[nodiscard]]
			bool operator==(const Filter& filter) const noexcept {
				if (this == &filter) { return true; }
				return (includes_ == filter.includes_) && (excludes_ == filter.excludes_);
			}

			[[nodiscard]]
			bool Matches(const Filter& filter) const noexcept {
				const bool thereAreAllNeedIncludes =
					(includes_.count() == 0) || ((filter.includes_ & includes_) == includes_);
				if (thereAreAllNeedIncludes) {
					const bool thereArentNoNeedComponents = (filter.includes_ & excludes_) == 0;
					if (thereArentNoNeedComponents) {
						return (includes_ & filter.excludes_) == 0;
					}
				}
				return false;
			}

			[[nodiscard]]
			Filter operator+(const Filter& filter) const noexcept {
				Filter result;
				{
					result.includes_ = includes_ | filter.includes_;
					result.excludes_ = excludes_ | filter.excludes_;
				}
				OS::AssertMessage(IsValid(), "Error while merging entity filters.");
				return result;
			}

			void Clear() noexcept {
				includes_.reset();
				excludes_.reset();
			}

		private:

			[[nodiscard]]
			bool IsValid() const noexcept {
				return (includes_ & excludes_) == 0;
			}

		private:
			std::bitset<256> includes_;
			std::bitset<256> excludes_;
		};

		class Id final {
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


		Entity(Id id) noexcept : id_{ id } { }
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
