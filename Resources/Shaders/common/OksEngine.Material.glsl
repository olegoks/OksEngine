#extension GL_ARB_gpu_shader_int64 : enable

#define MATERIAL_SET 1
#define MATERIAL_DIFFUSE_MAP_BINDING 0
#define MATERIAL_NORMAL_MAP_BINDING 1
#define MATERIAL_METALLIC_MAP_BINDING 2

#define MATERIAL_INFO_SET 2
#define MATERIAL_INFO_BINDING 0

#define SHADER_USE_MATERIAL                                                                             \
layout(set = MATERIAL_SET, binding = MATERIAL_DIFFUSE_MAP_BINDING) uniform sampler2D diffuseMaps[];     \
layout(set = MATERIAL_SET, binding = MATERIAL_NORMAL_MAP_BINDING) uniform sampler2D normalMaps[];       \
layout(set = MATERIAL_SET, binding = MATERIAL_METALLIC_MAP_BINDING) uniform sampler2D metallicMaps[];   \
                                                                                                        \
layout(set = MATERIAL_INFO_SET, binding = MATERIAL_INFO_BINDING) uniform MaterialInfoBlock {            \
    uint diffuseMapIndex;                                                                               \
    uint normalMapIndex;                                                                                \
    uint metallicMapIndex;                                                                              \
} materialInfo;                                                                                         \

#define MATERIAL_GET_DIFFUSE_MAP diffuseMaps[nonuniformEXT(materialInfo.diffuseMapIndex)]
