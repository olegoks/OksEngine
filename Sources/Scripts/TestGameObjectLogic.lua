TestGameObject = {}

function extended(child, parent)
    setmetatable(child, { __index = parent })
end

extended(TestGameObject, Entity)

function TestGameObject:New()
    local object = Entity:New()

    return object
end

TestGameObjectUpdater = {}

function TestGameObjectUpdater:Update(camera, deltaMs)
   
end

TestGameObjectInputProcessor = {}

function TestGameObjectInputProcessor:ProcessKeyboardInput(camera, Key, Event)
    
end

function TestGameObjectInputProcessor:ProcessMouseInput(camera, Key, Event, offsetX, offsetY, scroll)

end
