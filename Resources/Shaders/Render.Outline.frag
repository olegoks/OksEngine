#version 450
#extension GL_ARB_gpu_shader_int64 : enable

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(set = 1, binding = 0) buffer Data {
    int cursorPosX_;
    int cursorPosY_;
    uint64_t selectedId_;
} data;

layout(push_constant) uniform PushConstants { 
    float outlineWidth_;
};

const float COLOR_EPSILON = 0.01; // 1% различия

bool colorsEqual(vec3 color1, vec3 color2) {
    vec3 diff = abs(color1 - color2);
    return all(lessThan(diff, vec3(COLOR_EPSILON)));
}

void main() {

    float hue = fract(float(data.selectedId_) * 0.618033988749895);
    
    // HSV to RGB
    vec3 rgb = clamp(
        abs(mod(hue * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
        0.0, 1.0
    );

    ivec2 fragmentPos = ivec2(gl_FragCoord.xy);

    vec3 idColor = texture(texSampler, gl_FragCoord.xy).rgb;


    
    //outColor = inColor;
}
