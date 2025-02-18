#pragma once 

#include <optional>
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 1024
#include <magic_enum.hpp>
#include <Common.Types.hpp>
#include <Common.Exception.hpp>
#include <Common.Format.hpp>
//#include <Common.Memory.hpp>

namespace Common {

	enum class Configuration {
		Debug,
		Release,
		ReleaseWithDebugInfo
	};

	inline constexpr Configuration currentConfiguration = Configuration::Debug;

	template<class Type>
	using Maybe = std::optional<Type>;

	template<class Type>
	concept Enum = std::is_enum<Type>::value && requires(Type value) {
		value = Type::Undefined;
	};


#ifdef _DEBUG
	constexpr static inline Configuration configuration_ = Configuration::Debug;
#else
	constexpr static inline Configuration configuration_ = Configuration::Release;
#endif

	[[nodiscard]]
	constexpr bool IsDebug() {
		return (configuration_ == Configuration::Debug);
	}


	template<class Type>
	constexpr void DiscardUnusedParameter(Type&& value) { value; }


	template<class Callback, class Type>
	void ProcessType(Callback&& callback) {
		callback.template operator() < Type > ();
	}


	template<class Callback, class Type, class ...Types>
	void ProcessType(Callback&& callback) {
		callback.template operator() < Type > ();
		ProcessType<Callback, Types...>(std::forward<Callback>(callback));
	}


	template<class Callback>
	void ProcessType(Callback&&) { }


	template<class Callback, class ... Types>
	void ProcessTemplateList(Callback&& callback) {
		ProcessType<Callback, Types...>(std::forward<Callback>(callback));
	}

}