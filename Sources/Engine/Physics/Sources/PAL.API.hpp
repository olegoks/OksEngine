#pragma once 
#include <memory>
#include <PAL.World.hpp>

namespace PAL {

	class API {
	public:

		virtual std::shared_ptr<World> CreateWorld(const PAL::World::CreateInfo createInfo) = 0;

	private:
	};

}