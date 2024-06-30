
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
    end
    if componentName == "ImmutableRenderGeometry" then
        return self.EngineEntity:GetImmutableRenderGeometry()
    end
    if componentName == "Camera" then
        return self.EngineEntity:GetCamera()
    end
    if componentName == "RigidBody" then
        return self.EngineEntity:GetRigidBody()
    end
end