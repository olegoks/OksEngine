-- local Math3D = require("Math3D")

Camera = setmetatable({ },
    { __index = Entity }
)

function Camera:New()
    local Camera = Entity:New()
    Camera.Speed = 3
    Camera.Boost = 10.0
    Camera.DirectionUp = false
    Camera.DirectionDown = false
    Camera.MovingForward = false
    Camera.SpeedBoost = false
    Camera.MovingBackward = false
    Camera.MovingLeft = false
    Camera.MovingRight = false
    Camera.MovingUp = false
    Camera.MovingDown = false
    function Camera:Forward(speed)
        local position = self:GetComponent("Position")
        local camera = self:GetComponent("Camera")
        position:SetX(position:GetX() + camera:GetDirectionX() * speed)
        position:SetY(position:GetY() + camera:GetDirectionY() * speed)
        position:SetZ(position:GetZ() + camera:GetDirectionZ() * speed)
    end
    function Camera:Backward(speed)
        local position = self:GetComponent("Position")
        local camera = self:GetComponent("Camera")
        position:SetX(position:GetX() - camera:GetDirectionX() * speed)
        position:SetY(position:GetY() - camera:GetDirectionY() * speed)
        position:SetZ(position:GetZ() - camera:GetDirectionZ() * speed)
    end
    function Camera:Left(speed)
        local position = self:GetComponent("Position")
        local camera = self:GetComponent("Camera")
        local px, py, pz  = Math3D:CrossProduct(
            camera:GetUpX(),
            camera:GetUpY(),
            camera:GetUpZ(),
            camera:GetDirectionX(), 
            camera:GetDirectionY(),
            camera:GetDirectionZ())
        local npx, npy, npz = Math3D:Normalize(px, py, pz)

        position:SetX(position:GetX() + npx * speed)
        position:SetY(position:GetY() + npy * speed)
        position:SetZ(position:GetZ() + npz * speed)
    end
    function Camera:Right(speed)
        local position = self:GetComponent("Position")
        local camera = self:GetComponent("Camera")
        local px, py, pz  = Math3D:CrossProduct(
            camera:GetDirectionX(), 
            camera:GetDirectionY(),
            camera:GetDirectionZ(),
            camera:GetUpX(),
            camera:GetUpY(),
            camera:GetUpZ())
        local npx, npy, npz = Math3D:Normalize(px, py, pz)

        position:SetX(position:GetX() + npx * speed)
        position:SetY(position:GetY() + npy * speed)
        position:SetZ(position:GetZ() + npz * speed)
    end
    return Camera
end

CameraUpdater = {}

function CameraUpdater:Update(Camera, deltaMs)

    local direction = Camera:GetComponent("Direction")
    local dirX = direction:GetX()
    local dirY = direction:GetY()
    local dirZ = direction:GetZ()

    local up = Camera:GetComponent("Up")
    local upX = up:GetX()
    local upY = up:GetY()
    local upZ = up:GetZ()

    local position = Camera:GetComponent("Position")
    local posX = position:GetX()
    local posY = position:GetY()
    local posZ = position:GetZ()

    --print("got camera ingo")

    if Camera.MovingForward then
        --print("Moving forward")
        if Camera.SpeedBoost then
            cameraComponent:Forward(Camera.Speed * Camera.Boost)
            position:SetX(posX + dirX * Camera.Boost)
            position:SetY(posY + dirY * Camera.Boost)
            position:SetZ(posZ + dirZ * Camera.Boost)
        else
            position:SetX(posX + dirX)
            position:SetY(posY + dirY)
            position:SetZ(posZ + dirZ)
        end
    end
    if Camera.MovingBackward then
        if Camera.SpeedBoost then
            cameraComponent:Forward(Camera.Speed * Camera.Boost)
            position:SetX(posX - dirX * Camera.Boost)
            position:SetY(posY - dirY * Camera.Boost)
            position:SetZ(posZ - dirZ * Camera.Boost)
        else
            position:SetX(posX - dirX)
            position:SetY(posY - dirY)
            position:SetZ(posZ - dirZ)
        end
    end
    if Camera.MovingLeft then 
        
        

        Camera:Left(Camera.Speed)
    end
    if Camera.MovingRight then 
        Camera:Right(Camera.Speed)
    end
    -- if Camera.MovingUp then 
    --     cameraComponent:Up(Camera.Speed)
    -- end
    -- if Camera.MovingDown then 
    --     cameraComponent:Down(Camera.Speed)
    -- end
end

CameraInputProcessor = {}

function CameraInputProcessor:ProcessInput(Camera, Key, Event, offsetX, offsetY)



    if Key == "W" then
        if Event == "Pressed" then
            Camera.MovingForward = true
            print("pressed forward")
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

    directionComponent = Camera:GetComponent("Direction")

    --print("Direction got")
    local directionVector = Vector(directionComponent:GetX(),directionComponent:GetY(), directionComponent:GetZ())

    --print("Direction vector created"..directionVector:GetX().."  "..directionVector:GetY().."  "..directionVector:GetZ().."  ")

    local rotatedVector = Math3D():RotateVector(directionVector, Vector(0, 1, 0), offsetX / 1000.0)
    local up = Camera:GetComponent("Up")
    local upX = up:GetX()
    local upY = up:GetY()
    local upZ = up:GetZ()
   -- print("pre crodd product")
    local upDirPerpendicular = Math3D():CrossProduct(directionVector, Vector(upX, upY, upZ))
   -- print("post crodd product")
    rotatedVector = Math3D():RotateVector(rotatedVector, upDirPerpendicular, offsetY / 1000.0)
    --print("x y z"..upDirPerpendicular:GetX().. "   "..upDirPerpendicular:GetY().."   "..upDirPerpendicular:GetZ())
    local rotatedVector = Math3D():RotateVector(directionVector, Vector(0, 1, 0), offsetX / 1000.0)
   -- print("RotateVector")
    --print("Direction vector rotated"..rotatedVector:GetX().."  "..rotatedVector:GetY().."  "..rotatedVector:GetZ().."  ")
    -- Direction left
    -- cameraComponent:DirectionLeft(offsetX / 10.0)
    directionComponent:Set(rotatedVector)
    -- Direction left
    -- cameraComponent:DirectionUp(offsetY / 10.0)
    -- directionComponent:Rotate(0.0, 1.0, 0.0, offsetX / -10.0)
end



