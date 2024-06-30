

Camera = setmetatable({ },
    { __index = Entity }
)

function Camera:New()
    local Camera = Entity:New()
    Camera.Counter = 0;
    Camera.WPressed = false
    Camera.SPressed = false
    function Camera:IncreaseCounter()
        self.Counter = self.Counter + 1;
        --print(self.Counter)
    end

    function Camera:ChangePosition()

    end

    return Camera
end

CameraUpdater = {}

function CameraUpdater:Update(Camera, deltaMs)
    position = Camera:GetComponent("Position")
    cameraComp = Camera:GetComponent("Camera")
    if Camera.WPressed then
        position:SetX(position:GetX() + cameraComp:GetDirectionX() * 0.01)
        position:SetY(position:GetY() + cameraComp:GetDirectionY() * 0.01)
        position:SetZ(position:GetZ() + cameraComp:GetDirectionZ() * 0.01)
    end
    if Camera.SPressed then
        position:SetX(position:GetX() - cameraComp:GetDirectionX() * 0.01)
        position:SetY(position:GetY() - cameraComp:GetDirectionY() * 0.01)
        position:SetZ(position:GetZ() - cameraComp:GetDirectionZ() * 0.01)
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
    
end



