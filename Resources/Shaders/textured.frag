#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require

#include "/common/OksEngine.Material.glsl"

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 inUV;


layout(location = 0) out vec4 outColor;

layout(set = MATERIAL_SET, binding = DIFFUSE_MAP_BINDING) uniform sampler2D texSampler;

void main() {
    outColor = texture(texSampler, inUV);
}
