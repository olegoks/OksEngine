

TestObject = setmetatable({ },
    { __index = Entity }
)

function TestObject:New()
    local testObject = Entity:New()
    testObject.Counter = 0;
    
    function testObject:IncreaseCounter()
        self.Counter = self.Counter + 1;
        --print(self.Counter)
    end

    function testObject:ChangePosition()

    end

    return testObject
end

TestObjectUpdater = {}

function TestObjectUpdater:Update(testObject, deltaMs)
    --print("Update method of TestObject called")
    --testObject:IncreaseCounter()
    --position = testObject:GetComponent("Position")
    --x = position:GetX()
    --position:SetX(x + 1)
    --renderGeometry = testObject:GetComponent("ImmutableRenderGeometry")
    --graducesPerSecond = 30;

    --renderGeometry:Rotate(graducesPerSecond * deltaMs / 1000, 1, 0, 0)
    --print(graducesPerSecond * deltaMs / 1000)
end




