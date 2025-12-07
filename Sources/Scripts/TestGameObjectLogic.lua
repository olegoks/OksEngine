TestGameObject = {}

function TestGameObject:New()
    local object = Entity:New()
    object.movingForward = false
    object.movingBack = false
    object.rotating = false
    return object
end

TestGameObjectUpdater = {}

function TestGameObjectUpdater:Update(object, deltaMs)
    local objectEntityId = object:GetIdValue()
    local modelEntityId = ECSWorld:GetRender_Mdl_ModelEntity(objectEntityId)
    local rigidBodyEntityId = ECSWorld:GetPhysics_RigidBodyEntityId(objectEntityId)
    local rbEntityId = rigidBodyEntityId.id
    local setVelocityRequests = ECSWorld:GetPhysics_SetVelocityRequests(rbEntityId)
    local direction = ECSWorld:GetDirection3D(objectEntityId)

    if object.movingForward then
        setVelocityRequests:AddSetVelocityRequest(direction.x, direction.y, direction.z, 2.0)
        if not ECSWorld:IsComponentExist(modelEntityId.id,  "AnimationInProgress") then
            ECSWorld:CreateRunModelAnimation(modelEntityId.id, "Walk")
        end
    else 
        setVelocityRequests:AddSetVelocityRequest(direction.x, direction.y, direction.z, 0.0)
        if not ECSWorld:IsComponentExist(modelEntityId.id,  "AnimationInProgress") then
            ECSWorld:CreateRunModelAnimation(modelEntityId.id, "Idle")
        end
    end

end

TestGameObjectInputProcessor = {}

function TestGameObjectInputProcessor:ProcessKeyboardInput(object, Key, Event)

    local objectEntityId = object:GetIdValue()
    local modelEntityId = ECSWorld:GetRender_Mdl_ModelEntity(objectEntityId)
    if Key == "MOVE_FORWARD" then
        if Event == "Pressed" then
            object.movingForward = true
            --print("local animationInProgressName = AnimationInProgress")
            if ECSWorld:IsComponentExist(modelEntityId.id,  "AnimationInProgress") then
                ECSWorld:RemoveComponent(modelEntityId.id, "AnimationInProgress")
            end
            ECSWorld:CreateRunModelAnimation(modelEntityId.id, "Walk")
            
            --print("camera.movingForward = true")
        elseif Event == "Released" then
            object.movingForward = false
            if ECSWorld:IsComponentExist(modelEntityId.id,  "AnimationInProgress") then
                ECSWorld:RemoveComponent(modelEntityId.id, "AnimationInProgress")
            end
            ECSWorld:CreateRunModelAnimation(modelEntityId.id, "Idle")
            --print("camera.movingForward = false")
        end
    end

end

function TestGameObjectInputProcessor:ProcessMouseInput(camera, Key, Event, offsetX, offsetY, scroll)
    --print("TestGameObjectInputProcessor:ProcessMouseInput(")
    local cameraEntityId = camera:GetIdValue()
    local rigidBodyEntityId = ECSWorld:GetPhysics_RigidBodyEntityId(cameraEntityId)
    local rbEntityId = rigidBodyEntityId.id

    local setAngularVelocityRequests = ECSWorld:GetPhysics_SetAngularVelocityRequests(rbEntityId)
    --print("local setAngularVelocityRequests = ECSWorld:GetPhysics_SetAngularVelocityRequests(rbEntityId)")
    if offsetX ~= 0 then
        setAngularVelocityRequests:AddSetAngularVelocityRequest(0.0, 1.0, 0.0, 2.0 * offsetX * 0.02)
    end
    --print("setAngularVelocityRequests:AddSetAngularVelocityRequest(0.0, 1.0, 0.0, 2.0)")
end
