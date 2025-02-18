#include <Common/FramesCounter/auto_OksEngine.IncreaseFramesCounter.hpp>

namespace OksEngine {

	void IncreaseFramesCounter::Update(FramesCounter* framesCounter, Counter* counter) {
		++counter->value_;
	}

}