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
#include <Common.Types.hpp>

namespace ECS2 {


	class Entity {
	public:

		//class Filter {
		//public:
		//	template<class ComponentType>
		//	Filter& Include() {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		const ComponentTypeId typeId = ComponentType::GetTypeId();
		//		includes_.set(typeId);
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		//debugInfo_.idName_[typeId] = ComponentType::GetName();
		//		return *this;
		//	}

		//	template<class ComponentType>
		//	Filter& DeleteInclude() {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		includes_.reset(ComponentType::GetTypeId());
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		//debugInfo_.idName_.erase(ComponentType::GetTypeId());
		//		return *this;
		//	}

		//	template<class ComponentType>
		//	Filter& Exclude() {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		const ComponentTypeId typeId = ComponentType::GetTypeId();
		//		excludes_.set(typeId);
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		return *this;
		//	}

		//	Filter& ExcludeAll() {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		excludes_.set();
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		return *this;
		//	}

		//	template<class ComponentType>
		//	Filter& DeleteExclude() {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		excludes_.reset(ComponentType::GetTypeId());
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		return *this;
		//	}

		//	[[nodiscard]]
		//	bool operator==(const Filter& filter) const noexcept {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		if (this == &filter) { return true; }
		//		return (includes_ == filter.includes_) && (excludes_ == filter.excludes_);
		//	}

		//	[[nodiscard]]
		//	bool Matches(const Filter& filter) const noexcept {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		const bool thereAreAllNeedIncludes =
		//			/*(includes_.count() == 0) || */((filter.includes_ & includes_) == includes_);
		//		if (thereAreAllNeedIncludes) {
		//			const bool thereArentNoNeedComponents = (filter.includes_ & excludes_) == 0;
		//			if (thereArentNoNeedComponents) {
		//				return (includes_ & filter.excludes_) == 0;
		//			}
		//		}
		//		return false;
		//	}

		//	[[nodiscard]]
		//	Filter operator+(const Filter& filter) const noexcept {
		//		//ASSERT_FMSG(IsValid(), "Error while merging entity filters.");
		//		//ASSERT_FMSG(filter.IsValid(), "Error while merging entity filters.");
		//		Filter result;
		//		{
		//			result.includes_ = (includes_ & ~filter.excludes_) | (filter.includes_ & ~excludes_);
		//			result.excludes_ = 0;
		//		}
		//		//ASSERT_FMSG(IsValid(), "Error while merging entity filters.");
		//		return result;
		//	}

		//	void Clear() noexcept {
		//		//ASSERT_FMSG(IsValid(), "Invalid filter state.");
		//		includes_.reset();
		//		excludes_.reset();
		//	}

		//	[[nodiscard]]
		//	std::string IncludesToString() const noexcept {
		//		return includes_.to_string();
		//	}

		//	[[nodiscard]]
		//	std::string ExcludesToString() const noexcept {
		//		return excludes_.to_string();
		//	}

		//private:

		//	[[nodiscard]]
		//	bool IsValid() const noexcept {
		//		return (includes_ & excludes_) == 0;
		//	}

		//private:

		//	std::bitset<256> includes_;
		//	std::bitset<256> excludes_;
		//};

		enum class Type {
			Dynamic,	//Can contain all types of components.
			Archetype,	//Have a definite set of components that entity can contain.
			Undefined
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

			[[nodiscard]]
			ValueType GetRawValue() const noexcept {
				return value_;
			}

			[[nodiscard]]
			bool IsInvalid() const noexcept {
				return value_ == invalid_;
			}

			[[nodiscard]]
			bool IsValid() const noexcept {
				return value_ != invalid_;
			}

			Id operator++(int value) noexcept {
				return ++value_;
			}

			struct Hash {
				Common::Size operator()(const Id id) const noexcept {
					return id.value_;
				}
			};

			~Id() noexcept = default;

		private:
			ValueType value_;
		};


		Entity(Id id) noexcept : id_{ id } { }
		[[nodiscard]]
		Id GetId() const noexcept { return id_; }


	private:
		Id id_;

	};

	static_assert(sizeof(Entity::Id) == sizeof(Common::Size), "");

	inline const Entity::Id Entity::Id::invalid_ = Common::Limits<Entity::Id::ValueType>::Max();


}
