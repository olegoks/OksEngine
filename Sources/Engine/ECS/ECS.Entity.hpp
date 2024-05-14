#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <unordered_set>
#include <optional>
#include <algorithm>
#include <functional>
#include <utility>

#include <Common.hpp>

namespace ECS {

	class Entity {
	public:

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
				return value++;
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


	private:

		Id id_;

	};


	inline const Entity::Id Entity::Id::invalid_ = Common::Limits<Entity::Id::ValueType>::Max();


}