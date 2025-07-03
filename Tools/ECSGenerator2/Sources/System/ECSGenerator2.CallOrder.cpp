#include <System/ECSGenerator2.ParsedSystem.hpp>

namespace ECSGenerator2 {


	void ParsedSystem::CallOrderInfo::ForEachRunAfterSystem(ProcessSystemName&& processSystemName) {
		for (const std::string& runAfterSystemName : runAfter_) {
			const bool stop = !processSystemName(runAfterSystemName);
			if (stop) {
				break;
			}
		}
	}
	void ParsedSystem::CallOrderInfo::ForEachRunBeforeSystem(ProcessSystemName&& processSystemName) {
		for (const std::string& runBeforeSystemName : runBefore_) {
			const bool stop = !processSystemName(runBeforeSystemName);
			if (stop) {
				break;
			}
		}
	}

}