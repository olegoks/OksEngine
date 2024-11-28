#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {


	struct MutableMesh : public ECSComponent<MutableMesh> {
	public:
		MutableMesh()
			: ECSComponent{ nullptr } {

		}
	};

}