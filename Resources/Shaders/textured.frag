#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_nonuniform_qualifier : require

#include "/common/OksEngine.Material.glsl"

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 inUV;


layout(location = 0) out vec4 outColor;

layout(set = MATERIAL_SET, binding = MATERIAL_DIFFUSE_MAP_BINDING) uniform sampler2D diffuseMaps[];
layout(set = MATERIAL_SET, binding = MATERIAL_NORMAL_MAP_BINDING) uniform sampler2D normalMaps[];
layout(set = MATERIAL_SET, binding = MATERIAL_METALLIC_MAP_BINDING) uniform sampler2D metallicMaps[];

layout(set = MATERIAL_INFO_SET, binding = MATERIAL_INFO_BINDING) uniform MaterialInfoBlock {
    uint diffuseMapIndex;
    uint normalMapIndex;
    uint metallicMapIndex;
} materialInfo;


void main() {
    outColor = texture(diffuseMaps[nonuniformEXT(materialInfo.diffuseMapIndex)], inUV);
}
