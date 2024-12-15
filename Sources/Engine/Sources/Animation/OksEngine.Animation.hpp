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

		struct PositionKey {
			Common::Double time_ = 0.0f;
			glm::vec3 value_ = { 0.f, 0.f, 0.f };
		};

		struct RotationKey {
			Common::Double time_ = 0.0f;
			glm::quat value_{};
		};

		struct ScalingKey {
			Common::Double time_ = 0.0f;
			glm::vec3 value_{};
		};

		struct CreateInfo {
			std::string name_ = "No name";
			Common::Double duration_ = 0;
			float ticksPerSecond_ = 1.f;
			std::vector<PositionKey> positionKeys_;
			std::vector<RotationKey> rotationKeys_;
			std::vector<ScalingKey> scalingKeys_;
		};

		Animation(const CreateInfo& createInfo);

		std::string name_ = "No name";
		Common::Double durationInTicks_ = 0; // Duration in ticks.
		float ticksPerSecond_ = 1.f;

		std::vector<PositionKey> positionKeys_;
		std::vector<RotationKey> rotationKeys_;
		std::vector<ScalingKey> scalingKeys_;

	};

	template<>
	void Edit<Animation>(Animation* animation);

	template<>
	void Add<Animation>(ECS::World* world, ECS::Entity::Id id);

}