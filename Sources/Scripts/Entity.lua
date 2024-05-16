
Entity = {}

function Entity:New()
    local entity = {}
    entity.EngineEntity = EngineEntity()
    setmetatable(entity, self)
    self.__index = self
    return entity;
end

function Entity:GetId()
    return self.EngineEntity:GetId()
end

function Entity:SetId(id)
    self.EngineEntity:SetId(id)
end

function Entity:GetComponent(componentName)
    if componentName == "Position" then 
        return self.EngineEntity:GetPosition() 
    else
        return nil
    end
end