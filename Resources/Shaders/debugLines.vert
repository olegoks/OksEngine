#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
} camera;

void main() {

    fragColor = inColor;

    gl_Position = camera.proj * camera.view * vec4(inPosition, 1.0);
}
