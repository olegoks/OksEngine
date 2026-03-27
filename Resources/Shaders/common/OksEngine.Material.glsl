#extension GL_ARB_gpu_shader_int64 : enable

#define MATERIAL_SET 1
#define MATERIAL_DIFFUSE_MAP_BINDING 0
#define MATERIAL_NORMAL_MAP_BINDING 1
#define MATERIAL_METALLIC_MAP_BINDING 2

#define MATERIAL_INFO_SET 2
#define MATERIAL_INFO_BINDING 0

struct MaterialInfo {
    uint diffuseMapIndex;
    uint normalMapIndex;
    uint metallicMapIndex;
};