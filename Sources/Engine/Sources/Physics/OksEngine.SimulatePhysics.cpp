#include <Physics\auto_OksEngine.SimulatePhysics.hpp>

namespace OksEngine {
void SimulatePhysics::Update(ECS2::Entity::Id entityId, PhysicsEngine* physicsEngine) {
	using namespace std::chrono_literals;
	static std::chrono::high_resolution_clock::time_point previousUpdate = std::chrono::high_resolution_clock::now();
	static std::chrono::high_resolution_clock::duration remainder = 0ms;
	const auto simulationGranularity = 100ms;
	const auto now = std::chrono::high_resolution_clock::now();
	const auto delta = (now - previousUpdate);
	auto toSimulate = delta + remainder;

	while (toSimulate >= simulationGranularity) {
		physicsEngine->physicsEngine_->GetWorld()->Simulate(simulationGranularity.count() / 1000.f);
		toSimulate -= simulationGranularity;
	}
	remainder = toSimulate;
	previousUpdate = std::chrono::high_resolution_clock::now();
};

}