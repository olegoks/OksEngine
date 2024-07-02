

Camera = setmetatable({ },
    { __index = Entity }
)

function Camera:New()
    local Camera = Entity:New()
    Camera.MovingForward = false
    Camera.MovingBackward = false
    Camera.MovingLeft = false
    Camera.MovingRight = false
    Camera.MovingUp = false
    Camera.MovingDown = false
    Camera.yaw = -90.0
    Camera.fov = 45.0
    Camera.pitch = 0.0
    function Camera:IncreaseCounter()
        self.Counter = self.Counter + 1;
        --print(self.Counter)
    end

    function Camera:MoveForward(speed)
        
        -- position = Camera:GetComponent("Position")
        -- cameraComp = Camera:GetComponent("Camera")
        -- position:SetX(position:GetX() + cameraComp:GetDirectionX() * speed)
        -- position:SetY(position:GetY() + cameraComp:GetDirectionY() * speed)
        -- position:SetZ(position:GetZ() + cameraComp:GetDirectionZ() * speed)
    end

    function Camera:MoveBackward(speed)
        position = Camera:GetComponent("Position")
        cameraComp = Camera:GetComponent("Camera")
        position:SetX(position:GetX() - cameraComp:GetDirectionX() * 0.1)
        position:SetY(position:GetY() - cameraComp:GetDirectionY() * 0.1)
        position:SetZ(position:GetZ() - cameraComp:GetDirectionZ() * 0.1)
    end
    function Camera:UpDirection()

    end

    return Camera
end

CameraUpdater = {}

function CameraUpdater:Update(Camera, deltaMs)
    print("Moving forward")
    if Camera.MovingForward then
        Camera:Forward(0.1)
    elseif Camera.MovingBackward then
        Camera:Backward(0.1)
    elseif Camera.MovingLeft then 
        Camera:Left(0.1)
    elseif Camera.MovingRight then 
        Camera:Right(0.1)
    elseif Camera.MovingUp then 
        Camera:Up(0.1)
    elseif Camera.MovingDown then 
        Camera:Down(0.1)
    end
    print("Success")
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
    elseif Key == "D" then
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
    print(Key.."  "..Event.."  "..offsetX.."  "..offsetY)
    -- offsetX = offsetX * 0.05
    -- offsetY = offsetY * 0.05

    -- Camera.yaw = Camera.yaw + offsetY;
    -- Camera.pitch =  Camera.pitch - offsetX;

    -- if Camera.pitch > 89.0 then
    --     Camera.pitch = 89.0
    -- elseif Camera.pitch < -89.0 then
    --     Camera.pitch = -89.0
    -- end 

    -- local frontX = math.cos(math.rad(Camera.pitch)) *  math.cos(math.rad(Camera.yaw))
    -- local frontY = math.sin(math.rad(Camera.pitch))
    -- local frontZ = math.cos(math.rad(Camera.pitch)) * math.sin(math.rad(Camera.yaw))

    -- cameraComp:SetDirectionX(frontX)
    -- cameraComp:SetDirectionY(frontY)
    -- cameraComp:SetDirectionZ(frontZ)

  
end



