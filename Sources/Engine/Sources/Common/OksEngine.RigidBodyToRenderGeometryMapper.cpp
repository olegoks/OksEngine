#pragma once
#include <Common\auto_OksEngine.RigidBodyToRenderGeometryMapper.hpp>

namespace OksEngine
{
void RigidBodyWorldPositionToModelWorldPositionMapper::Update(
    ECS2::Entity::Id entity0id, const Physics::RigidBodyEntityId *physics__RigidBodyEntityId0,
    const Render::Mdl::ModelEntity *render__Mdl__ModelEntity0) {

    auto* modelPosition = GetComponent<WorldPosition3D>(render__Mdl__ModelEntity0->id_);
    auto* modelRotation = GetComponent<WorldRotation3D>(render__Mdl__ModelEntity0->id_);

    const auto* rbPosition = GetComponent<WorldPosition3D>(physics__RigidBodyEntityId0->id_);
    const auto* rbRotation = GetComponent<WorldRotation3D>(physics__RigidBodyEntityId0->id_);

    glm::vec3 rbPositionVec{ rbPosition->x_, rbPosition->y_, rbPosition->z_ };
    glm::vec3 modelPositionVec{ modelPosition->x_, modelPosition->y_, modelPosition->z_ };

    glm::vec3 offsetVec = rbPositionVec - modelPositionVec;
    
    ASSERT(!std::isnan(offsetVec.x) &&
        !std::isnan(offsetVec.y) &&
        !std::isnan(offsetVec.z));

    const float requiredOffsetLength = glm::length(offsetVec);

    if (requiredOffsetLength > 0.0f) {
        offsetVec = glm::normalize(offsetVec);

        static float maxOffsetPerFrame = 0.1;
        static float criticalOffsetPerFrame = 0.2;

        const float distance = glm::distance(rbPositionVec, modelPositionVec);

        if (requiredOffsetLength > maxOffsetPerFrame && requiredOffsetLength < criticalOffsetPerFrame) {
            offsetVec *= maxOffsetPerFrame;
        }
        else if(requiredOffsetLength > criticalOffsetPerFrame){
            offsetVec *= criticalOffsetPerFrame;
        }
        else {
            offsetVec *= requiredOffsetLength;
        }
        

        ASSERT(!std::isnan(offsetVec.x) &&
            !std::isnan(offsetVec.y) &&
            !std::isnan(offsetVec.z));
        modelPosition->x_ += offsetVec.x;
        modelPosition->y_ += offsetVec.y;
        modelPosition->z_ += offsetVec.z;


        modelRotation->w_ = rbRotation->w_;
        modelRotation->x_ = rbRotation->x_;
        modelRotation->y_ = rbRotation->y_;
        modelRotation->z_ = rbRotation->z_;
    }

    };

}