#pragma once

#include <cstdlib>
#include <map>

#include <ECS.Common.hpp>

#include <TypeInfo.hpp>

namespace ECS {

	using ComponentTypeId = Common::TypeId;

	extern ComponentTypeId nextId;
	static inline std::map<std::string, ComponentTypeId> nameId;
	static inline std::vector<std::pair<std::string, ComponentTypeId>> nameIds;

	template<class Type = int>
	class IComponent {
	public:

		//Id starts with 0 and grows to +infinity.
		[[nodiscard]]
		static ComponentTypeId GetTypeId() noexcept {
			static ComponentTypeId id = ++nextId;
			return id;
		}

		[[nodiscard]]
		static std::string GetName() noexcept {
			const std::string fullName = Common::TypeInfo<Type>{}.GetName();
			const Common::Index shortNameFirstIndex = fullName.find_last_of(':');
			const std::string shortName = fullName.substr(shortNameFirstIndex + 1);
			return shortName;
		}

		[[nodiscard]]
		static std::size_t GetSize() noexcept { return sizeof(Type); }
	};

}