#version 450
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_GOOGLE_include_directive : require
#include "/common/OksEngine.Common.Math3D.glsl"


layout(location = 0) in vec2 inPosition;

layout(location = 0) out vec4 outColor;
layout(location = 1) out flat uint id;

layout(set = 1, binding = 0) uniform Transform {
    vec2 scale;
    vec2 translate;
} transform;


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
    outColor = idToColor(uint(0));
    id = uint(0);
    // outColor = vec4(
    //     float(id_) / 255.0,  // R
    //     1.0 - (float(id_) / 255.0),                      // G
    //     0.0,                      // B
    //     1.0                       // A
    // );

    const vec2 posTransformed = inPosition * transform.scale + transform.translate;
    gl_Position =  vec4(posTransformed, 0.0, 1.0);
}
