

TestObject = setmetatable({ },
    { __index = Entity }
)

function TestObject:New()
    local testObject = Entity:New()
    testObject.Counter = 0;
    
    function testObject:IncreaseCounter()
        self.Counter = self.Counter + 1;
        print(self.Counter)
    end

    function testObject:ChangePosition()

    end

    return testObject
end

TestObjectUpdater = {}

function TestObjectUpdater:Update(testObject)
    print("Update method of TestObject called")
    testObject:IncreaseCounter()
    position = testObject:GetComponent("Position")
    x = position:GetX()
    position:SetX(x + 1)
    print("Update method of TestObject ended")
end




