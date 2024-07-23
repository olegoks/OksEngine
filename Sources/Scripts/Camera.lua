

Camera = setmetatable({ },
    { __index = Entity }
)

function Camera:New()
    local Camera = Entity:New()
    Camera.Speed = 10
    Camera.Boost = 100.0
    Camera.DirectionUp = false
    Camera.DirectionDown = false
    Camera.MovingForward = false
    Camera.SpeedBoost = false
    Camera.MovingBackward = false
    Camera.MovingLeft = false
    Camera.MovingRight = false
    Camera.MovingUp = false
    Camera.MovingDown = false

    return Camera
end

CameraUpdater = {}

function CameraUpdater:Update(Camera, deltaMs)
    cameraComponent = Camera:GetComponent("Camera")
    if Camera.MovingForward then
        if Camera.SpeedBoost then
            cameraComponent:Forward(Camera.Speed * Camera.Boost)
        else
            cameraComponent:Forward(Camera.Speed)
        end
    end
    if Camera.MovingBackward then
        cameraComponent:Backward(Camera.Speed)
    end
    if Camera.MovingLeft then 
        cameraComponent:Left(Camera.Speed)
    end
    if Camera.MovingRight then 
        cameraComponent:Right(Camera.Speed)
    end
    if Camera.MovingUp then 
        cameraComponent:Up(Camera.Speed)
    end
    if Camera.MovingDown then 
        cameraComponent:Down(Camera.Speed)
    end
end

CameraInputProcessor = {}

function CameraInputProcessor:ProcessInput(Camera, Key, Event, offsetX, offsetY)

    if Key == "W" then
        if Event == "Pressed" then
            Camera.MovingForward = true
        elseif Event == "Released" then
            Camera.MovingForward = false
        end
    elseif Key == "A" then
        if Event == "Pressed" then
            Camera.MovingLeft = true
        elseif Event == "Released" then
            Camera.MovingLeft = false
        end
    elseif Key == "D" then
        if Event == "Pressed" then
            Camera.MovingRight = true
        elseif Event == "Released" then
            Camera.MovingRight = false
        end
    elseif Key == "Q" then
        if Event == "Pressed" then
            Camera.MovingUp = true
        elseif Event == "Released" then
            Camera.MovingUp = false
        end
    elseif Key == "E" then
        if Event == "Pressed" then
            Camera.MovingDown = true
        elseif Event == "Released" then
            Camera.MovingDown = false
        end
    elseif Key == "S" then
        if Event == "Pressed" then
            Camera.MovingBackward = true
        elseif Event == "Released" then
            Camera.MovingBackward = false
        end
    elseif Key == "LeftShift" then
        if Event == "Pressed" then
            Camera.SpeedBoost = true
        elseif Event == "Released" then
            Camera.SpeedBoost = false
        end
    end 
    --print(Key.."  "..Event.."  "..offsetX.."  "..offsetY)


    cameraComponent = Camera:GetComponent("Camera")
    cameraComponent:DirectionLeft(offsetX / 10.0)
    cameraComponent:DirectionUp(offsetY / 10.0)
end



