#version 450
#extension GL_ARB_gpu_shader_int64 : enable

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) in vec3 inPosition;

layout(push_constant) uniform PushConstants { 
    uint64_t id_;      // Components index for current mesh.
};

void main() {
    gl_Position = camera.proj * camera.view * vec4(inPosition, 1.0);
}
