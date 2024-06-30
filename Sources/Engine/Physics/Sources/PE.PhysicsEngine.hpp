#pragma once 

#include <PAL.hpp>

namespace PE {

	class PhysicsEngine {
	public:

		PhysicsEngine() {

			std::shared_ptr<PAL::API> api = PAL::CreateAPI();

			PAL::World::CreateInfo worldCreateInfo {
				.gravity_ = { 0.f, -9.81f, 0.f }
			};
			std::shared_ptr<PAL::World> world = api->CreateWorld(worldCreateInfo);

		}

	private:
		std::shared_ptr<PAL::API> api_ = nullptr;
		std::shared_ptr<PAL::World> world_ = nullptr;
	};
}