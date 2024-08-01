#pragma once 
#include <PAL.API.hpp>

namespace PAL {

	[[nodiscard]]
	std::shared_ptr<PAL::API> CreateAPI();

}