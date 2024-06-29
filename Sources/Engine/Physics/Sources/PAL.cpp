
#include <PAL.hpp>
#include <PhysX.API.hpp>

namespace PAL {

	[[nodiscard]]
	std::shared_ptr<PAL::API> CreateAPI() {
		return std::make_shared<PhysX::API>();
	}

}