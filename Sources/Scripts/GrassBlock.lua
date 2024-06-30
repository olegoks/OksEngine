

GrassBlock = setmetatable({ },
    { __index = Entity }
)

function GrassBlock:New()
    local GrassBlock = Entity:New()
    GrassBlock.Counter = 0;
    
    function GrassBlock:IncreaseCounter()
        self.Counter = self.Counter + 1;
        --print(self.Counter)
    end

    function GrassBlock:ChangePosition()

    end

    return GrassBlock
end

GrassBlockUpdater = {}


function GrassBlockUpdater:Update(GrassBlock, deltaMs)
    --print("Update method of TestObject called")
    GrassBlock:IncreaseCounter()
    position = GrassBlock:GetComponent("Position")
    x = position:GetX()
    position:SetX(x + 1)
    renderGeometry = GrassBlock:GetComponent("ImmutableRenderGeometry")
    graducesPerSecond = 30;

    renderGeometry:Rotate(graducesPerSecond * deltaMs / 1000, 1, 0, 0)
    --print(graducesPerSecond * deltaMs / 1000)
end




