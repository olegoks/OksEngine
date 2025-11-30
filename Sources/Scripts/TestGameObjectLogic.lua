TestGameObject = {}

function TestGameObject:New()
    local object = Entity:New()
    object.movingForward = false
    return object
end

TestGameObjectUpdater = {}

function TestGameObjectUpdater:Update(object, deltaMs)
    local objectEntityId = object:GetValue()
    if object.movingForward then
        --print("camera.movingForward == true")
        local rigidBodyEntityId = ECSWorld:GetPhysics_RigidBodyEntityId(objectEntityId)
        local rbEntityId = rigidBodyEntityId.id
        local setVelocityRequests = ECSWorld:GetPhysics_SetVelocityRequests(rbEntityId)
        local direction = ECSWorld:GetDirection3D(objectEntityId)
        setVelocityRequests:AddSetVelocityRequest(direction.x, direction.y, direction.z, 2.0)
        print("setVelocityRequests:AddSetVelocityRequest(direction.x, direction.y, direction.z, 2.0)")
    end

end

TestGameObjectInputProcessor = {}

function TestGameObjectInputProcessor:ProcessKeyboardInput(camera, Key, Event)

    local cameraEntityId = camera:GetValue()
    if Key == "MOVE_FORWARD" then
        if Event == "Pressed" then
            camera.movingForward = true
            --print("camera.movingForward = true")
        elseif Event == "Released" then
            camera.movingForward = false
            --print("camera.movingForward = false")
        end
    end

end

function TestGameObjectInputProcessor:ProcessMouseInput(camera, Key, Event, offsetX, offsetY, scroll)
    --print("TestGameObjectInputProcessor:ProcessMouseInput(")
    local cameraEntityId = camera:GetValue()
    local rigidBodyEntityId = ECSWorld:GetPhysics_RigidBodyEntityId(cameraEntityId)
    local rbEntityId = rigidBodyEntityId.id

    local setAngularVelocityRequests = ECSWorld:GetPhysics_SetAngularVelocityRequests(rbEntityId)
    --print("local setAngularVelocityRequests = ECSWorld:GetPhysics_SetAngularVelocityRequests(rbEntityId)")
    if offsetX ~= 0 then
        setAngularVelocityRequests:AddSetAngularVelocityRequest(0.0, 1.0, 0.0, 2.0 * offsetX * 0.02)
    else
        setAngularVelocityRequests:AddSetAngularVelocityRequest(0.0, 1.0, 0.0, 0)
    end
    --print("setAngularVelocityRequests:AddSetAngularVelocityRequest(0.0, 1.0, 0.0, 2.0)")
end
