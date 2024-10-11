Vector = {}

function Vector:New()
    local vector = {}
    setmetatable(vector, self)
    self.__index = self

    vector.x = 0.0;
    vector.y = 0.0;
    vector.z = 0.0;

    function Vector:Normalize()
        rigidBodyComponent = self.x

    return vector;
end