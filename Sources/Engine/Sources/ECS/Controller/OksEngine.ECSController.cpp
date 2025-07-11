#pragma once
#include <ECS\Controller\auto_OksEngine.ECSController.hpp>

namespace OksEngine
{
void CreateECSController::Update() {


    const ECS2::Entity::Id entityId = CreateEntity();

    CreateComponent<ECSController>(entityId);

};

}