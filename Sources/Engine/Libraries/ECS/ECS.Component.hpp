#pragma once

#include <cstdlib>

#include <ECS.Common.hpp>

#include <TypeInfo.hpp>

namespace ECS {

	using ComponentTypeId = Common::TypeId;

	template<class Type = int>
	class IComponent {
	public:

		[[nodiscard]]
		static ComponentTypeId GetTypeId() noexcept { return Common::TypeInfo<Type>{}.GetId(); }

		[[nodiscard]]
		static std::size_t GetSize() noexcept { return sizeof(Type); }
	};

	struct DebugInfo : public ECS::IComponent<DebugInfo> {
		DebugInfo(const char* name) noexcept :  name_{ name }{ }
	private:
		const char* name_ = nullptr;
	};


}