#version 450
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : require

#include "/common/OksEngine.Common.Math3D.glsl"
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(set = 1, binding = 0) buffer Data {
    int cursorPosX_;
    int cursorPosY_;
    uint64_t potencialSelectedId_;
    uint64_t selectedIds_[16]; // size of array must corresponds to size in the CPU struct !!!
} data;

layout(push_constant) uniform PushConstants { 
    float outlineWidth_;
};

const float COLOR_EPSILON = 0.05; // 1% различия

bool colorsEqual(vec3 color1, vec3 color2) {
    vec3 diff = abs(color1 - color2);
    return all(lessThan(diff, vec3(COLOR_EPSILON)));
}

vec3 idToColor(uint id) {
    // Используем золотое сечение для генерации HUE
    float hue = fract(float(id) * 0.618033988749895);
    
    // HSV to RGB
    vec3 rgb = clamp(
        abs(mod(hue * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0,
        0.0, 1.0
    );
    
    // Делаем цвета насыщенными и яркими
    return vec3(rgb * 0.8 + 0.2);
}

void main() {

    bool needToDiscard = true;


    ivec2 fragmentPosCenter = ivec2(gl_FragCoord.xy);
    ivec2 fragmentPosRight = ivec2(fragmentPosCenter.x + 1, fragmentPosCenter.y);
    ivec2 fragmentPosLeft = ivec2(fragmentPosCenter.x - 1, fragmentPosCenter.y);
    ivec2 fragmentPosTop = ivec2(fragmentPosCenter.x, fragmentPosCenter.y + 1);
    ivec2 fragmentPosBottom = ivec2(fragmentPosCenter.x, fragmentPosCenter.y - 1);

    vec3 idColorCenter = texelFetch(texSampler, fragmentPosCenter, 0).rgb;
    vec3 idColorRight = texelFetch(texSampler, fragmentPosRight, 0).rgb;
    vec3 idColorLeft = texelFetch(texSampler, fragmentPosLeft, 0).rgb;
    vec3 idColorTop = texelFetch(texSampler, fragmentPosTop, 0).rgb;
    vec3 idColorBottom = texelFetch(texSampler, fragmentPosBottom, 0).rgb;
    
    for(int i = 0; i < 16; i++){
        if(uint(data.selectedIds_[i]) == 0) {
            continue;
        }
        vec3 selectedColor = idToColor(uint(data.selectedIds_[i]));

        if(colorsEqual(selectedColor, idColorCenter)) {
            if(!colorsEqual(selectedColor, idColorRight)) {
                outColor = vec4(1.0, 1.0, 0.0, 1.0);
                needToDiscard = false;
            } else if(!colorsEqual(selectedColor, idColorLeft)) {
                outColor = vec4(1.0, 1.0, 0.0, 1.0);
                needToDiscard = false;
            } else if(!colorsEqual(selectedColor, idColorTop)) {
                outColor = vec4(1.0, 1.0, 0.0, 1.0);
                needToDiscard = false;
            } else if(!colorsEqual(selectedColor, idColorBottom)) {
                outColor = vec4(1.0, 1.0, 0.0, 1.0);
                needToDiscard = false;
            } else {
                //needToDiscard = true;
            }
        }
        // } else {
            
        // }
    }

    if(needToDiscard){
        discard;
    }
    //outColor = vec4(0.0, 0.0, 0.0, 1.0);
   
}
