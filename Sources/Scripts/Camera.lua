local function hook(event, line)
    local info = debug.getinfo(2)
    print(string.format("Event: %s, Line: %d [%s]", event, line, info.source))
end

debug.sethook(hook, "с")
Camera = {}

function extended (child, parent)
    setmetatable(child,{__index = parent}) 
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

    function camera:Forward()
        local position = self:GetComponent("Position")
        local direction = self:GetComponent("Direction")
        position.x = (position.x + direction.x * self.Speed)
        position.y = (position.y + direction.y * self.Speed)
        position.z = (position.z + direction.z * self.Speed)
    end
    function camera:Backward()
        local position = self:GetComponent("Position")
        local direction = self:GetComponent("Direction")
        position.x = (position.x - direction.x * self.Speed)
        position.y = (position.y - direction.y * self.Speed)
        position.z = (position.z - direction.z * self.Speed)
    end

    function camera:DirectionUpDown(degree)
        local position = self:GetComponent("Position")
        local direction = self:GetComponent("Direction")
        
        local up = self:GetComponent("Up")
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

    function camera:DirectionLeftRight(degree)
        local direction = self:GetComponent("Direction")
        local up = self:GetComponent("Up")
        local rotatedDirection = Math3D():RotateVector(direction, up, degree)
        rotatedDirection:Normalize()
        direction.x = rotatedDirection.x
        direction.y = rotatedDirection.y
        direction.z = rotatedDirection.z
    end
    
    function camera:Left(speed)
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

        position.x = (position.x + npx * speed)
        position.y = (position.y + npy * speed)
        position.z = (position.z + npz * speed)
    end
    function camera:Right(speed)
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

        position.x = (position.x + npx * speed)
        position.y = (position.y + npy * speed)
        position.z = (position.z + npz * speed)
    end
    return camera
end

CameraUpdater = {}

function CameraUpdater:Update(camera, deltaMs)

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

    print("pre camera:DirectionUpDown(offsetY / 1000.0)")
    camera:DirectionUpDown(offsetY / 1000.0)
    print("camera:DirectionUpDown(offsetY / 1000.0)")
    camera:DirectionLeftRight(offsetX / 1000.0)
    print("camera:DirectionLeftRight(offsetX / 1000.0)")
end


