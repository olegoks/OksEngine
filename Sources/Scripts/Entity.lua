
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
    if componentName == "RigidBodyBox" then
        return self.EngineEntity:GetRigidBodyBox()
    end
    if componentName == "RigidBodyCapsule" then
        return self.EngineEntity:GetRigidBodyCapsule()
    end
    if componentName == "Direction" then
        return self.EngineEntity:GetDirection()
    end
    if componentName == "Up" then
        return self.EngineEntity:GetUp()
    end
    if componentName == "Width" then
        return self.EngineEntity:GetWidth()
    end
    if componentName == "Height" then
        return self.EngineEntity:GetHeight()
    end
    print('Entity:GetComponent('..componentName..')')
end

