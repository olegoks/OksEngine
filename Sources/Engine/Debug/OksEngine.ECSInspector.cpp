#include <Debug/OksEngine.EnginePerformance.hpp>


namespace OksEngine {


	EnginePerformance::EnginePerformance() : ECSComponent{ nullptr } {

	}

	EnginePerformance::EnginePerformance(
		Context* context) :
		ECSComponent{ context } {}




}