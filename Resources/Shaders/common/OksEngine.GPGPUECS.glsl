#extension GL_ARB_gpu_shader_int64 : enable

struct WorldPosition3D {
    vec4 position_;
};
struct WorldRotation3D {
    vec4 rotation_;
};
struct WorldScale3D {
    vec4 scale_;
};

struct ModelNodeEntityIds {
    uint64_t nodeIds_[512];
};

struct ModelEntityIds {
    uint64_t modelIds_[1024];
};
