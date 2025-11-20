
--Entity class
Entity = {}


function Entity:New()
    -- local entity = {}
    -- entity.EngineEntity = EngineEntity()

    local luaEntity = {
        engineEntity = EngineEntity()
    }

    -- Make Entity as metatable for entity
    -- It means that defines behaviour of main table using metamethods:
    -- 1. __index - access to undefined fields: if main table doesnt have field, 
    -- lua will go to metatable to find it and use.
    -- 2. __call - use table as a function:
    -- local multiplier = {factor = 2}
    -- local meta = {
    --  __call = function(self, x)
    --    return x * self.factor
    --  end
    -- }
    -- setmetatable(multiplier, meta)
    --print(multiplier(5))  -- 10
    -- 3. ...
    setmetatable(luaEntity, 
    { __index = function(t, k)

         -- Сначала методы подкласса
         local val = rawget(t, k)
         if val ~= nil then 
            return val
         end
         
         -- Затем C++ базовый класс
         local value = t.engineEntity[k]
         if type(value) == "function" then
            --print("__index function in engineEntity")
            --print(k)
            return function(...)
                return value(t.engineEntity, ...)
            end
         else 
            return value
         end
        end
        })
    --luaEntity.__index = engineEntity


    return luaEntity;
end

-- function Entity:SetWorld(world)
--     return self.EngineEntity:SetWorld(world)
-- end

-- function Entity:GetId()
--     return self.EngineEntity:GetId()
-- end

-- function Entity:SetId(id)
--     print("function Entity:SetId(id)")
--     self.EngineEntity:SetId(id)
-- end

-- function Entity:GetComponent(componentName)
--     if componentName == "WorldPosition3D" then 
--         return self.EngineEntity:GetWorldPosition3D() 
--     end
--     if componentName == "ImmutableRenderGeometry" then
--         return self.EngineEntity:GetImmutableRenderGeometry()
--     end
--     if componentName == "Camera" then
--         return self.EngineEntity:GetCamera()
--     end
--     if componentName == "RigidBodyBox" then
--         return self.EngineEntity:GetRigidBodyBox()
--     end
--     if componentName == "RigidBodyCapsule" then
--         return self.EngineEntity:GetRigidBodyCapsule()
--     end
--     if componentName == "Direction3D" then
--         return self.EngineEntity:GetDirection3D()
--     end
--     if componentName == "Up3D" then
--         return self.EngineEntity:GetUp3D()
--     end
--     if componentName == "Width" then
--         return self.EngineEntity:GetWidth()
--     end
--     if componentName == "Height" then
--         return self.EngineEntity:GetHeight()
--     end
-- end
