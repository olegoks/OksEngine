local Math3D = require("Math3D")

Character = setmetatable({ },
    { __index = Entity }
)

function Character:New()
    local Character = Entity:New()
    Character.StopMoving = true
    Character.MovingForward = false
    Character.MovingBackward = false
    Character.MovingLeft = false
    Character.MovingRight = false
    function Character:Jump(dirX, dirY, dirZ)
        rigidBodyComponent = self:GetComponent("RigidBodyCapsule")
        rigidBodyComponent:ApplyForce(
            dirX, 
            dirY, 
            dirZ, 200000.0)
    end
    return Character
end

CharacterUpdater = {}

function CharacterUpdater:Update(Character, deltaMs)
    cameraComponent = Character:GetComponent("Camera")
    rigidBodyComponent = Character:GetComponent("RigidBodyCapsule")

    local velocity = 100.0
    local isMoving = false
    if Character.MovingForward then
        rigidBodyComponent:SetVelocity(
            cameraComponent:GetDirectionX(), 
            cameraComponent:GetDirectionY(),
             cameraComponent:GetDirectionZ(), velocity)
        isMoving = true
    end
    if Character.MovingBackward then
        rigidBodyComponent:SetVelocity(
            -cameraComponent:GetDirectionX(), 
            -cameraComponent:GetDirectionY(), 
            -cameraComponent:GetDirectionZ(), velocity)
        isMoving = true
    end
    if Character.MovingLeft then
        local x, y, z = Math3D:CrossProduct(
            cameraComponent:GetUpX(),
            cameraComponent:GetUpY(),
            cameraComponent:GetUpZ(),
            cameraComponent:GetDirectionX(),
            cameraComponent:GetDirectionY(),
            cameraComponent:GetDirectionZ()
        )
        local nx, ny, nz = Math3D:Normalize(x, y, z)
        rigidBodyComponent:SetVelocity(
            nx, 
            ny, 
            nz, velocity)
        isMoving = true
    end
    if Character.MovingRight then
        local x, y, z = Math3D:CrossProduct(
            cameraComponent:GetUpX(),
            cameraComponent:GetUpY(),
            cameraComponent:GetUpZ(),
            cameraComponent:GetDirectionX(),
            cameraComponent:GetDirectionY(),
            cameraComponent:GetDirectionZ()
        )
        local nx, ny, nz = Math3D:Normalize(x, y, z)
        rigidBodyComponent:SetVelocity(
            -nx, 
            -ny, 
            -nz, velocity)
        isMoving = true
    end

    if isMoving == false then
        rigidBodyComponent:SetVelocity(
            -nx, 
            -ny, 
            -nz, 0.0)
    end 

end

CharacterInputProcessor = {}

function CharacterInputProcessor:ProcessInput(Character, Key, Event, offsetX, offsetY)

    if Key == "W" then
        if Event == "Pressed" then
            Character.MovingForward = true
        elseif Event == "Released" then
            Character.MovingForward = false
        end
    elseif Key == "A" then
        if Event == "Pressed" then
            Character.MovingLeft = true
        elseif Event == "Released" then
            Character.MovingLeft = false
        end
    elseif Key == "D" then
        if Event == "Pressed" then
            Character.MovingRight = true
        elseif Event == "Released" then
            Character.MovingRight = false
        end
    elseif Key == "S" then
        if Event == "Pressed" then
            Character.MovingBackward = true
        elseif Event == "Released" then
            Character.MovingBackward = false
        end
    elseif Key == "Space" then
        if Event == "Pressed" then
            Character:Jump(0.0, 1.0, 0.0)
        end
    end  
    if Event == Released then
        Character.StopMoving = true
    end
    cameraComponent = Character:GetComponent("Camera")
    cameraComponent:DirectionLeft(offsetX / 10.0)
    cameraComponent:DirectionUp(offsetY / 10.0)

end



