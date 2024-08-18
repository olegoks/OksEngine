#pragma once 

#include <OksEngine.ECS.Component.hpp>

namespace OksEngine {

	class Texture : public ECSComponent<Texture> {
	public:
		std::string name_ = "";
	};

	template<>
	inline void Edit<Texture>(Texture* texture) {

	}


}