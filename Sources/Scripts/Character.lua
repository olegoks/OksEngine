

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

    local sensetivity = 0.1
    if Character.MovingForward then
        print('Moving forward start.')
        print('rbComponent: '..rigidBodyComponent)
        rigidBodyComponent:ApplyForce(1.0, 0.0, 0.0, 1.0)
        print('Moving forward end')
        --cameraComponent:Forward(sensetivity)
    end
    if Character.MovingBackward then
        --cameraComponent:Backward(sensetivity)
    end
    if Character.MovingLeft then 
        --cameraComponent:Left(sensetivity)
    end
    if Character.MovingRight then 
        --cameraComponent:Right(sensetivity)
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
end



