#version 450

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

layout(set = 1, binding = 0) uniform Transform {
    mat4 model;
} transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 outUV;

void main() {
    gl_Position = camera.proj * camera.view * transform.model * vec4(inPosition, 1.0);
    outUV = inUV;
}