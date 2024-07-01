

Camera = setmetatable({ },
    { __index = Entity }
)

function Camera:New()
    local Camera = Entity:New()
    Camera.WPressed = false
    Camera.SPressed = false
    Camera.yaw = -90.0
    Camera.fov = 45.0
    Camera.pitch = 0.0
    function Camera:IncreaseCounter()
        self.Counter = self.Counter + 1;
        --print(self.Counter)
    end

    function Camera:MoveForward(speed)
        position = Camera:GetComponent("Position")
        cameraComp = Camera:GetComponent("Camera")
        position:SetX(position:GetX() + cameraComp:GetDirectionX() * speed)
        position:SetY(position:GetY() + cameraComp:GetDirectionY() * speed)
        position:SetZ(position:GetZ() + cameraComp:GetDirectionZ() * speed)
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
    position = Camera:GetComponent("Position")
    cameraComp = Camera:GetComponent("Camera")
    if Camera.WPressed then
       Camera:MoveForward(0.1)
    end
    if Camera.SPressed then
       Camera:MoveBackward(0.1)
    end
end

CameraInputProcessor = {}

function CameraInputProcessor:ProcessInput(Camera, Key, Event, offsetX, offsetY)
    print(Key.."  "..Event.."  "..offsetX.."  "..offsetY)
    position = Camera:GetComponent("Position")
    cameraComp = Camera:GetComponent("Camera")
    if Key == "W" then
        if Event == "Pressed" then
            Camera.WPressed = true
        elseif Event == "Released" then
            Camera.WPressed = false
        end
    elseif Key == "A" then
        y = position:GetY()
        position:SetY(y + 1)
    elseif Key == "D" then
    elseif Key == "S" then
        if Event == "Pressed" then
            Camera.SPressed = true
        elseif Event == "Released" then
            Camera.SPressed = false
        end
    end 
    
    offsetX = offsetX * 0.05
    offsetY = offsetY * 0.05

    Camera.yaw = Camera.yaw + offsetY;
    Camera.pitch =  Camera.pitch - offsetX;

    if Camera.pitch > 89.0 then
        Camera.pitch = 89.0
    elseif Camera.pitch < -89.0 then
        Camera.pitch = -89.0
    end 

    local frontX = math.cos(math.rad(Camera.pitch)) *  math.cos(math.rad(Camera.yaw))
    local frontY = math.sin(math.rad(Camera.pitch))
    local frontZ = math.cos(math.rad(Camera.pitch)) * math.sin(math.rad(Camera.yaw))

    -- cameraComp:SetDirectionX(frontX)
    -- cameraComp:SetDirectionY(frontY)
    -- cameraComp:SetDirectionZ(frontZ)

    print("Success")
end



