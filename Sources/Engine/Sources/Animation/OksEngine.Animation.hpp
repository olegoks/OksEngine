#pragma once 

#include <OksEngine.ECS.Component.hpp>

#include <assimp/StringUtils.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>

namespace OksEngine {


	struct Animation : public ECSComponent<Animation> {
		struct StateInfo {
			Common::Double time_ = 0;
			glm::vec3 translate_;
			glm::quat rotation_;
		};
		Animation(Common::Double ticksNumber_, float ticksPerSecond_, std::vector<StateInfo> states_);

		

		Common::Double duration_ = 0;
		float ticksPerSecond_ = 1.f;
		std::vector<StateInfo> states_;
	};

	template<>
	void Edit<Animation>(Animation* animation);

	template<>
	void Add<Animation>(ECS::World* world, ECS::Entity::Id id);

}