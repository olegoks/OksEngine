#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {



	struct ECSInspector : public ECSComponent<ECSInspector> {
	public:
		ECSInspector();

		ECSInspector(Context* context);
		bool show_ = true;
	};


}