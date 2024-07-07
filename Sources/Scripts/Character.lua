

Character = setmetatable({ },
    { __index = Entity }
)

function Character:New()
    local Character = Entity:New()
    Character.MovingForward = false
    Character.MovingBackward = false
    Character.MovingLeft = false
    Character.MovingRight = false
    return Character
end

CharacterUpdater = {}

function CharacterUpdater:Update(Character, deltaMs)
    cameraComponent = Character:GetComponent("Camera")
    rigidBodyComponent = Character:GetComponent("RigidBodyCapsule")

    if Character.MovingForward then
        rigidBodyComponent:ApplyForce(
            cameraComponent:GetDirectionX(), 
            cameraComponent:GetDirectionY(),
             cameraComponent:GetDirectionZ(), 1000.0)
    end
    if Character.MovingBackward then
        rigidBodyComponent:ApplyForce(
            -cameraComponent:GetDirectionX(), 
            -cameraComponent:GetDirectionY(), 
            -cameraComponent:GetDirectionZ(), 1000.0)
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
    end 
    cameraComponent = Character:GetComponent("Camera")
    
   -- cameraComponent:DirectionUp(offsetY / 10.0)
    cameraComponent:DirectionLeft(offsetX / 10.0)
end



