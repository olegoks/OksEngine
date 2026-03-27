#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "/common/OksEngine.Material.glsl"

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 inUV;


layout(location = 0) out vec4 outColor;

SHADER_USE_MATERIAL

void main() {
    outColor = texture(MATERIAL_GET_DIFFUSE_MAP, inUV);
}
