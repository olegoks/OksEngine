local Math3D = {}

-- function Math3D:CrossProduct(vector1, vector2)

--     print('Math3D:CrossProduct(vector1, vector2)')
--     local x1 = vector1:GetX()
--     local y1 = vector1:GetY()
--     local z1 = vector1:GetZ()
--     local x2 = vector2:GetX()
--     local y2 = vector2:GetY()
--     local z2 = vector2:GetZ()

--     local resX = y1 * z2 - z1 * y2
--     local resY = z1 * x2 - x1 * z2
--     local resZ = x1 * y2 - y1 * x2 

--     return Vector(resX, resY, resZ)
-- end 

-- function Math3D:CrossProduct(vector1, vector2)
--     return Math3D():CrossProduct(
--         vector1:GetX(), vector1:GetY(), vector1:GetZ(),
--         vector2:GetX(), vector2:GetY(), vector2:GetZ()
--     )
-- end 

function Math3D:CrossProduct(x1, y1, z1, x2, y2, z2)

    print('Math3D:CrossProduct(vector1, vector2)')
    -- local x1 = vector1:GetX()
    -- local y1 = vector1:GetY()
    -- local z1 = vector1:GetZ()
    -- local x2 = vector2:GetX()
    -- local y2 = vector2:GetY()
    -- local z2 = vector2:GetZ()

    local resX = y1 * z2 - z1 * y2
    local resY = z1 * x2 - x1 * z2
    local resZ = x1 * y2 - y1 * x2 

    return resX, resY, resZ
    --return Vector(resX, resY, resZ)
end 


function Math3D:DotProduct(x1, y1, z1, x2, y2, z2)
    local result = 0.0
    result = result + x1 * x2
    result = result + y1 * y2
    result = result + z1 * z2
    return result
end 

function Math3D:Length(x, y, z)
    return math.sqrt(x * x + y * y + z * z)
end 

function Math3D:Normalize(x, y, z)
    local length = Math3D:Length(x, y, z)
    return x / length, y / length, z / length
end 

-- return Math3D