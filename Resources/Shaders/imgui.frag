#version 450 core
#extension GL_EXT_nonuniform_qualifier : require
#extension GL_GOOGLE_include_directive : require
#include "/common/OksEngine.Material.glsl"
//location - color attachment index
layout(location = 0) out vec4 fColor;


SHADER_USE_MATERIAL

layout(location = 0) in struct { vec4 Color; vec2 UV; } In;

void main()
{
    fColor = In.Color * texture(MATERIAL_GET_DIFFUSE_MAP, In.UV.st);
    
}