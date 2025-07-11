#pragma once
#include <Common\FramesCounter\auto_OksEngine.FramesCounter.hpp>

namespace OksEngine
{

void CreateFramesCounterEntity::Update() {

    ECS2::Entity::Id entityId = CreateEntity();

    CreateComponent<FramesCounter>(entityId, 0);

};

void IncreaseFramesCounter::Update(ECS2::Entity::Id entity0id, FramesCounter *framesCounter0) {

    ++framesCounter0->value_;

};



} // namespace OksEngine