#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
layout(set = 0, binding = 0) uniform Transform {
    vec2 scale;
    vec2 translate;
} transform;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
    Out.Color = aColor;
    Out.UV = aUV;
    //const vec2 scale = vec2(transform.model[0][0], transform.model[0][1]);
    //const vec2 translate = vec2(transform.model[1][0], transform.model[1][1]);
    const vec2 posTransformed = aPos * transform.scale + transform.translate;
    //const vec2 posTransformed = aPos + translate;
    gl_Position = vec4(posTransformed, 0, 1);
}