#pragma once 

#include <optional>

#include <Common.Types.hpp>
#include <Common.Exception.hpp>
#include <Common.Format.hpp>
//#include <Common.Memory.hpp>

namespace Common {

	enum class Configuration { 
		Debug,
		Release
	};

	inline constexpr Configuration currentConfiguration = Configuration::Debug;

	template<class Type>
	using Maybe = std::optional<Type>;

}