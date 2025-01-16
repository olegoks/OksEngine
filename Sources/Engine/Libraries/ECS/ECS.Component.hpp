#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include <ECS.Common.hpp>

#include <TypeInfo.hpp>

namespace ECS {

	using ComponentTypeId = Common::TypeId;

	extern ComponentTypeId nextId;
	constexpr inline Common::Size maxNameLength = 100;

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
		static const char* GetName() noexcept {
			static char name[maxNameLength] = { 0 };
			if(std::strlen(name) == 0) {
				const std::string fullName = Common::TypeInfo<Type>{}.GetFullName();
				Common::Index shortNameStart = fullName.find_last_of(':');
				const std::string shortName = fullName.substr(shortNameStart + 1);
				OS::AssertMessage(shortName.length() <= maxNameLength, "Component name is too long.");
				std::memcpy(name, shortName.c_str(), shortName.length());
			}
			return name;
		}

		[[nodiscard]]
		static std::size_t GetSize() noexcept { return sizeof(Type); }
	};

}