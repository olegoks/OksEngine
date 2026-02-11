#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_gpu_shader_int64 : enable

#extension GL_GOOGLE_include_directive : require

#include "/common/OksEngine.Common.Math3D.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 1) in flat uint id;

layout(location = 0) out vec4 outColor;


layout(set = 1, binding = 0) buffer Data {
    int cursorPosX_;
    int cursorPosY_;
    uint64_t potencialSelectedId_;
    uint64_t selectedIds[16];
} data;

void main() {

    ivec2 fragmentPos = ivec2(gl_FragCoord.xy);

    if(fragmentPos.x == data.cursorPosX_ && fragmentPos.y == data.cursorPosY_) {
        if(id != 0){
            data.potencialSelectedId_ = id;
        }
        //LOG_FMSG_3("Selected id %d, frag x %d, frag y %d", id, fragmentPos.x, fragmentPos.y);
    }

    outColor = inColor;
}
