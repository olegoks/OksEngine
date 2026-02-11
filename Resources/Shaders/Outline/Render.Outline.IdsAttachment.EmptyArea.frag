#version 450
#extension GL_ARB_gpu_shader_int64 : enable
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) buffer Data {
    int cursorPosX_;
    int cursorPosY_;
    uint64_t potencialSelectedId_;
    uint64_t selectedIds[16];
} data;

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

    ivec2 fragmentPos = ivec2(gl_FragCoord.xy);

    if(fragmentPos.x == data.cursorPosX_ && fragmentPos.y == data.cursorPosY_) {
        data.potencialSelectedId_ = 1;
        //LOG_FMSG_3("Selected id %d, frag x %d, frag y %d", id, fragmentPos.x, fragmentPos.y);
    }

    outColor = idToColor(1);
}