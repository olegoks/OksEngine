#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct Model : public ECSComponent<Model> {
		Model() :
			ECSComponent{ nullptr } { }
	};

	template<>
	inline void Edit<Model>(Model* model) { }


}