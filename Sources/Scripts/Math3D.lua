local Math3D = {}

function Math3D:CrossProduct(x1, y1, z1, x2, y2, z2)
    local resX = y1 * z2 - z1 * y2
    local resY = z1 * x2 - x1 * z2
    local resZ = x1 * y2 - y1 * x2 

    return resX, resY, resZ
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

return Math3D