

AttachedCamera = setmetatable({ },
    { __index = Entity }
)

function AttachedCamera:New()
    local AttachedCamera = Entity:New()
    return AttachedCamera
end

AttachedCameraUpdater = {}

function AttachedCameraUpdater:Update(AttachedCamera, deltaMs)
    cameraComponent = AttachedCamera:GetComponent("Camera")
    rigidBodyComponent = AttachedCamera:GetComponent("RigidBodyCapsule")
    
end

AttachedCameraInputProcessor = {}

function AttachedCameraInputProcessor:ProcessInput(AttachedCamera, Key, Event, offsetX, offsetY)
end



