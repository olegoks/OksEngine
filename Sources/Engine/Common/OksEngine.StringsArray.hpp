#pragma once

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	struct StringsArray : public ECSComponent<StringsArray> {
		std::vector<std::string> strings_;
	};

}