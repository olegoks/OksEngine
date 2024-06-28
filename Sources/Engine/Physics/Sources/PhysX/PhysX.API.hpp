#pragma once 
#include <PAL.API.hpp>
#include <PhysX.World.hpp>

namespace PhysX {

	class API : public PAL::API {
	public:

		virtual std::shared_ptr<PAL::World> CreateWorld() override {
			return std::make_shared<PhysX::World>();
		}
	private:
		
	};
}