--local function hook(event, line)
--    local info = debug.getinfo(2)
--    print(string.format("Event: %s, Line: %d [%s]", event, line, info.source))
--end
--
--debug.sethook(hook, "с")
Camera = {}

function extended(child, parent)
    setmetatable(child, { __index = parent })
end

extended(Camera, Entity)

function Camera:New()
    local camera = Entity:New()
    camera.Speed = 0.005
    camera.DirectionUp = false
    camera.DirectionDown = false
    camera.MovingForward = false
    camera.SpeedBoost = false
    camera.MovingBackward = false
    camera.MovingLeft = false
    camera.MovingRight = false
    camera.MovingUp = false
    camera.MovingDown = false
    --print("Camera:New() bnegin",     camera.MovingForward)

    function camera:Forward()
        --print("camera:Forward()")
        local position = self:GetComponent("WorldPosition3D")
        local direction = self:GetComponent("Direction3D")
        position.x = (position.x + direction.x * self.Speed)
        position.y = (position.y + direction.y * self.Speed)
        position.z = (position.z + direction.z * self.Speed)
    end

    function camera:Backward()
        local position = self:GetComponent("WorldPosition3D")
        local direction = self:GetComponent("Direction3D")
        position.x = (position.x - direction.x * self.Speed)
        position.y = (position.y - direction.y * self.Speed)
        position.z = (position.z - direction.z * self.Speed)
    end

    --For mouse
    function camera:DirectionUpDown(degree)
        print("DirectionUpDown degree:", degree)
        local position = self:GetComponent("WorldPosition3D")
        local direction = self:GetComponent("Direction3D")

        local up = self:GetComponent("Up3D")
        local upDirPerpendicular = Math3D():CrossProduct(direction, up)
        print("got components")
        upDirPerpendicular:Normalize()
        local newDirection = Math3D():RotateVector(direction, upDirPerpendicular, degree);
        newDirection:Normalize()
        direction.x = newDirection.x
        direction.y = newDirection.y
        direction.z = newDirection.z
        local newUp = Math3D():RotateVector(up, upDirPerpendicular, degree)
        newUp:Normalize()
        up.x = newUp.x
        up.y = newUp.y
        up.z = newUp.z
    end

    --For mouse
    function camera:DirectionLeftRight(degree)
        print("DirectionLeftRight degree:", degree)
  
        local direction = self:GetComponent("Direction3D")
        local up = self:GetComponent("Up3D")
        local verticalUp = Vector(0.0, 1.0, 0.0)

        local rotatedDirection = Math3D():RotateVector(direction, verticalUp, degree)
        rotatedDirection:Normalize()
        direction.x = rotatedDirection.x
        direction.y = rotatedDirection.y
        direction.z = rotatedDirection.z

        local rotatedUp = Math3D():RotateVector(up, verticalUp, degree)
        rotatedUp:Normalize()
        up.x = rotatedUp.x
        up.y = rotatedUp.y
        up.z = rotatedUp.z

        rotatedUp:Normalize()

    end

    function camera:Left(speed)

        local position   = self:GetComponent("WorldPosition3D")
        local direction  = self:GetComponent("Direction3D")
        local up         = self:GetComponent("Up3D")

        local test = Vector(up.x, up.y, up.z)

        local pVector = Math3D():CrossProduct(
            Vector(up.x, up.y, up.z),
            Vector(direction.x, direction.y, direction.z))
        
        pVector:Normalize()
        position.x = (position.x + pVector.x * speed)
        position.y = (position.y + pVector.y * speed)
        position.z = (position.z + pVector.z * speed)
    end

    function camera:Right(speed)
        local position   = self:GetComponent("WorldPosition3D")
        local direction  = self:GetComponent("Direction3D")
        local up         = self:GetComponent("Up3D")

        local pVector = Math3D():CrossProduct(
            Vector(direction.x,
            direction.y,
            direction.z),
            Vector(up.x,
            up.y,
            up.z))
        pVector:Normalize()

        position.x = (position.x + pVector.x * speed)
        position.y = (position.y + pVector.y * speed)
        position.z = (position.z + pVector.z * speed)
    end

    return camera
end

CameraUpdater = {}

function CameraUpdater:Update(camera, deltaMs)
    --print("CameraUpdater:Update")
    --print("camera.MovingForward: ", tostring(camera.MovingForward))
    if camera.MovingForward then
        camera:Forward(camera.Speed)
    end
    if camera.MovingBackward then
        camera:Backward(camera.Speed)
    end
    if camera.MovingLeft then
        camera:Left(camera.Speed)
    end
    if camera.MovingRight then
        camera:Right(camera.Speed)
    end
end

CameraInputProcessor = {}

function CameraInputProcessor:ProcessKeyboardInput(camera, Key, Event)
    if Key == "W" then
        if Event == "Pressed" then
            camera.MovingForward = true
        elseif Event == "Released" then
            camera.MovingForward = false
        end
    elseif Key == "A" then
        if Event == "Pressed" then
            camera.MovingLeft = true
        elseif Event == "Released" then
            camera.MovingLeft = false
        end
    elseif Key == "D" then
        if Event == "Pressed" then
            camera.MovingRight = true
        elseif Event == "Released" then
            camera.MovingRight = false
        end
    elseif Key == "Q" then
        if Event == "Pressed" then
            camera.MovingUp = true
        elseif Event == "Released" then
            camera.MovingUp = false
        end
    elseif Key == "E" then
        if Event == "Pressed" then
            camera.MovingDown = true
        elseif Event == "Released" then
            camera.MovingDown = false
        end
    elseif Key == "S" then
        if Event == "Pressed" then
            camera.MovingBackward = true
        elseif Event == "Released" then
            camera.MovingBackward = false
        end
    elseif Key == "LeftShift" then
        if Event == "Pressed" then
            camera.SpeedBoost = true
        elseif Event == "Released" then
            camera.SpeedBoost = false
        end
    end
end

function CameraInputProcessor:ProcessMouseInput(camera, Key, Event, offsetX, offsetY)
    print("OffsetX:", offsetX)
    print("OffsetY:", offsetY)
    camera:DirectionUpDown((offsetY / 1000.0))
    camera:DirectionLeftRight((offsetX / 1000.0))
end
