#version 450 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;
layout(set = 0, binding = 0) uniform Transform {
    mat3 model;
} transform;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

void main()
{
    Out.Color = aColor;
    Out.UV = aUV;
    const vec3 pos3 = transform.model * vec3(aPos, 0);
    gl_Position = vec4( pos3, 1);
}