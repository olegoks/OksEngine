#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include <ECS2.Common.hpp>

#include <Common.Types.hpp>

namespace ECS2 {

	using ComponentTypeId = Common::TypeId;

	extern ComponentTypeId nextId;

	template<class Type = int>
	class IComponent {
	public:

		//Id starts with 0 and grows to +infinity.
		[[nodiscard]]
		static ComponentTypeId GetTypeId() noexcept {
			static ComponentTypeId id = ++nextId;
			return id;
		}


	};

}