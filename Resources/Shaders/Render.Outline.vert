#version 450
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_GOOGLE_include_directive : require
#include "/common/OksEngine.Common.Math3D.glsl"

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants { 
    uint64_t id_;      // Components index for current mesh.
    float posX_;
    float posY_;
    float posZ_;

    float rotW_;
    float rotX_;
    float rotY_;
    float rotZ_;
};

vec4 idToColor(uint id) {
    // Используем золотое сечение для генерации HUE
    float hue = fract(float(id) * 0.618033988749895);
    
    // HSV to RGB
    vec3 rgb = clamp(
        abs(mod(hue * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
        0.0, 1.0
    );
    
    // Делаем цвета насыщенными и яркими
    return vec4(rgb * 0.8 + 0.2, 1.0);
}

void main() {
    outColor = idToColor(uint(id_));
    // outColor = vec4(
    //     float(id_) / 255.0,  // R
    //     1.0 - (float(id_) / 255.0),                      // G
    //     0.0,                      // B
    //     1.0                       // A
    // );
    mat4 matrix = RTS_to_mat4_optimized(vec3(posX_, posY_, posZ_),
        vec4(rotW_,
        rotX_,
        rotY_,
        rotZ_),
        vec3(1.0,
        1.0,
        1.0));
    gl_Position = camera.proj * camera.view * matrix * vec4(inPosition, 1.0);
}
