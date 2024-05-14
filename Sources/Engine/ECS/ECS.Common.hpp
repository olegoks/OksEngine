#pragma once 

#include <optional>
#include <memory>

#include <OS.Assert.hpp>

namespace ECS {

	template<class Type>
	using Maybe = std::optional<Type>;

	template<class Type>
	using Ptr = std::shared_ptr<Type>;

}