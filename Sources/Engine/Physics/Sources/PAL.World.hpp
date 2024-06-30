#pragma once 

#include <Math.Vector.hpp>

namespace PAL {

	class World {
	public:
		struct CreateInfo {
			Math::Vector3f gravity_{ 0.f, -9.81, 0.f };
		};

		World(const CreateInfo& createInfo) : 
			createInfo_{ createInfo } { }


	private:
		CreateInfo createInfo_;
	};

}