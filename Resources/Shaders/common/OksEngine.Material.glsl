#extension GL_ARB_gpu_shader_int64 : enable

#define MATERIAL_SET 1
#define MATERIAL_INFO_BINDING 0
#define DIFFUSE_MAP_BINDING 0
#define NORMAL_MAP_BINDING 1

struct MaterialInfo {
    uint infoIndex;
    uint diffuseMapIndex;
    uint normalMapIndex;
    uint metallicMapIndex;
};