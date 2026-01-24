#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_gpu_shader_int64 : enable

layout(location = 0) in vec4 inColor;
layout(location = 1) in flat uint id;

layout(location = 0) out vec4 outColor;


layout(set = 1, binding = 0) buffer Data {
    int cursorPosX_;
    int cursorPosY_;
    uint64_t selectedId_;
} data;

void main() {

    ivec2 fragmentPos = ivec2(gl_FragCoord.xy);

    if(fragmentPos.x == data.cursorPosX_ && fragmentPos.y == data.cursorPosY_) {
        data.selectedId_ = id;
    }


    outColor = inColor;
}
