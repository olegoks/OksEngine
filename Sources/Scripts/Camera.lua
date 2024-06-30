

Camera = setmetatable({ },
    { __index = Entity }
)

function Camera:New()
    local Camera = Entity:New()
    Camera.Counter = 0;
    
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
    --print("Update method of TestObject called")
    --Camera:IncreaseCounter()
    -- position = Camera:GetComponent("Position")
    -- x = position:GetX()
    -- position:SetX(x + 1)
    --print(graducesPerSecond * deltaMs / 1000)
end

CameraInputProcessor = {}

function CameraInputProcessor:ProcessInput(Camera, Input)

    position = Camera:GetComponent("Position")
    camera = Camera:GetComponent("Camera")
    if Input == "W" then
        x = position:GetX()
        position:SetX(x + 1)
    elseif Input == "A" then
        y = position:GetY()
        position:SetY(y + 1)
    elseif Input == "D" then
    elseif Input == "S" then
    end 

end



