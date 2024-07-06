

Camera = setmetatable({ },
    { __index = Entity }
)

function Camera:New()
    local Camera = Entity:New()
    Camera.DirectionUp = false
    Camera.DirectionDown = false
    Camera.MovingForward = false
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
    local sensetivity = 0.1
    if Camera.MovingForward then
        cameraComponent:Forward(sensetivity)
    end
    if Camera.MovingBackward then
        cameraComponent:Backward(sensetivity)
    end
    if Camera.MovingLeft then 
        cameraComponent:Left(sensetivity)
    end
    if Camera.MovingRight then 
        cameraComponent:Right(sensetivity)
    end
    if Camera.MovingUp then 
        cameraComponent:Up(sensetivity)
    end
    if Camera.MovingDown then 
        cameraComponent:Down(sensetivity)
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
    end 
    --print(Key.."  "..Event.."  "..offsetX.."  "..offsetY)


    cameraComponent = Camera:GetComponent("Camera")
    cameraComponent:DirectionUp(offsetY / 10.0)
    cameraComponent:DirectionLeft(offsetX / 10.0)
end



