#include <Debug/OksEngine.ECSInspector.hpp>

namespace OksEngine {


	ECSInspector::ECSInspector() : ECSComponent{ nullptr } {

	}

	ECSInspector::ECSInspector(
		Context* context) :
		ECSComponent{ context } {}

}