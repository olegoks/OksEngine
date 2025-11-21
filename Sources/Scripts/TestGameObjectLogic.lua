TestGameObject = {}

function TestGameObject:New()
    local object = Entity:New()

    return object
end

TestGameObjectUpdater = {}

function TestGameObjectUpdater:Update(camera, deltaMs)
   
end

TestGameObjectInputProcessor = {}

function TestGameObjectInputProcessor:ProcessKeyboardInput(camera, Key, Event)

    if Key == "MOVE_FORWARD" then
        if Event == "Pressed" then
            local rigidBodyEntityId = camera:GetRigidBodyEntityId()
            local rbEntityId = rigidBodyEntityId.id
            --local isWorldExist = ECSWorld:IsComponentExist(rbEntityId, "Physics::SetVelocityRequests")
            local setVelocityRequests = ECSWorld:GetSetVelocityRequests(rbEntityId)
            setVelocityRequests:AddSetVelocityRequest(1.0, 0.0, 0.0, 10.0)
        end
    end

    --local entityId = camera:GetId()
    --print("local entityId = camera:GetId()")
    -- print("ECSWorld:")
    -- print(ECSWorld)
    -- local isWorldExist = ECSWorld:IsComponentExist(1, "WorldPosition3D")
    -- print("local isWorldExist = ECSWorld:IsComponentExist(entityId, WorldPosition3D)")

    -- if Key == "MOVE_FORWARD" then
    --     if Event == "Pressed" then
    --         position.x = (position.x + 0.1)
    --     end
    -- elseif Key == "MOVE_LEFT" then
    --     if Event == "Pressed" then
    --         position.x = (position.x + 0.1)
    --         position.y = (position.y + 0.1)
    --         position.z = (position.z + 0.1)
    --     end
    -- elseif Key == "MOVE_RIGHT" then
    --     if Event == "Pressed" then

    --     end
    -- elseif Key == "MOVE_BACK" then
    --     if Event == "Pressed" then

    --     end
    -- end

end

function TestGameObjectInputProcessor:ProcessMouseInput(camera, Key, Event, offsetX, offsetY, scroll)

end
