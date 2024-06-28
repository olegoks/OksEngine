#pragma once 
#include <memory>
#include <PAL.World.hpp>

namespace PAL {

	class API {
	public:

		virtual std::shared_ptr<World> CreateWorld() = 0;

	private:
	};

}