#extension GL_ARB_gpu_shader_int64 : enable

#define RENDER__MODEL__DATA__SET 3
#define RENDER__MODEL__DATA__IDS_TO_COMPONENT_INDICES_BINDING 0
#define RENDER__MODEL__DATA__MODEL__ENTITY_IDS_BINDING 1
#define RENDER__MODEL__DATA__MESH_DATA_ENTITY_IDS_BINDING 2
#define RENDER__MODEL__DATA__NODE_DATA_ENTITY_IDS_BINDING 3
#define RENDER__MODEL__DATA__ANIMATIONS__BINDING 4


#define RENDER__MODEL__DATA\
    layout(                 \
        std430,                         \
        set = RENDER__MODEL__DATA__SET,                                                 \
        binding = RENDER__MODEL__DATA__IDS_TO_COMPONENT_INDICES_BINDING) buffer {    \
        uint64_t modelDataIdToComponentIndex_[];                                                           \
    };                                       \
                                                                                                                                                                \
    layout(std430,
        set = RENDER__MODEL__DATA__SET,
        binding = RENDER__MODEL__DATA__MODEL__ENTITY_IDS_BINDING)buffer NodeEntityIdsToComponentIndices {    \
        uint64_t nodeComponentsIndices_[];                                                           \
    };                                       \



#define RENDER__MODEL__NODE__SET 4
#define RENDER__MODEL__NODE__DATA__SET 5
