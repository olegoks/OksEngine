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

    modelPosition->x_ = rbPosition->x_;
    modelPosition->y_ = rbPosition->y_;
    modelPosition->z_ = rbPosition->z_;

    modelRotation->w_ = rbRotation->w_;
    modelRotation->x_ = rbRotation->x_;
    modelRotation->y_ = rbRotation->y_;
    modelRotation->z_ = rbRotation->z_;


    };

}