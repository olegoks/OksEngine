#pragma once 


#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct EnginePerformance : public ECSComponent<EnginePerformance> {
	public:
		EnginePerformance();

		EnginePerformance(Context* context);
		bool show_ = true;
	};

}